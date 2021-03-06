/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: lightning.proto */

#ifndef PROTOBUF_C_lightning_2eproto__INCLUDED
#define PROTOBUF_C_lightning_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1001001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Sha256Hash Sha256Hash;
typedef struct _BitcoinOutputId BitcoinOutputId;
typedef struct _BitcoinOutput BitcoinOutput;
typedef struct _BitcoinSignature BitcoinSignature;
typedef struct _Anchor Anchor;
typedef struct _OpenChannel OpenChannel;
typedef struct _OpenCommitSig OpenCommitSig;
typedef struct _OpenAnchorSig OpenAnchorSig;
typedef struct _OpenComplete OpenComplete;
typedef struct _Update Update;
typedef struct _UpdateAccept UpdateAccept;
typedef struct _UpdateComplete UpdateComplete;
typedef struct _NewAnchor NewAnchor;
typedef struct _NewAnchorAck NewAnchorAck;
typedef struct _NewAnchorAccept NewAnchorAccept;
typedef struct _NewAnchorComplete NewAnchorComplete;
typedef struct _CloseChannel CloseChannel;
typedef struct _CloseChannelComplete CloseChannelComplete;
typedef struct _Error Error;
typedef struct _Pkt Pkt;


/* --- enums --- */


/* --- messages --- */

/*
 * Protobufs don't have fixed-length fields, so this is a hack.
 */
struct  _Sha256Hash
{
  ProtobufCMessage base;
  uint64_t a;
  uint64_t b;
  uint64_t c;
  uint64_t d;
};
#define SHA256_HASH__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&sha256_hash__descriptor) \
    , 0, 0, 0, 0 }


/*
 * Identifies a bitcoin output.
 */
struct  _BitcoinOutputId
{
  ProtobufCMessage base;
  /*
   * This is the transaction ID.
   */
  Sha256Hash *txid;
  /*
   * This is the output number.
   */
  uint32_t output;
};
#define BITCOIN_OUTPUT_ID__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&bitcoin_output_id__descriptor) \
    , NULL, 0 }


/*
 * A bitcoin output
 */
struct  _BitcoinOutput
{
  ProtobufCMessage base;
  uint64_t amount;
  ProtobufCBinaryData script;
};
#define BITCOIN_OUTPUT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&bitcoin_output__descriptor) \
    , 0, {0,NULL} }


/*
 * A signature to use for a transaction; DER encoded with sigtype at the end.
 */
struct  _BitcoinSignature
{
  ProtobufCMessage base;
  ProtobufCBinaryData der_then_sigtype;
};
#define BITCOIN_SIGNATURE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&bitcoin_signature__descriptor) \
    , {0,NULL} }


/*
 * All about an anchor transaction.
 */
struct  _Anchor
{
  ProtobufCMessage base;
  /*
   * 0 or more unspent inputs we want to use for anchor.
   */
  size_t n_inputs;
  BitcoinOutputId **inputs;
  /*
   * Any change from anchor (in case we don't want to use them all)
   */
  BitcoinOutput *anchor_change;
  /*
   * How much transaction fee we'll pay in the anchor tx.
   */
  uint64_t fee;
  /*
   * How many confirmations on anchor before we'll use channel.
   */
  uint32_t min_confirms;
};
#define ANCHOR__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&anchor__descriptor) \
    , 0,NULL, NULL, 0, 0 }


typedef enum {
  OPEN_CHANNEL__LOCKTIME__NOT_SET = 0,
  OPEN_CHANNEL__LOCKTIME_LOCKTIME_SECONDS = 2,
  OPEN_CHANNEL__LOCKTIME_LOCKTIME_BLOCKS = 3,
} OpenChannel__LocktimeCase;

/*
 * Set channel params.
 */
struct  _OpenChannel
{
  ProtobufCMessage base;
  /*
   * Seed which sets order we create outputs for all transactions.
   */
  uint64_t seed;
  /*
   * Hash seed for revoking commitment transactions.
   */
  Sha256Hash *revocation_hash;
  /*
   * How to pay money to us.
   */
  ProtobufCBinaryData script_to_me;
  /*
   * How much transaction fee we'll pay for commitment txs.
   */
  uint64_t commitment_fee;
  /*
   * The anchor transaction details.
   */
  Anchor *anchor;
  OpenChannel__LocktimeCase locktime_case;
  union {
    uint32_t locktime_seconds;
    uint32_t locktime_blocks;
  };
};
#define OPEN_CHANNEL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&open_channel__descriptor) \
    , 0, NULL, {0,NULL}, 0, NULL, OPEN_CHANNEL__LOCKTIME__NOT_SET, {} }


/*
 * Supply signature for commitment tx
 */
struct  _OpenCommitSig
{
  ProtobufCMessage base;
  BitcoinSignature *sig;
};
#define OPEN_COMMIT_SIG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&open_commit_sig__descriptor) \
    , NULL }


/*
 * Supply signature for anchor tx
 */
struct  _OpenAnchorSig
{
  ProtobufCMessage base;
  BitcoinSignature *sig;
};
#define OPEN_ANCHOR_SIG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&open_anchor_sig__descriptor) \
    , NULL }


/*
 * Indicates we've seen transaction reach min-depth.
 */
struct  _OpenComplete
{
  ProtobufCMessage base;
  /*
   * Block it went into.
   */
  /*
   * FIXME: add a merkle proof plus block headers here?
   */
  Sha256Hash *blockid;
};
#define OPEN_COMPLETE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&open_complete__descriptor) \
    , NULL }


/*
 * Let's spend some money in the channel!
 */
struct  _Update
{
  ProtobufCMessage base;
  /*
   * Hash for which I will supply preimage to revoke this.
   */
  Sha256Hash *revocation_hash;
  /*
   * Change in current payment to-me (implies reverse to-you).
   */
  int64_t delta;
  /*
   * Signature for new commitment tx.
   */
  BitcoinSignature *sig;
  /*
   * Signature for old anchor (if any)
   */
  /*
   * FIXME: optional HTLC ops.
   */
  BitcoinSignature *old_anchor_sig;
};
#define UPDATE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&update__descriptor) \
    , NULL, 0, NULL, NULL }


/*
 * OK, I accept that update.
 */
struct  _UpdateAccept
{
  ProtobufCMessage base;
  /*
   * Signature for new commitment tx.
   */
  BitcoinSignature *sig;
  /*
   * Signature for old anchor (if any)
   */
  BitcoinSignature *old_anchor_sig;
  /*
   * Hash preimage which revokes old commitment tx.
   */
  Sha256Hash *revocation_preimage;
};
#define UPDATE_ACCEPT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&update_accept__descriptor) \
    , NULL, NULL, NULL }


/*
 * Complete the update.
 */
struct  _UpdateComplete
{
  ProtobufCMessage base;
  /*
   * Hash preimage which revokes old commitment tx.
   */
  Sha256Hash *revocation_preimage;
};
#define UPDATE_COMPLETE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&update_complete__descriptor) \
    , NULL }


/*
 * Let's change the channel funding source.
 */
struct  _NewAnchor
{
  ProtobufCMessage base;
  /*
   * The new anchor: previous anchor 2x2 input assumed.
   */
  Anchor *anchor;
};
#define NEW_ANCHOR__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&new_anchor__descriptor) \
    , NULL }


/*
 * That seems OK to me!
 */
struct  _NewAnchorAck
{
  ProtobufCMessage base;
  BitcoinSignature *sig;
};
#define NEW_ANCHOR_ACK__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&new_anchor_ack__descriptor) \
    , NULL }


/*
 * Here's my signature on the new anchor to complete it.
 */
struct  _NewAnchorAccept
{
  ProtobufCMessage base;
  BitcoinSignature *sig;
};
#define NEW_ANCHOR_ACCEPT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&new_anchor_accept__descriptor) \
    , NULL }


/*
 * Complete the transfer to new anchor (both ends need to send this,
 * once they're happy that it's reached their required depth).
 */
struct  _NewAnchorComplete
{
  ProtobufCMessage base;
  Sha256Hash *revocation_preimage;
};
#define NEW_ANCHOR_COMPLETE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&new_anchor_complete__descriptor) \
    , NULL }


/*
 * Begin cooperative close of channel.
 */
struct  _CloseChannel
{
  ProtobufCMessage base;
  /*
   * This is our signature a new transaction which spends my current
   * commitment tx output 0 (which is 2/2) to script_to_me.
   */
  BitcoinSignature *sig;
};
#define CLOSE_CHANNEL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&close_channel__descriptor) \
    , NULL }


/*
 * OK, here's my sig so you can broadcast it too.
 */
struct  _CloseChannelComplete
{
  ProtobufCMessage base;
  /*
   * This is our signature a new transaction which spends your current
   * commitment tx output 0 (which is 2/2) to your script_to_me.
   */
  BitcoinSignature *sig;
};
#define CLOSE_CHANNEL_COMPLETE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&close_channel_complete__descriptor) \
    , NULL }


/*
 * This means we're going to hang up; it's to help diagnose only! 
 */
struct  _Error
{
  ProtobufCMessage base;
  char *problem;
};
#define ERROR__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&error__descriptor) \
    , NULL }


typedef enum {
  PKT__PKT__NOT_SET = 0,
  PKT__PKT_OPEN = 201,
  PKT__PKT_OPEN_COMMIT_SIG = 202,
  PKT__PKT_OPEN_ANCHOR_SIG = 203,
  PKT__PKT_OPEN_COMPLETE = 204,
  PKT__PKT_UPDATE = 1,
  PKT__PKT_UPDATE_ACCEPT = 2,
  PKT__PKT_UPDATE_COMPLETE = 3,
  PKT__PKT_NEW_ANCHOR = 301,
  PKT__PKT_NEW_ANCHOR_ACK = 302,
  PKT__PKT_NEW_ANCHOR_ACCEPT = 303,
  PKT__PKT_NEW_ANCHOR_COMPLETE = 304,
  PKT__PKT_CLOSE = 401,
  PKT__PKT_CLOSE_COMPLETE = 402,
  PKT__PKT_ERROR = 1000,
} Pkt__PktCase;

/*
 * This is the union which defines all of them
 */
struct  _Pkt
{
  ProtobufCMessage base;
  Pkt__PktCase pkt_case;
  union {
    /*
     * Opening
     */
    OpenChannel *open;
    OpenCommitSig *open_commit_sig;
    OpenAnchorSig *open_anchor_sig;
    OpenComplete *open_complete;
    /*
     * Updating (most common)
     */
    Update *update;
    UpdateAccept *update_accept;
    UpdateComplete *update_complete;
    /*
     * Topping up
     */
    NewAnchor *new_anchor;
    NewAnchorAck *new_anchor_ack;
    NewAnchorAccept *new_anchor_accept;
    NewAnchorComplete *new_anchor_complete;
    /*
     * Closing
     */
    CloseChannel *close;
    CloseChannelComplete *close_complete;
    /*
     * Unexpected issue.
     */
    Error *error;
  };
};
#define PKT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&pkt__descriptor) \
    , PKT__PKT__NOT_SET, {} }


/* Sha256Hash methods */
void   sha256_hash__init
                     (Sha256Hash         *message);
size_t sha256_hash__get_packed_size
                     (const Sha256Hash   *message);
size_t sha256_hash__pack
                     (const Sha256Hash   *message,
                      uint8_t             *out);
size_t sha256_hash__pack_to_buffer
                     (const Sha256Hash   *message,
                      ProtobufCBuffer     *buffer);
Sha256Hash *
       sha256_hash__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   sha256_hash__free_unpacked
                     (Sha256Hash *message,
                      ProtobufCAllocator *allocator);
/* BitcoinOutputId methods */
void   bitcoin_output_id__init
                     (BitcoinOutputId         *message);
size_t bitcoin_output_id__get_packed_size
                     (const BitcoinOutputId   *message);
size_t bitcoin_output_id__pack
                     (const BitcoinOutputId   *message,
                      uint8_t             *out);
size_t bitcoin_output_id__pack_to_buffer
                     (const BitcoinOutputId   *message,
                      ProtobufCBuffer     *buffer);
BitcoinOutputId *
       bitcoin_output_id__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   bitcoin_output_id__free_unpacked
                     (BitcoinOutputId *message,
                      ProtobufCAllocator *allocator);
/* BitcoinOutput methods */
void   bitcoin_output__init
                     (BitcoinOutput         *message);
size_t bitcoin_output__get_packed_size
                     (const BitcoinOutput   *message);
size_t bitcoin_output__pack
                     (const BitcoinOutput   *message,
                      uint8_t             *out);
size_t bitcoin_output__pack_to_buffer
                     (const BitcoinOutput   *message,
                      ProtobufCBuffer     *buffer);
BitcoinOutput *
       bitcoin_output__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   bitcoin_output__free_unpacked
                     (BitcoinOutput *message,
                      ProtobufCAllocator *allocator);
/* BitcoinSignature methods */
void   bitcoin_signature__init
                     (BitcoinSignature         *message);
size_t bitcoin_signature__get_packed_size
                     (const BitcoinSignature   *message);
size_t bitcoin_signature__pack
                     (const BitcoinSignature   *message,
                      uint8_t             *out);
size_t bitcoin_signature__pack_to_buffer
                     (const BitcoinSignature   *message,
                      ProtobufCBuffer     *buffer);
BitcoinSignature *
       bitcoin_signature__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   bitcoin_signature__free_unpacked
                     (BitcoinSignature *message,
                      ProtobufCAllocator *allocator);
/* Anchor methods */
void   anchor__init
                     (Anchor         *message);
size_t anchor__get_packed_size
                     (const Anchor   *message);
size_t anchor__pack
                     (const Anchor   *message,
                      uint8_t             *out);
size_t anchor__pack_to_buffer
                     (const Anchor   *message,
                      ProtobufCBuffer     *buffer);
Anchor *
       anchor__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   anchor__free_unpacked
                     (Anchor *message,
                      ProtobufCAllocator *allocator);
/* OpenChannel methods */
void   open_channel__init
                     (OpenChannel         *message);
size_t open_channel__get_packed_size
                     (const OpenChannel   *message);
size_t open_channel__pack
                     (const OpenChannel   *message,
                      uint8_t             *out);
size_t open_channel__pack_to_buffer
                     (const OpenChannel   *message,
                      ProtobufCBuffer     *buffer);
OpenChannel *
       open_channel__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   open_channel__free_unpacked
                     (OpenChannel *message,
                      ProtobufCAllocator *allocator);
/* OpenCommitSig methods */
void   open_commit_sig__init
                     (OpenCommitSig         *message);
size_t open_commit_sig__get_packed_size
                     (const OpenCommitSig   *message);
size_t open_commit_sig__pack
                     (const OpenCommitSig   *message,
                      uint8_t             *out);
size_t open_commit_sig__pack_to_buffer
                     (const OpenCommitSig   *message,
                      ProtobufCBuffer     *buffer);
OpenCommitSig *
       open_commit_sig__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   open_commit_sig__free_unpacked
                     (OpenCommitSig *message,
                      ProtobufCAllocator *allocator);
/* OpenAnchorSig methods */
void   open_anchor_sig__init
                     (OpenAnchorSig         *message);
size_t open_anchor_sig__get_packed_size
                     (const OpenAnchorSig   *message);
size_t open_anchor_sig__pack
                     (const OpenAnchorSig   *message,
                      uint8_t             *out);
size_t open_anchor_sig__pack_to_buffer
                     (const OpenAnchorSig   *message,
                      ProtobufCBuffer     *buffer);
OpenAnchorSig *
       open_anchor_sig__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   open_anchor_sig__free_unpacked
                     (OpenAnchorSig *message,
                      ProtobufCAllocator *allocator);
/* OpenComplete methods */
void   open_complete__init
                     (OpenComplete         *message);
size_t open_complete__get_packed_size
                     (const OpenComplete   *message);
size_t open_complete__pack
                     (const OpenComplete   *message,
                      uint8_t             *out);
size_t open_complete__pack_to_buffer
                     (const OpenComplete   *message,
                      ProtobufCBuffer     *buffer);
OpenComplete *
       open_complete__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   open_complete__free_unpacked
                     (OpenComplete *message,
                      ProtobufCAllocator *allocator);
/* Update methods */
void   update__init
                     (Update         *message);
size_t update__get_packed_size
                     (const Update   *message);
size_t update__pack
                     (const Update   *message,
                      uint8_t             *out);
size_t update__pack_to_buffer
                     (const Update   *message,
                      ProtobufCBuffer     *buffer);
Update *
       update__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   update__free_unpacked
                     (Update *message,
                      ProtobufCAllocator *allocator);
/* UpdateAccept methods */
void   update_accept__init
                     (UpdateAccept         *message);
size_t update_accept__get_packed_size
                     (const UpdateAccept   *message);
size_t update_accept__pack
                     (const UpdateAccept   *message,
                      uint8_t             *out);
size_t update_accept__pack_to_buffer
                     (const UpdateAccept   *message,
                      ProtobufCBuffer     *buffer);
UpdateAccept *
       update_accept__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   update_accept__free_unpacked
                     (UpdateAccept *message,
                      ProtobufCAllocator *allocator);
/* UpdateComplete methods */
void   update_complete__init
                     (UpdateComplete         *message);
size_t update_complete__get_packed_size
                     (const UpdateComplete   *message);
size_t update_complete__pack
                     (const UpdateComplete   *message,
                      uint8_t             *out);
size_t update_complete__pack_to_buffer
                     (const UpdateComplete   *message,
                      ProtobufCBuffer     *buffer);
UpdateComplete *
       update_complete__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   update_complete__free_unpacked
                     (UpdateComplete *message,
                      ProtobufCAllocator *allocator);
/* NewAnchor methods */
void   new_anchor__init
                     (NewAnchor         *message);
size_t new_anchor__get_packed_size
                     (const NewAnchor   *message);
size_t new_anchor__pack
                     (const NewAnchor   *message,
                      uint8_t             *out);
size_t new_anchor__pack_to_buffer
                     (const NewAnchor   *message,
                      ProtobufCBuffer     *buffer);
NewAnchor *
       new_anchor__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   new_anchor__free_unpacked
                     (NewAnchor *message,
                      ProtobufCAllocator *allocator);
/* NewAnchorAck methods */
void   new_anchor_ack__init
                     (NewAnchorAck         *message);
size_t new_anchor_ack__get_packed_size
                     (const NewAnchorAck   *message);
size_t new_anchor_ack__pack
                     (const NewAnchorAck   *message,
                      uint8_t             *out);
size_t new_anchor_ack__pack_to_buffer
                     (const NewAnchorAck   *message,
                      ProtobufCBuffer     *buffer);
NewAnchorAck *
       new_anchor_ack__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   new_anchor_ack__free_unpacked
                     (NewAnchorAck *message,
                      ProtobufCAllocator *allocator);
/* NewAnchorAccept methods */
void   new_anchor_accept__init
                     (NewAnchorAccept         *message);
size_t new_anchor_accept__get_packed_size
                     (const NewAnchorAccept   *message);
size_t new_anchor_accept__pack
                     (const NewAnchorAccept   *message,
                      uint8_t             *out);
size_t new_anchor_accept__pack_to_buffer
                     (const NewAnchorAccept   *message,
                      ProtobufCBuffer     *buffer);
NewAnchorAccept *
       new_anchor_accept__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   new_anchor_accept__free_unpacked
                     (NewAnchorAccept *message,
                      ProtobufCAllocator *allocator);
/* NewAnchorComplete methods */
void   new_anchor_complete__init
                     (NewAnchorComplete         *message);
size_t new_anchor_complete__get_packed_size
                     (const NewAnchorComplete   *message);
size_t new_anchor_complete__pack
                     (const NewAnchorComplete   *message,
                      uint8_t             *out);
size_t new_anchor_complete__pack_to_buffer
                     (const NewAnchorComplete   *message,
                      ProtobufCBuffer     *buffer);
NewAnchorComplete *
       new_anchor_complete__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   new_anchor_complete__free_unpacked
                     (NewAnchorComplete *message,
                      ProtobufCAllocator *allocator);
/* CloseChannel methods */
void   close_channel__init
                     (CloseChannel         *message);
size_t close_channel__get_packed_size
                     (const CloseChannel   *message);
size_t close_channel__pack
                     (const CloseChannel   *message,
                      uint8_t             *out);
size_t close_channel__pack_to_buffer
                     (const CloseChannel   *message,
                      ProtobufCBuffer     *buffer);
CloseChannel *
       close_channel__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   close_channel__free_unpacked
                     (CloseChannel *message,
                      ProtobufCAllocator *allocator);
/* CloseChannelComplete methods */
void   close_channel_complete__init
                     (CloseChannelComplete         *message);
size_t close_channel_complete__get_packed_size
                     (const CloseChannelComplete   *message);
size_t close_channel_complete__pack
                     (const CloseChannelComplete   *message,
                      uint8_t             *out);
size_t close_channel_complete__pack_to_buffer
                     (const CloseChannelComplete   *message,
                      ProtobufCBuffer     *buffer);
CloseChannelComplete *
       close_channel_complete__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   close_channel_complete__free_unpacked
                     (CloseChannelComplete *message,
                      ProtobufCAllocator *allocator);
/* Error methods */
void   error__init
                     (Error         *message);
size_t error__get_packed_size
                     (const Error   *message);
size_t error__pack
                     (const Error   *message,
                      uint8_t             *out);
size_t error__pack_to_buffer
                     (const Error   *message,
                      ProtobufCBuffer     *buffer);
Error *
       error__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   error__free_unpacked
                     (Error *message,
                      ProtobufCAllocator *allocator);
/* Pkt methods */
void   pkt__init
                     (Pkt         *message);
size_t pkt__get_packed_size
                     (const Pkt   *message);
size_t pkt__pack
                     (const Pkt   *message,
                      uint8_t             *out);
size_t pkt__pack_to_buffer
                     (const Pkt   *message,
                      ProtobufCBuffer     *buffer);
Pkt *
       pkt__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   pkt__free_unpacked
                     (Pkt *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Sha256Hash_Closure)
                 (const Sha256Hash *message,
                  void *closure_data);
typedef void (*BitcoinOutputId_Closure)
                 (const BitcoinOutputId *message,
                  void *closure_data);
typedef void (*BitcoinOutput_Closure)
                 (const BitcoinOutput *message,
                  void *closure_data);
typedef void (*BitcoinSignature_Closure)
                 (const BitcoinSignature *message,
                  void *closure_data);
typedef void (*Anchor_Closure)
                 (const Anchor *message,
                  void *closure_data);
typedef void (*OpenChannel_Closure)
                 (const OpenChannel *message,
                  void *closure_data);
typedef void (*OpenCommitSig_Closure)
                 (const OpenCommitSig *message,
                  void *closure_data);
typedef void (*OpenAnchorSig_Closure)
                 (const OpenAnchorSig *message,
                  void *closure_data);
typedef void (*OpenComplete_Closure)
                 (const OpenComplete *message,
                  void *closure_data);
typedef void (*Update_Closure)
                 (const Update *message,
                  void *closure_data);
typedef void (*UpdateAccept_Closure)
                 (const UpdateAccept *message,
                  void *closure_data);
typedef void (*UpdateComplete_Closure)
                 (const UpdateComplete *message,
                  void *closure_data);
typedef void (*NewAnchor_Closure)
                 (const NewAnchor *message,
                  void *closure_data);
typedef void (*NewAnchorAck_Closure)
                 (const NewAnchorAck *message,
                  void *closure_data);
typedef void (*NewAnchorAccept_Closure)
                 (const NewAnchorAccept *message,
                  void *closure_data);
typedef void (*NewAnchorComplete_Closure)
                 (const NewAnchorComplete *message,
                  void *closure_data);
typedef void (*CloseChannel_Closure)
                 (const CloseChannel *message,
                  void *closure_data);
typedef void (*CloseChannelComplete_Closure)
                 (const CloseChannelComplete *message,
                  void *closure_data);
typedef void (*Error_Closure)
                 (const Error *message,
                  void *closure_data);
typedef void (*Pkt_Closure)
                 (const Pkt *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor sha256_hash__descriptor;
extern const ProtobufCMessageDescriptor bitcoin_output_id__descriptor;
extern const ProtobufCMessageDescriptor bitcoin_output__descriptor;
extern const ProtobufCMessageDescriptor bitcoin_signature__descriptor;
extern const ProtobufCMessageDescriptor anchor__descriptor;
extern const ProtobufCMessageDescriptor open_channel__descriptor;
extern const ProtobufCMessageDescriptor open_commit_sig__descriptor;
extern const ProtobufCMessageDescriptor open_anchor_sig__descriptor;
extern const ProtobufCMessageDescriptor open_complete__descriptor;
extern const ProtobufCMessageDescriptor update__descriptor;
extern const ProtobufCMessageDescriptor update_accept__descriptor;
extern const ProtobufCMessageDescriptor update_complete__descriptor;
extern const ProtobufCMessageDescriptor new_anchor__descriptor;
extern const ProtobufCMessageDescriptor new_anchor_ack__descriptor;
extern const ProtobufCMessageDescriptor new_anchor_accept__descriptor;
extern const ProtobufCMessageDescriptor new_anchor_complete__descriptor;
extern const ProtobufCMessageDescriptor close_channel__descriptor;
extern const ProtobufCMessageDescriptor close_channel_complete__descriptor;
extern const ProtobufCMessageDescriptor error__descriptor;
extern const ProtobufCMessageDescriptor pkt__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_lightning_2eproto__INCLUDED */
