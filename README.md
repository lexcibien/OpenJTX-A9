# OpenJTX-A9

> [!IMPORTANT]
> OpenJTX-A9 is an independent open-source implementation. This project is not affiliated with, endorsed by, or sponsored by JTXTools. All trademarks belong to their respective owners.

Alternative open-source software in Qt(multi-platform) for multimeter with bluetooth or serial connection

> [!WARNING]
> This software is currently in active development, expect bugs and not implemented features.

## Prerequisites

- CMake 4.3 or newer
- Qt 6.4 or newer
- MSVC 2022 (for Windows)
- Ninja
- clangd (optional, for editor integration)

## Connecting the JTXTools A9 on Linux

On Linux, the serial port may require additional user permissions. See the
[JTXTools A9 connection guide](docs/JTXTools%20A9.md#linux-serial-port-permissions)
for how to identify the USB device and configure persistent access.

## Building

On Windows, first you need to enter on the terminal MSVC 2022. Also you can do this on a powershell 7 terminal:

```sh
$vs = 'path\to\your\Microsoft Visual Studio\version\Community'
$env:OPENJTX_VSDEV = "$vs\Common7\Tools\VsDevCmd.bat"
$env:Path = "$vs\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;$env:Path"
$env:CMAKE_PREFIX_PATH = 'path\to\your\Qt\version\msvc2022_64'
cmd.exe /d /c 'call "%OPENJTX_VSDEV%" -arch=amd64 -host_arch=amd64 && pwsh -NoLogo'
```

Make sure you change to your path

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

On Windows, build the release preset and run the executable from the build
directory. The build copies the Qt DLLs and plugins next to the application.

```sh
cmake --preset release
cmake --build --preset release
.\build\release\OpenJTX_A9.exe
```

To install to a separate location:

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
