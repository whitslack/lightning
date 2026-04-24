#include "config.h"
#include <ccan/err/err.h>
#include <ccan/io/io.h>
#include <ccan/read_write_all/read_write_all.h>
#include <ccan/str/hex/hex.h>
#include <ccan/tal/link/link.h>
#include <ccan/tal/str/str.h>
#include <common/clock_time.h>
#include <common/configvar.h>
#include <common/json_command.h>
#include <common/memleak.h>
#include <errno.h>
#include <fcntl.h>
#include <lightningd/log.h>
#include <lightningd/notification.h>
#include <stdio.h>

/* What logging level to use if they didn't specify */
#define DEFAULT_LOGLEVEL LOG_INFORM

/* Once we're up and running, this is set up. */
struct logger *crashlog;

struct print_filter {
	/* In list log_book->print_filters / log_file->print_filters */
	struct list_node list;

	const char *prefix;
	enum log_level level;
};

struct log_file {
	struct list_head print_filters;
	FILE *f;
};

struct log_book {
	struct list_head print_filters;

	/* Non-null once it's been initialized */
	enum log_level *default_print_level;
	struct timeabs init_time;

	/* Our loggers */
	struct list_head loggers;

	/* Array of log files: one per ld->logfiles[] */
	struct log_file **log_files;
	bool print_timestamps;

	/* Prefix this to every entry as you output */
	const char *prefix;

	/* Although log_book will copy log entries to parent log_book
	 * (the log_book belongs to lightningd), a pointer to lightningd
	 *  is more directly because the notification needs ld->plugins.
	 */
	struct lightningd *ld;
};

struct logger {
	/* Inside log_book->loggers. */
	struct list_node list;
	struct log_book *log_book;
	const struct node_id *default_node_id;
	struct log_prefix *prefix;

	/* Print log message at >= this level */
	enum log_level print_level;
	/* For non-trivial setups, we might need to test filters again
	 * when actually producing output. */
	bool need_refiltering;
};

static struct log_prefix *log_prefix_new(const tal_t *ctx,
					 const char *prefix TAKES)
{
	struct log_prefix *lp = tal(ctx, struct log_prefix);
	lp->refcnt = 1;
	lp->prefix = tal_strdup(lp, prefix);
	return lp;
}

static void log_prefix_drop(struct log_prefix *lp)
{
	if (--lp->refcnt == 0)
		tal_free(lp);
}

static struct log_prefix *log_prefix_get(struct log_prefix *lp)
{
	assert(lp->refcnt);
	lp->refcnt++;
	return lp;
}

static const char *level_prefix(enum log_level level)
{
	switch (level) {
	case LOG_IO_OUT:
	case LOG_IO_IN:
		return "IO     ";
	case LOG_TRACE:
		return "TRACE  ";
	case LOG_DBG:
		return "DEBUG  ";
	case LOG_INFORM:
		return "INFO   ";
	case LOG_UNUSUAL:
		return "UNUSUAL";
	case LOG_BROKEN:
		return "**BROKEN**";
	}
	abort();
}

/* What do these filters say about level to log this entry at? */
static bool filter_level(const struct list_head *print_filters,
			 const char *prefix,
			 const char *node_id_str,
			 enum log_level *level)
{
	struct print_filter *i;

	list_for_each(print_filters, i, list) {
		if (strstr(prefix, i->prefix) || strstr(node_id_str, i->prefix)) {
			*level = i->level;
			return true;
		}
	}
	return false;
}

/* What's the lowest filtering which could possibly apply? */
static void lowest_filter(const struct list_head *print_filters,
			  const char *prefix,
			  const struct node_id *node_id,
			  enum log_level *level)
{
	struct print_filter *i;
	const char *node_id_str;

	if (node_id)
		node_id_str = fmt_node_id(tmpctx, node_id);
	else
		node_id_str = NULL;

	list_for_each(print_filters, i, list) {
		bool match;

		if (strstr(prefix, i->prefix))
			match = true;
		else if (node_id_str) {
			match = (strstr(node_id_str, i->prefix) != NULL);
		} else {
			/* Could this possibly match a node_id? */
			match = strstarts(i->prefix, "02") || strstarts(i->prefix, "03");
		}

		if (match && i->level < *level) {
			*level = i->level;
		}
	}
}

static void log_to_files(const char *log_prefix,
			 const char *entry_prefix,
			 enum log_level level,
			 /* The node_id to log under. */
			 const struct node_id *node_id,
			 /* Filters to apply, if non-NULL */
			 const struct list_head *print_filters,
			 const struct timeabs *time,
			 const char *str,
			 const u8 *io,
			 size_t io_len,
			 bool print_timestamps,
			 const enum log_level *default_print_level,
			 struct log_file **log_files)
{
	char tstamp[sizeof("YYYY-mm-ddTHH:MM:SS.nnnZ ")];
	char *entry, nodestr[hex_str_size(PUBKEY_CMPR_LEN)];
	char buf[sizeof("%s%s%s %s-%s: %s\n")
		 + strlen(log_prefix)
		 + sizeof(tstamp)
		 + strlen(level_prefix(level))
		 + sizeof(nodestr)
		 + strlen(entry_prefix)
		 + strlen(str)];
	bool filtered;

	if (print_timestamps) {
		char iso8601_msec_fmt[sizeof("YYYY-mm-ddTHH:MM:SS.%03dZ ")];
		strftime(iso8601_msec_fmt, sizeof(iso8601_msec_fmt), "%FT%T.%%03dZ ", gmtime(&time->ts.tv_sec));
		snprintf(tstamp, sizeof(tstamp), iso8601_msec_fmt, (int) time->ts.tv_nsec / 1000000);
	} else
		tstamp[0] = '\0';

	if (node_id)
		hex_encode(node_id->k, sizeof(node_id->k),
			   nodestr, sizeof(nodestr));
	else
		nodestr[0] = '\0';
	if (level == LOG_IO_IN || level == LOG_IO_OUT) {
		const char *dir = level == LOG_IO_IN ? "[IN]" : "[OUT]";
		char *hex = tal_hexstr(NULL, io, io_len);
		if (!node_id)
			entry = tal_fmt(tmpctx, "%s%s%s: %s%s %s\n",
					log_prefix, tstamp, entry_prefix, str, dir, hex);
		else
			entry = tal_fmt(tmpctx, "%s%s%s-%s: %s%s %s\n",
					log_prefix, tstamp,
					nodestr,
					entry_prefix, str, dir, hex);
		tal_free(hex);
	} else {
		size_t len;
		entry = buf;
		if (!node_id)
			len = snprintf(buf, sizeof(buf),
				       "%s%s%s %s: %s\n",
				       log_prefix, tstamp, level_prefix(level), entry_prefix, str);
		else
			len = snprintf(buf, sizeof(buf), "%s%s%s %s-%s: %s\n",
				       log_prefix, tstamp, level_prefix(level),
				       nodestr,
				       entry_prefix, str);
		assert(len < sizeof(buf));
	}

	/* In complex configurations, we tell loggers to overshare: then we
	 * need to filter here to see if we really want it. */
	filtered = false;
	if (print_filters) {
		enum log_level filter;
		if (filter_level(print_filters,
				 entry_prefix, nodestr, &filter)) {
			if (level < filter)
				return;
			/* Even if they specify a default filter level of 'INFO', this overrides */
			filtered = true;
		}
	}

	/* Default if nothing set is stdout */
	if (!log_files) {
		fwrite(entry, strlen(entry), 1, stdout);
		fflush(stdout);
	}

	/* We may have to apply per-file filters. */
	for (size_t i = 0; i < tal_count(log_files); i++) {
		enum log_level filter;
		if (!filter_level(&log_files[i]->print_filters,
				  entry_prefix, nodestr, &filter)) {
			/* If we haven't set default yet, only log UNUSUAL */
			if (!default_print_level)
				filter = LOG_UNUSUAL;
			else {
				/* If we've filtered it already, it passes */
				if (filtered)
					filter = level;
				else
					filter = *default_print_level;
			}
		}
		if (level < filter)
			continue;
		fwrite(entry, strlen(entry), 1, log_files[i]->f);
		fflush(log_files[i]->f);
	}
}

static void destroy_entry(struct log_entry *i)
{
	free(i->log), i->log = NULL;
	log_prefix_drop(i->prefix), i->prefix = NULL;
	tal_free(i->io), i->io = NULL;
}

struct log_book *new_log_book(struct lightningd *ld, size_t max_mem UNUSED)
{
	struct log_book *log_book = tal_linkable(tal(NULL, struct log_book));

	log_book->log_files = NULL;
	log_book->default_print_level = NULL;
	/* We have to allocate this, since we tal_free it on resetting */
	log_book->prefix = tal_strdup(log_book, "");
	list_head_init(&log_book->print_filters);
	list_head_init(&log_book->loggers);
	log_book->init_time = clock_time();
	log_book->ld = ld;
	log_book->print_timestamps = true;

	return log_book;
}

/* What's the minimum level to print this prefix and node_id for this
 * log book?  Saves us marshalling long print lines in most cases. */
static enum log_level print_level(struct log_book *log_book,
				  const struct log_prefix *lp,
				  const struct node_id *node_id,
				  bool *need_refiltering)
{
	enum log_level level = *log_book->default_print_level;
	bool have_filters = false;

	lowest_filter(&log_book->print_filters, lp->prefix, node_id, &level);
	if (!list_empty(&log_book->print_filters))
		have_filters = true;

	/* We need to look into per-file filters as well: might give a
	 * lower filter! */
	for (size_t i = 0; i < tal_count(log_book->log_files); i++) {
		lowest_filter(&log_book->log_files[i]->print_filters,
			      lp->prefix, node_id, &level);
		if (!list_empty(&log_book->log_files[i]->print_filters))
			have_filters = true;
	}

	/* Almost any complex array of filters can mean we want to re-check
	 * when logging. */
	if (need_refiltering)
		*need_refiltering = have_filters;

	return level;
}

static void destroy_logger(struct logger *log)
{
	list_del_from(&log->log_book->loggers, &log->list);
}

/* With different entry points */
struct logger *
new_logger(const tal_t *ctx, struct log_book *log_book,
	   const struct node_id *default_node_id,
	   const char *fmt, ...)
{
	struct logger *log = tal(ctx, struct logger);
	va_list ap;

	log->log_book = tal_link(log, log_book);
	va_start(ap, fmt);
	/* Owned by the log book itself, since it can be referenced
	 * by log entries, too */
	log->prefix = log_prefix_new(log->log_book, take(tal_vfmt(NULL, fmt, ap)));
	va_end(ap);
	log->default_node_id = tal_dup_or_null(log, struct node_id,
					       default_node_id);

	/* Still initializing?  Print UNUSUAL / BROKEN messages only  */
	if (!log->log_book->default_print_level) {
		log->print_level = LOG_UNUSUAL;
		log->need_refiltering = false;
	} else {
		log->print_level = print_level(log->log_book,
					       log->prefix,
					       default_node_id,
					       &log->need_refiltering);
	}
	list_add(&log->log_book->loggers, &log->list);
	tal_add_destructor(log, destroy_logger);
	return log;
}

const char *log_prefix(const struct logger *log)
{
	return log->prefix->prefix;
}

bool log_has_io_logging(const struct logger *log)
{
	return print_level(log->log_book, log->prefix, log->default_node_id, NULL) < LOG_TRACE;
}

bool log_has_trace_logging(const struct logger *log)
{
	return print_level(log->log_book, log->prefix, log->default_node_id, NULL) < LOG_DBG;
}

static struct log_entry *construct_entry(struct logger *log, struct log_entry *l,
				       enum log_level level,
				       const struct node_id *node_id)
{
	l->time = clock_time();
	l->level = level;
	l->skipped = 0;
	l->prefix = log_prefix_get(log->prefix);
	l->log = NULL;
	l->io = NULL;
	if (!node_id)
		node_id = log->default_node_id;
	l->node_id = node_id;

	return l;
}

static void maybe_print(struct logger *log, const struct log_entry *l)
{
	if (l->level >= log->print_level)
		log_to_files(log->log_book->prefix, log->prefix->prefix, l->level,
			     l->node_id,
			     log->need_refiltering ? &log->log_book->print_filters : NULL,
			     &l->time, l->log,
			     l->io, tal_bytelen(l->io),
			     log->log_book->print_timestamps,
			     log->log_book->default_print_level,
			     log->log_book->log_files);
}

static void maybe_notify_log(struct logger *log,
			     const struct log_entry *l)
{
	if (l->level >= log->print_level)
		notify_log(log->log_book->ld, l);
}

void logv(struct logger *log, enum log_level level,
	  const struct node_id *node_id,
	  bool call_notifier,
	  const char *fmt, va_list ap)
{
	int save_errno = errno;
	struct log_entry l_, *const l = construct_entry(log, &l_, level, node_id);

	/* This is WARN_UNUSED_RESULT, because everyone should somehow deal
	 * with OOM, even though nobody does. */
	if (vasprintf(&l->log, fmt, ap) == -1)
		abort();

	size_t log_len = strlen(l->log);

	/* Sanitize any non-printable characters, and replace with '?' */
	for (size_t i=0; i<log_len; i++)
		if (l->log[i] < ' ' || l->log[i] >= 0x7f)
			l->log[i] = '?';

	maybe_print(log, l);
	maybe_notify_log(log, l);

	if (call_notifier)
		notify_warning(log->log_book->ld, l);

	destroy_entry(l);

	errno = save_errno;
}

void log_io(struct logger *log, enum log_level dir,
	    const struct node_id *node_id,
	    const char *str TAKES,
	    const void *data TAKES, size_t len)
{
	int save_errno = errno;

	assert(dir == LOG_IO_IN || dir == LOG_IO_OUT);

	/* Print first, in case we need to truncate. */
	if (dir >= log->print_level) {
		struct timeabs time = time_now();
		log_to_files(log->log_book->prefix, log->prefix->prefix, dir,
			     node_id,
			     log->need_refiltering ? &log->log_book->print_filters : NULL,
			     &time, str,
			     data, len,
			     log->log_book->print_timestamps,
			     log->log_book->default_print_level,
			     log->log_book->log_files);
	}

	if (taken(str))
		tal_free(str);

	if (taken(data))
		tal_free(data);

	errno = save_errno;
}

void log_(struct logger *log, enum log_level level,
	  const struct node_id *node_id,
	  bool call_notifier,
	  const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logv(log, level, node_id, call_notifier, fmt, ap);
	va_end(ap);
}

static struct log_file *find_log_file(struct log_book *log_book,
				      const char *fname)
{
	assert(tal_count(log_book->log_files)
	       == tal_count(log_book->ld->logfiles));
	for (size_t i = 0; i < tal_count(log_book->log_files); i++) {
		if (streq(log_book->ld->logfiles[i], fname))
			return log_book->log_files[i];
	}
	return NULL;
}

char *opt_log_level(const char *arg, struct log_book *log_book)
{
	enum log_level level;
	int len;

	len = strcspn(arg, ":");
	if (!log_level_parse(arg, len, &level))
		return tal_fmt(tmpctx, "unknown log level %.*s", len, arg);

	if (arg[len]) {
		struct print_filter *f = tal(log_book, struct print_filter);
		f->prefix = arg + len + 1;
		f->level = level;

		/* :<filename> */
		len = strcspn(f->prefix, ":");
		if (f->prefix[len]) {
			struct log_file *lf;
			lf = find_log_file(log_book, f->prefix + len + 1);
			if (!lf)
				return tal_fmt(tmpctx,
					       "unknown log file %s",
					       f->prefix + len + 1);
			f->prefix = tal_strndup(f, f->prefix, len);
			list_add_tail(&lf->print_filters, &f->list);
		} else {
			list_add_tail(&log_book->print_filters, &f->list);
		}
	} else {
		tal_free(log_book->default_print_level);
		log_book->default_print_level = tal(log_book, enum log_level);
		*log_book->default_print_level = level;
	}
	return NULL;
}

void json_add_opt_log_levels(struct json_stream *response, struct log_book *log_book)
{
	struct print_filter *i;

	list_for_each(&log_book->print_filters, i, list) {
		json_add_str_fmt(response, "log-level", "%s:%s",
				 log_level_name(i->level), i->prefix);
	}
}

static bool show_log_level(char *buf, size_t len, const struct log_book *log_book)
{
	enum log_level l;

	if (log_book->default_print_level)
		l = *log_book->default_print_level;
	else
		l = DEFAULT_LOGLEVEL;
	strncpy(buf, log_level_name(l), len);
	return true;
}

static char *arg_log_prefix(const char *arg, struct log_book *log_book)
{
	tal_free(log_book->prefix);
	log_book->prefix = tal_strdup(log_book, arg);
	return NULL;
}

static bool show_log_prefix(char *buf, size_t len, const struct log_book *log_book)
{
	strncpy(buf, log_book->prefix, len);
	/* Default is empty, so don't print that! */
	return !streq(log_book->prefix, "");
}

static int signalfds[2];

static void handle_sighup(int sig)
{
	/* Writes a single 0x00 byte to the signalfds pipe. This may fail if
	 * we're hammered with SIGHUP.  We don't care. */
	if (write(signalfds[1], "", 1))
		;
}

/* Mutual recursion */
static struct io_plan *setup_read(struct io_conn *conn, struct lightningd *ld);

static struct io_plan *rotate_log(struct io_conn *conn, struct lightningd *ld)
{
	log_info(ld->log, "Ending log due to SIGHUP");
	for (size_t i = 0; i < tal_count(ld->log->log_book->log_files); i++) {
		if (streq(ld->logfiles[i], "-"))
			continue;
		fclose(ld->log->log_book->log_files[i]->f);
		ld->log->log_book->log_files[i]->f = fopen(ld->logfiles[i], "a");
		if (!ld->log->log_book->log_files[i]->f)
			err(1, "failed to reopen log file %s", ld->logfiles[i]);
	}

	log_info(ld->log, "Started log due to SIGHUP");
	return setup_read(conn, ld);
}

static struct io_plan *setup_read(struct io_conn *conn, struct lightningd *ld)
{
	/* We read and discard. */
	static char discard;
	return io_read(conn, &discard, 1, rotate_log, ld);
}

static void setup_log_rotation(struct lightningd *ld)
{
	struct sigaction act;
	if (pipe(signalfds) != 0)
		errx(1, "Pipe for signalfds");

	notleak(io_new_conn(ld, signalfds[0], setup_read, ld));

	io_fd_block(signalfds[1], false);
	memset(&act, 0, sizeof(act));
	act.sa_handler = handle_sighup;
	/* We do not need any particular flags; the sigaction
	 * default behavior (EINTR any system calls, pass only
	 * the signo to the handler, retain the same signal
	 * handler throughout) is fine with us.
	 */
	act.sa_flags = 0;
	/* Block all signals while handling SIGHUP.
	 * Without this, e.g. an inopportune SIGCHLD while we
	 * are doing a `write` to the SIGHUP signal pipe could
	 * prevent us from sending the byte and performing the
	 * log rotation in the main loop.
	 *
	 * The SIGHUP handler does very little anyway, and
	 * the blocked signals will get delivered soon after
	 * the SIGHUP handler returns.
	 */
	sigfillset(&act.sa_mask);

	if (sigaction(SIGHUP, &act, NULL) != 0)
		err(1, "Setting up SIGHUP handler");
}

char *arg_log_to_file(const char *arg, struct lightningd *ld)
{
	int size;
	struct log_file *logf;

	if (!ld->logfiles) {
		setup_log_rotation(ld);
		ld->logfiles = tal_arr(ld, const char *, 0);
		ld->log_book->log_files = tal_arr(ld->log_book, struct log_file *, 0);
	}

	logf = tal(ld->log_book->log_files, struct log_file);
	list_head_init(&logf->print_filters);
	if (streq(arg, "-"))
		logf->f = stdout;
	else {
		logf->f = fopen(arg, "a");
		if (!logf->f)
			return tal_fmt(tmpctx, "Failed to open: %s", strerror(errno));
	}

	tal_arr_expand(&ld->logfiles, tal_strdup(ld->logfiles, arg));
	tal_arr_expand(&ld->log_book->log_files, logf);

	/* For convenience make a block of empty lines just like Bitcoin Core */
	size = ftell(logf->f);
	if (size > 0)
		fprintf(logf->f, "\n\n\n\n");

	log_debug(ld->log, "Opened log file %s", arg);
	return NULL;
}

void opt_register_logging(struct lightningd *ld)
{
	opt_register_early_arg("--log-level",
			       opt_log_level, show_log_level, ld->log_book,
			       "log level (io, debug, info, unusual, broken) [:prefix]");
	clnopt_witharg("--log-timestamps", OPT_EARLY|OPT_SHOWBOOL,
		       opt_set_bool_arg, opt_show_bool,
		       &ld->log_book->print_timestamps,
		       "prefix log messages with timestamp");
	clnopt_witharg("--log-prefix", OPT_EARLY|OPT_KEEP_WHITESPACE,
		       arg_log_prefix, show_log_prefix, ld->log_book, "log prefix");
	clnopt_witharg("--log-file=<file>",
		       OPT_EARLY|OPT_MULTI,
		       arg_log_to_file, NULL, ld,
		       "Also log to file (- for stdout)");
}

void logging_options_parsed(struct log_book *log_book)
{
	struct logger *log;

	/* If they didn't set an explicit level, set to info */
	if (!log_book->default_print_level) {
		log_book->default_print_level = tal(log_book, enum log_level);
		*log_book->default_print_level = DEFAULT_LOGLEVEL;
	}

	/* Set print_levels for each log, depending on filters. */
	list_for_each(&log_book->loggers, log, list) {
		log->print_level = print_level(log_book,
					       log->prefix,
					       log->default_node_id,
					       &log->need_refiltering);
	}
}

void log_backtrace_print(const char *fmt, ...)
{
	va_list ap;

	if (!crashlog)
		return;

	va_start(ap, fmt);
	logv(crashlog, LOG_BROKEN, NULL, false, fmt, ap);
	va_end(ap);
}

void log_backtrace_exit(void)
{
}

void fatal_vfmt(const char *fmt, va_list ap)
{
	va_list ap2;

	/* You are not allowed to re-use va_lists, so make a copy. */
	va_copy(ap2, ap);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");

	if (!crashlog)
		exit(1);

	logv(crashlog, LOG_BROKEN, NULL, true, fmt, ap2);
	abort();
	/* va_copy() must be matched with va_end(), even if unreachable. */
	va_end(ap2);
}

void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fatal_vfmt(fmt, ap);
	va_end(ap);
}
