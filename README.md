# red-black-tree

A self-balancing **red-black tree** implementation in C.  

Originally written as a university assignment, refactored into a reusable and well-structured library.

## Features

- Insert, search and delete operations with automatic rebalancing
- In-order print with node colors (`R`/`B`)
- Iterative node cleanup to avoid recursion depth limits
- Uses a nil sentinel node internally for clear and consistent tree logic
- Clean API suitable for integration into other C projects

## Build and Run

```bash
make        # build demo
./demo      # run example
```

## Notes

- Duplicate keys are allowed.
