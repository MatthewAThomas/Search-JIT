# Search-JIT

This project uses runtime code generation to speed up search algorithms. It is inspired by the article "A method for JIT’ing algorithms and data structures with LLVM": https://blog.christianperone.com/2009/11/a-method-for-jiting-algorithms-and-data-structures-with-llvm/. 

Currently, only basic BST search algorithms are being explored, though other types of algorithms and data structures may be explored in the future.


## bst
The binary search tree directory contains 1) a rudimentary implementation of a binary search tree (bst/bst-data), and 2) a "JITed" version of a binary search tree (bst/bst-ir). The aformentioned article explains what it means to JIT a BST, and why one might want to do so.   