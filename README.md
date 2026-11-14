# OpenJTX-A9

> [!IMPORTANT]
> OpenJTX-A9 is an independent open-source implementation. This project is not affiliated with, endorsed by, or sponsored by JTXTools. All trademarks belong to their respective owners.

Alternative open-source software in Qt(multi-platform) for multimeter with bluetooth or serial connection

> [!WARNING]
> This software is currently in active development, expect bugs and not implemented features.

## Prerequisites

- CMake 4.3 or newer
- Qt 6.4 or newer
- Ninja
- clangd (optional, for editor integration)

## Connecting the JTXTools A9 on Linux

On Linux, the serial port may require additional user permissions. See the
[JTXTools A9 connection guide](docs/JTXTools%20A9.md#linux-serial-port-permissions)
for how to identify the USB device and configure persistent access.

## Building

Configure and build the development preset:

```sh
cmake --preset dev
cmake --build --preset dev
```

If Qt is installed outside the system search paths, provide its installation prefix
when configuring instead of creating system-wide symbolic links:

```sh
cmake --preset dev -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/compiler
```

The development compilation database is generated at
`build/dev/compile_commands.json`.

## Testing

```sh
ctest --preset dev
```

The test infrastructure is ready for Qt Test targets. Until the first test is
added, CTest completes successfully without running any tests.

The CI workflow preset configures, builds, and tests in one command:

```sh
cmake --workflow --preset ci
```

## Install

```sh
cmake --preset release
cmake --build --preset release
cmake --install build/release --prefix "$HOME/.local"
```

## Packaging

After building the release preset, generate a portable package with CPack:

```sh
cpack --preset release
```

Packaging produces a ZIP archive on Windows and macOS, and a TGZ archive on
Linux. Linux packages use the Qt libraries installed on the target system.
