#!/bin/sh
## Short script to build and pack the library into a debian.
## It will also invoke an installation for the library
BUILD_PATH="src/build"
if [ ! -d "${BUILD_PATH}" ]; then
	mkdir -p "${BUILD_PATH}"
fi
cd "${BUILD_PATH}"
cmake ..
OUTPUT=$(sudo cpack .. | tail -1)
echo "The package was created in the build directory and is named : $OUTPUT"
echo "You may use sudo dpkg -i <package name> to install the package"