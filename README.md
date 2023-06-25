# FFS (Fast File Search)

## Building Solution C/C++

> Install brotli using vcpkg

```cmd
vcpkg install brotli:x64-windows brotli:x86-windows
```

> Informations

- C++20
- main.cpp is the test file
- file structure
  - cache
  - index
  - tree
  - lienar
  - dfs

## Building Python Extension

> if you don't have VSCode

```cmd
python setup.py build
```

> else

Press CTRL+MAJ+B to run the build command.

## How it works

### Linear Search

> Linear search consists of reading folders recursively until there are no more files.
> The parallel mode makes it possible to carry out research in several thread simulator. The child folders of the input folder (root) are distributed in threads according to the number of cores available on the processor.

### DFS (Depth First Search)

> Uses the Depth First Search algorithm to search through the file. the parallel version does the same thing as for the linear search.

### Tree

> Created a folder and file tree. the tree is then compressed with brotli and saved in a cache file. The cache file can then be read to be able to search quickly.

### Indexer

> Indexing it creates a hash table that keeps all files and folders. the hash table is then compressed and saved in a cache file. the cache file can then be read to perform quick searches.

### Average speed for the same search

| Methods           | Average Speed |
| ----------------- | :-----------: |
| DFS               |   2s 500ms    |
| Linear            |   2s 450ms    |
| DFS (Parallel)    |   2s 100ms    |
| Linear (Parallel) |    2s 50ms    |
| Tree (cache)      |    200 ms     |
| Index (cache)     |     15 ms     |
