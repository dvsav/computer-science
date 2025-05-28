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
- Quick sort ([quick_sort.h](quick_sort.h))
- [Trial division](https://en.wikipedia.org/wiki/Trial_division) ([prime.h](src/prime.h))

## Prerequisites

On Linux install GNU Make and CMake build systems (example below is given for APT package manager which is used by default in Debian based Linux, e.g. Ubuntu and Linux Mint):

```bash
sudo apt-get -y install make
sudo apt-get -y install cmake
```

On Windows install CMake build system using [Chocolatey](https://chocolatey.org/) package manager:
```
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
```

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

...or CMake:

```bash
./cmake.sh
```

## Usage

You can include the source code of this project into your own project.

## Contributing

Contributions are welcome! Please follow these steps:

[Fork the repository](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/fork-a-repo).
Create a new branch (`git checkout -b feature/YourFeature`).
Commit your changes (`git commit -m 'Add some feature'`).
Push to the branch (`git push origin feature/YourFeature`).
[Open a pull request](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request).

## License

This project is licensed under the [MIT License](LICENSE).
