#!/bin/bash

set -o nounset
set -o errexit

mkdir msvc10_0
cd msvc10_0
cmake -g "Visual Studio 10" ..
cmake --build .