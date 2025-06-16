# computer-science

This is an educational project containing implementations of some core computer science algorithms.

## Table of Contents

- [About](#about)
- [Algorithms](#algorithms)
- [Prerequisites](#prerequisites)
- [How to build](#how-to-build)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## About

The project is written in C++. Its goal is to showcase several things:
- Clean well-documented C++ code
- Usage of different build systems (MSBuild, CMake, GNU Make)
- Unit testing via Catch2 framework
- Automated testing via GitHub Actions

## Algorithms

The list of implemented algorithms.
- Simple sorting algorithms ([simple_sorts.h](src/simple_sorts.h))
  - selection sort
  - insertion sort
  - bubble sort
- Merge sort ([merge_sort.h](src/merge_sort.h))
- Quick sort with Lomuto and randomized partitioning ([quick_sort.h](src/quick_sort.h))
- Binary search in a sorted array ([binary_search.h](src/binary_search.h))
- [Trial division](https://en.wikipedia.org/wiki/Trial_division) ([prime.h](src/prime.h))
- Binary search tree, AVL tree ([binary_tree.h](src/binary_tree.h))
- B-tree ([btree.h](src/btree.h))
- Heap (aka pyramid), priority queue, heap sort ([heap.h](src/heap.h))
- Graph algorithms
  - Graph data structure ([graph.h](src/graph.h))
  - Breadth-first and Depth-first search ([graph_search.h](src/graph_search.h))
  - Topological sort and Kosaraju's strongly connected components algorithm ([graph_topology.h](src/graph_topology.h))
  - Dijkstra shortest path ([graph_dijkstra.h](src/graph_dijkstra.h))
  - Kruskal's Minimal Spanning Tree ([graph_kruskal.h](src/graph_kruskal.h))
  - Prim's Minimal Spanning Tree ([graph_prim.h](src/graph_prim.h))
  - Bellman-Ford shortest path algorithm (to be implemented)
  - Floyd-Warshall shortest path algorithm (to be implemented)
  - Johnson's shortest path algorithm (to be implemented)
- Hash map with separate chaining collision resolution ([hash_map.h](src/hash_map.h))
- Bloom filter ([bloom_filter.h](src/bloom_filter.h))
- Greedy algorithms
  - Huffman encoding ([huffman_encoding.h](src/huffman_encoding.h))
- Dynamic programming
  - Knapsack problem ([knapsack.h](src/knapsack.h))
- Divide and conquer algorithms
  - Karatsuba multiplication algorithm ([karatsuba.h](src/karatsuba.h))
  - Strassen's matrix multiplication algorithm (to be implemented)
- NP-hard problems
  - Travelling salesman problem (to be implemented)

## Prerequisites

- This project is written in C++, so it needs a C++ compiler like GCC or Clang.
- The project showcases the usage of several build systems (you may use whichever you want): GNU Make, CMake and MSBuild (MS Visual Studio).
- Python is used to generate test vectors, so you might want to install it as well.

On Linux install development tools (GCC, GDB, etc.), GNU Make and CMake build systems (example below is given for APT package manager which is used by default in Debian based Linux, e.g. Ubuntu and Linux Mint):

```bash
sudo apt update
sudo apt install build-essential clang gdb make cmake python3
```

On Windows install CMake build system using [Chocolatey](https://chocolatey.org/) package manager:
```
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
```
...or [download CMake installer for Windows](https://cmake.org/download/)

## How to build

Clone the repository:

```bash
git clone https://github.com/dvsav/computer-science.git
cd computer-science
```

Compile the sources using one of convenience scripts that call either GNU Make:

```bash
./make.sh test
```
If the build is successful, you'll find the binary files in `build/GNUMake/bin` directory.

...or CMake:

```bash
./cmake.sh
```
If the build is successful, you'll find a binary executable `computer-science` in `build/CMake/gnumake` directory.  

For more information about setting up development environment see [Development.md](docs/Development.md).

## Usage

You can include the source code of this project into your own project, e.g. add this git repository as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules):
```
cd your_repository
git submodule add https://github.com/dvsav/computer-science.git
```

## Contributing

Contributions are welcome! Please follow these steps:

[Fork the repository](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/fork-a-repo).  
Create a new branch (`git checkout -b feature/YourFeature`).  
Commit your changes (`git commit -m 'Add some feature'`).  
Push to the branch (`git push origin feature/YourFeature`).  
[Open a pull request](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request).

## License

This project is licensed under the [MIT License](LICENSE).
