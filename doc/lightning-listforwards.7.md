lightning-listforwards -- Command showing all htlcs and their information
=========================================================================

SYNOPSIS
--------

**listforwards** [*status*] [*in\_channel*] [*out\_channel*]

DESCRIPTION
-----------

The **listforwards** RPC command displays all htlcs that have been
attempted to be forwarded by the Core Lightning node.

If *status* is specified, then only the forwards with the given status are returned.
*status* can be either *offered* or *settled* or *failed* or *local\_failed*

If *in\_channel* or *out\_channel* is specified, then only the matching forwards
on the given in/out channel are returned.

RETURN VALUE
------------

[comment]: # (GENERATE-FROM-SCHEMA-START)
On success, an object containing **forwards** is returned.  It is an array of objects, where each object contains:

- **in\_channel** (short\_channel\_id): the channel that received the HTLC
- **in\_htlc\_id** (u64): the unique HTLC id the sender gave this
- **in\_msat** (msat): the value of the incoming HTLC
- **status** (string): still ongoing, completed, failed locally, or failed after forwarding (one of "offered", "settled", "local\_failed", "failed")
- **received\_time** (number): the UNIX timestamp when this was received
- **out\_channel** (short\_channel\_id, optional): the channel that the HTLC (trying to) forward to
- **out\_htlc\_id** (u64, optional): the unique HTLC id we gave this when sending
- **style** (string, optional): Either a legacy onion format or a modern tlv format (one of "legacy", "tlv")

If **out\_msat** is present:

  - **fee\_msat** (msat): the amount this paid in fees
  - **out\_msat** (msat): the amount we sent out the *out\_channel*

If **status** is "settled" or "failed":

  - **resolved\_time** (number): the UNIX timestamp when this was resolved

If **status** is "local\_failed" or "failed":

  - **failcode** (u32, optional): the numeric onion code returned
  - **failreason** (string, optional): the name of the onion code returned

[comment]: # (GENERATE-FROM-SCHEMA-END)

AUTHOR
------

Rene Pickhardt <<r.pickhardt@gmail.com>> is mainly responsible.

SEE ALSO
--------

lightning-getinfo(7)

RESOURCES
---------

Main web site: <https://github.com/ElementsProject/lightning>

[comment]: # ( SHA256STAMP:6e61a0b0f90fcd471ca2e531961b4e756d619a652e199bc84a47c3b8c0e91388)
