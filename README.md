# fptest
A simple example of trying to get consistent floating point results across different architectures and compilers.

I usually have this setup to test different compilers
git clone https://github.com/bornskilled200/fptest.git
cd fptest

mkdir msvc10_0
cd msvc10_0
cmake -g "Visual Studio 10" ..
cmake --build .
executable_file.exe > output1.txt

cd ..
mkdir mingw_0
cd mingw_0
cmake -g "MinGW Makefiles" ..
cmake --build .
executable_file.exe > output2.txt

diff output1.txt output2.txt

##Sources
These are the sources that helped me to make this example

###Floating Point Precisison
https://randomascii.wordpress.com/2012/03/21/intermediate-floating-point-precision/

###Alternative Math functions
Sin/Cos: http://lab.polygonal.de/?p=205
Square Root: http://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
