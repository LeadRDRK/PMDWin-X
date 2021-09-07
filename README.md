# PMDWin-X
This is a fork of PMDWin, a PMD98 driver for Windows, which provides (better) cross-platform compatibility. It also has an experimental Nuked OPL-Mod backend.

PMD98 is a music driver originally designed for use with the PC-98 series of computers.
# Documentation
The original documentation in Japanese is available in [DLLInfop.txt](DLLInfop.txt)

Overall usage flow: music_load() -> music_start() -> getpcmdata() -> music_stop()

A sample program can be found in [/examples](examples) which is a simple cross-platform command line PMD player.

Please note that PMDWin itself does not produce audio output. You need a program which feeds the PCM data that PMDWin generates to your sound system.

The Nuked OPL-Mod backend is mostly complete. However, there's currently a bug in the PSG emulation which makes it sounds noisy and inaccurate. You shouldn't use it in production for now.
# Building
You can use CMake to generate build files for your preferred build system. Please make sure that CMake is installed on your system and is present in your PATH.

Create a new folder named "build" in the main directory, then open a terminal/command prompt in that directory.

Run one of these commands to generate the build files:
- Visual Studio: `cmake .. -G "Visual Studio 16 2019"`
- MinGW: `cmake .. -G "MinGW Makefiles"`
- Linux: `cmake .. -G "Ninja"` (or use Unix Makefiles if you prefer)
- macOS: `cmake .. -G "Xcode"`

These are only a few notable examples. Generators for other build systems are available. See [cmake-generators(7)](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) for a full list.

A few build options are available which can be specified by adding `-DOPTION_NAME=VALUE` to your CMake invocation:
- `PMDWIN_OPNA_EMU` - Specifies the OPNA emulator to be used. Available options are `fmgen` and `nuked`. Default: `fmgen`
- `PMDWIN_BUILD_EXAMPLE` - Build the sample program provided in the `/example` directory (pmdplayer). Default: `OFF`

The Windows DLL built with this project is fully compatible with FMPMD2000. If you built it with MinGW, please make sure to also include the MinGW runtime libraries in the folder.
# License
PMDWin-X is licensed under the PMDWin project license. Please see the [LICENSE file](LICENSE). The original unmodified Japanese version of the license can be found in [PMDWinS.txt](PMDWinS.txt)

The original version of PMDWin is made by C60, which is a modification of the PMD98 driver made by M.Kajihara.

PMDWin-X uses these external open source libraries:
- fmgen by cisc
> FM Sound Generator with OPN/OPM interface
> Copyright (C) by cisc 1998, 2003.
- Nuked OPL-Mod by Nuke.YKT & Jean Pierre Cimalando ([source](https://github.com/BambooTracker/BambooTracker/tree/master/BambooTracker/chip/nuked))
> Licensed under GNU LGPLv2.1
- emu2149 by Mitsutaka Okazaki & rerrahkr ([source](https://github.com/rerrahkr/emu2149/tree/opna-ssg))
> Licensed under The MIT License
- Some parts from the MAME project
> Licensed under The BSD-3-Clause License