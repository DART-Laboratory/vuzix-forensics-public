# Vuzix Forensics

This a repository for my summer 2026 research project with the Leadership Alliance's FYRE program. This repository contains a CMake project that provides an executable (`tp` located in the `build` directory) that reads and analyzes an Android `adb` bugreport (specifically tailored to a Vuzix device) and generates a provenance graph.

## Using `tp`

`tp` stands for "trace parse." Run `tp` with `-h` or `--help` to print the help page. The command-line parameters are as follows:

* `file`                        - Required argument for the bugreport file.
* `-o,--out TEXT`               - Output file, without file extension. This will be the file name for the generated dot and svg files.
* `-d,--dot,--no-dot{false}`    - Whether or not to save or delete the dot file after creating the svg.

## Dependencies

This project uses:

* CMake - Used for configuring and building C++ project.
* CLI11 - Used for customizing `tp` as a command-line utility.
* GraphViz (`dot` utility)\* - Used for generating graph visualization.

\**GraphViz is the only dependency required to actually RUN the program properly. It is used to generate the svg file. If your environment does not have the `dot` utility, `tp` will not be able to convert the .dot file into an svg. Everything else will work, however.*

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
