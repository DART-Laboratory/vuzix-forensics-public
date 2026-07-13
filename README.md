# Vuzix Forensics

This a repository for my summer 2026 research project with the Leadership Alliance's FYRE program. This repository contains a CMake project that provides an executable (`hs`) that reads and analyzes an Android `adb` bugreport (specifically tailored to a Vuzix device) and generates a provenance graph.

## Using `hs`

`hs` stands for "HindSight" Run `hs` with `-h` or `--help` to print the help page. The command-line parameters are as follows:

* `file`                        - Required argument for the bugreport file.
* `-o,--out TEXT`               - Output file, without file extension. This will be the file name for the generated dot and svg files.
* `-d,--dot,--no-dot{false}`    - Whether or not to save or delete the dot file after creating the svg.

## Dependencies

This project uses:

* CMake - Used for configuring and building C++ project.
* CLI11 - Used for customizing `hs` as a command-line utility.
* GraphViz (`dot` utility)\* - Used for generating graph visualization.

\**GraphViz is the only dependency required to actually RUN the program properly. It is used to generate the svg file. If your environment does not have the `dot` utility, HindSight will not be able to convert the .dot file into an svg. Everything else will work, however.*

### Installing dependencies

#### Debian-based

* `sudo apt install cmake`
* `sudo apt install libcli11-dev`
* `sudo apt install graphviz`

#### Red Hat-based

* `sudo dnf install cmake`
* `sudo dnf install cli11-devel`
* `sudo dnf install graphviz`

#### Windows

* CMake download: https://cmake.org/download/
* CLI11 vcpkg download: https://cliutils.github.io/CLI11/book/chapters/installation.html#installing-cli11-using-vcpkg
* GraphViz download: https://graphviz.org/download/

## Building

To build project, go to the project's root directory and run `cmake -B build -DCMAKE_BUILD_TYPE=Release .` The `CMAKE_BUILD_TYPE` flag is optional but I recommend keeping it to build in release mode.

After building with that command, the binary will be located in the `build` directory.

## Overview of research

The optimal tool for collecting system information for an android device without root seems to be `adb bugreport`. The bugreport command is a part of the `adb` (Android Debug) utility that is often used for Android development. For this project, we are using it for collecting system logs.

The bugreport command runs various other `adb` commands as well as several linux commands and compiles the outputs into a large bugreport file. The utilities present in the bugreport include but not limited to:

* `adb logcat`
* `dumpsys`
* `ps`
* `top`
* `ip`
* `ss`
* `iptables/ip6tables`
* `netstat`
* `dmesg`

For HindSight to identify process creation relationships between processes, it primarily uses the `logcat` section of the bugreport. The Android `ActivityManager` and `ActivityTaskManager` log when processes and tasks are started. Like any linux system, processes have PIDs, UIDs, and TIDs associated with them, as well as a parent process. On Android systems, processes often have their own UID that is not associated with any important user, similar to just another unchanging PID. This is to handle specialized permissions.

The difficulty in forming the relationships between processes is figuring out what IDs belong to what processes. Some logs describe relationships only by their UIDs, some by their names. So, when putting together the graph, you must figure out what UID belongs to what PID and name. Plus, processes can share UIDs and names, so PIDs are the only definitive form of identification for a process, but Android logs don't often refer to processes by their PID, so it is essentially impossible to draw certain conclusions about what process is actually acting on another, and thus attempting to using PIDs to draw relationships is a fruitless endeavor. The Android logs essentially build a huge complicated puzzle with several missing pieces, so in order to form a complete puzzle, you have to assume what is on the missing pieces and draw them yourself.

