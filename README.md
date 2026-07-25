# OpenJTX-A9

> [!IMPORTANT]
> OpenJTX-A9 is an independent open-source implementation. This project is not affiliated with, endorsed by, or sponsored by JTXTools. All trademarks belong to their respective owners.

Alternative open-source software in Qt(multi-platform) for multimeter with bluetooth or serial connection

> [!WARNING]
> This software is currently in active development, expect bugs and not implemented features.

## Prerequisites

- CMake version 4.3^
- Qt6 C++
- clangd

## Building

In order to cmake find the qtpaths file you must create a symbolic link or else to the bin path

```sh
sudo ln -s /usr/lib/qt6/bin/qtpaths /usr/bin/qtpaths
```

As for other OS should be simple as one command

```sh
cmake -B build --config Release
```

## Install

```sh
cmake -S . -B build
cmake --build build -j$(nproc)
cmake --install build --prefix "$HOME/.local"
```
