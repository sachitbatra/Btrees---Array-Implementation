# Btrees---Array-Implementation

Problem Definition:
Implement a B tree with the following operations:
create
insert
delete
search.
Experiment with different values of t - the minimum degree of each node.

Implement the B-tree as an array.
Each node shall have # of records(key: rest of the record).
The number of keys (but for the root node) shall be between t - 1 and 2t - 1.
Each node at a given point in time may have k keys and k + 1 references to its child trees.
In this implementation, each reference will be an index of the array.
