# Cauterize Club

A distributed shared timeline of messages defined by a Cauterize schema.

# Network Topology

Each node is free to create connections as it sees fit. There is no artificial
limit on how many connections a node can make.

# Network Messaging

Messages are broken into two categories: control messages and transport messages.

## Control Messages

Control messages are used to maintain the health and quality of the network.
They do not exchange any sort of data to be recorded in a timeline.

  * `PeerList(list_of_peers)` - sends a list of peers to which this node is already connected
    * Should be sent on first contact with another node.
    * Abbreviated list should be sent when a new node connects

## Transport Messages

Transport messages are used to discover and distribute timeline entries around
the network.

  * `Announce(hash)` - used to inform a peer that a new hash is available.
    * Send to all peers when the node learns of a new entry.
  * `Send(hash)` - used to send a timeline entry to another node.
    * Send to all peers when the node creates a new timeline entry.
    * Send to any peer that has made a `Request` of a hash.
  * `Request(hash)` - used to ask that a specific hash be sent to this node.
    * Send to any peer at any time. Useful to follow a chain of messages that
      this node doesn't yet contain.
  * `Query(hash)` - A cheap method to query whether or not peers contain the specified hash.
    * When a node wants to acquire a message with a specific hash, it can
      quickly query all its peers to discover which of them has the hash
      already. It's likely that this node will also have the parents of the
      node. This allows the node to heuristically discover peers that are
      likely to contain the next messages the node needs to acquire.
