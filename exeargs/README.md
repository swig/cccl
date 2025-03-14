# exeargs

Simple helper tool for displaying arguments passed to an executable.
Useful on MSYS operating systems where the operating system modifies arguments passed to native windows executables.

## Building
To build with cl.exe using cccl for creating a native Windows executable, simply run `make`, output should be:
```bash
$ make
../cccl --cccl-verbose /EHsc exeargs.cpp -o exeargs.exe
cl "-nologo" "-EHsc" "exeargs.cpp" "-Feexeargs.exe"
exeargs.cpp
```

To build with g++, run `make` as follows:
```bash
$ make CXX=g++ CXXFLAGS=
g++  exeargs.cpp -o exeargs.exe
```

## Example usage
MSYS expands absolute paths when passed to a native Windows executable. The example below shows the `/tmp/runme.cpp` argument being expanded to a native Windows path by MSYS:
```bash
$ ./exeargs "-nologo" "-EHsc" "/tmp/runme.cpp" "-Feexeargs.exe"
exeargs "-nologo" "-EHsc" "C:/msys64/tmp/runme.cpp" "-Feexeargs.exe"
```

The above is the output on MSYS when `exeargs` is built with cl.exe to generate a native Windows executable.
If `exeargs` is built with g++ instead or run on most other operating systems, the paths are not converted:

```bash
$ ./exeargs "-nologo" "-EHsc" "/tmp/runme.cpp" "-Feexeargs.exe"
exeargs "-nologo" "-EHsc" "/tmp/runme.cpp" "-Feexeargs.exe"
```

