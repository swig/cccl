# cccl
Unix cc compiler to Microsoft's cl compiler wrapper at https://github.com/swig/cccl

## Introduction

cccl is a wrapper around Microsoft's cl.exe Visual C++ compiler.  It converts
traditional Unix cc command line parameters to their cl.exe equivalents.

The main use for cccl is for using Unix build processes with the Microsoft
C/C++ compiler.  Using cccl in conjunction with ports of Unix utilities, it is
possible to build many Unix packages using MSVC, without modifying the build
process.  This is especially useful if you want to use GNU autotools
(autoconf/automake/libtool) with MSVC.

### Motivation

Why would one use a wrapper script to make cl.exe act like gcc, when gcc is
available for Windows?

There are a few reasons but the primary reason is usually to support 
cross-platform projects that need to use autoconf/automake/libtool for the 
build process.  The obvious thing might be to use gcc on all platforms.  But on 
Windows, not all third-party libraries work with gcc, some only work with MSVC.
cccl allows one to use the same build process with the MSVC compiler, but with 
only minimal changes to the build process.

Even though it's a somewhat simple approach, the original author, Geoffery
Wossum, wrote the initial version of cccl and released it for others to use as
it saved him a lot of hassle in the build process.

### Usage Overview

cccl assumes cl.exe is in your path. cccl is run the same way that a Unix C or
C++ compiler is used. Below shows usage to compile a simple C program:

```
$ cccl main.c -o runme.exe
main.c
```

By default cccl quietly passes the converted command line options to cl.exe.
These can be displayed by using the `--cccl-verbose` option and can be useful 
for analysing compilation problems, for example:

```
$ cccl --cccl-verbose main.c -o runme.exe
cl "/nologo" "main.c" "/Ferunme.exe"
main.c
```

### Getting Started

First, you'll want to [install cccl](#installing-cccl).  Then, you'll probably
want to learn using [autotools and MSVC](#autotools-and-msvc) together.


## Installing cccl

### Quick Overview

cccl is known to work on Cygwin, MinGW, MinGW-w64 and MSYS2.

You can either copy the cccl script to somewhere on your system, or you can do
a `./bootstrap && ./configure && make install` from the source if you have a 
Unixish enough environment installed.  The configure and Makefile don't really 
do anything but copy the cccl script, though.

### Prerequisites

cccl uses and hence requires ports of the following Unix utilities:
  - bash
  - grep
  - sed
  - tr
  - uname

cccl is a bash shell script which makes use of these utilities.  Therefore, you
will need at the very least a Windows versions of these utilities.  The easiest
and best source of these (as well as many other Unix tools) has traditionally
been RedHat's [Cygwin](https://cygwin.com/). Other well known Unix-like systems
can also be used - 
[MinGW](http://mingw.org/), [MinGW-w64](https://www.mingw-w64.org/),
[MSYS2](https://www.msys2.org/).

Install one of these and if you are planning on using cccl to build autotools projects, make sure that
you install autoconf, automake and libtool as well.

### Installing cccl

Once you have a working Unix-like environment, you have two options.  You
may manually copy the *cccl* file to somewhere in your path, since it's just a 
script. Alternatively, you may do the normal Unix 
`./bootstrap && ./configure && make && make install` routine.

### Setting Up Your Path

Obviously you'll want *cccl* to be in your path.  Since cccl directly invokes 
*cl.exe*, you'll need to make sure *cl.exe* is in your path as well.  This may have 
been done for you during your Visual Studio install.  If not, there should be a 
file called *vcvars32.bat* or *vcvarsall.bat* which can be run from an MS-DOS 
command prompt to set your path.  Visual Studio usually installs a menu item to 
run a *Visual Studio Command Prompt* and is commonly under the *Visual Studio 
Tools* menu. This invokes one of the aforementioned batch files.  Refer to the 
documentation included with Visual Studio for more details about running the 
Visual Studio command line tools.  Note that the GNU linker is also called 
*link.exe* and is usually present in the Unix-like systems running on Windows,
but this shouldn't be a 
problem as cccl does not invoke the linker directly, it relies on *cl.exe* to 
invoke the appropriate Microsoft linker.

## Autotools and MSVC

Autotools (autoconf, automake, libtool) and MSVC were never originally made to 
work together, but with cccl you can make them become reluctant friends.

### configure.ac

Autoconf requires a file called *configure.ac*, which on legacy projects may
still be called *configure.in*.  In order to use autoconf and MSVC, make sure 
the following line is in your *configure.ac* file:
```
AC_CANONICAL_HOST
```

If your *configure.ac* file contains a reference to `LT_INIT`, add the `[win32-dll]` option:
```
LT_INIT [win32-dll]
```

Or if your *configure.ac* file is using the legacy, deprecated equivalent syntax and contains a reference to `AM_PROG_LIBTOOL`, add the
following line *before* `AM_PROG_LIBTOOL`:
```
AC_LIBTOOL_WIN32_DLL
```

### Makefile.am

Believe it or not, your *Makefile.am* files will probably not require any 
changes to work.

#### Convenience Libraries

Convenience libraries (noinst .a targets) work fine without any changes.

#### Executable Targets

No changes required.  However, you may want to create a resource file to add
the explorer icon to the .exe file.

#### Static Libraries

Autotools can build static libraries (.lib files) fine.  However, you will
probably want the output file to have a different name under Windows (super.lib
vs.  libsuper.a).  You can accomplish this using some automake conditionals.

#### Dynamic Link Libraries (*.dll)

Older versions of the autotools (2.13 and earlier) didn't really support
dynamic link libraries (.dll files) very well. Newer versions of Libtool have
much improved Windows support for supporting dynamic link libraries.

### Building Your Projects

Once you've generated your configure script and Makefiles by running aclocal,
autoheader, autoconf and automake, you're ready to compile.

Before you run the configure script (from within your Unix-like system, of course),
you'll need to set the compiler and linker environment variables to use cccl.

```bash
export CC=cccl
export CXX=cccl
export LD=cccl
```

You may not need to set all these depending on whether your code base is just C
or C++ and how the linker is invoked, but it is safest to set all three to 
begin with.

The options in Visual C++ are numerous and have changed over the years but have 
got a bit simpler more recently.  Unlike gcc, the default options are not all 
that good though and often need tweaking. The good news is that the compiler 
will often tell you if you need to add an option, for example if you have C++ 
code that uses exceptions (any code that uses the STL uses exceptions), you 
should see a warning:

```
foo.cxx(10) : warning C4530: C++ exception handler used, but unwind semantics 
are not enabled. Specify /EHsc
```

Modern versions require `/EHsc`, but older versions require `/GX`.  Depending 
on the version of cl.exe, you may need to add `/GR` to enable run-time type 
information (RTTI). Use:

```bash
export CXXFLAGS="/EHsc"
```

for new versions or for older versions:

```bash
export CXXFLAGS="/GX /GR"
```

The options starting with `/` such as `/EHsc` and `/GX` are unrecognized by
cccl and are therefore passed directly to cl.exe.

Now, cross your fingers, and `./configure` and `make`.

The [Automake silent 
rules](https://www.gnu.org/software/automake/manual/html_node/Automake-Silent-Rules.html)
can control the verbosity of the output via the `V` variable. When `make V=1` 
is used, cccl will display the options passed to cl.exe as if `cccl --cccl-verbose`
was used.

## Usage

cc on Unix serves as a front end to the compiler and the linker.  Microsoft's 
cl.exe can operate the same but most usage is as a compiler.

### Compiling

Any traditional cc options that cccl does not convert into options for cl.exe
are passed unchanged to cl.exe. So options starting with `-` may or may not
be converted.  Any `/` options are expected to be for cl.exe and no attempt
is made to convert them; they are passed directly to cl.exe unmodified. 
Further details about option conversions are in the [Options](#options) 
section.

If cccl sees a C++ file with an extension other than .cpp (i.e. .cc, .C, or
.cxx), then cccl will prepend a `/Tp` option to the cl.exe command line to
force cl.exe to process it as a C++ source file.

### Linking

cl.exe interprets all options after `/link` to be linker options. cccl may
convert some options and if necessary pass them as a linker option by adding 
them after `/link`.  There are a few approaches to passing additional options 
to the linker directly. The first is to specify them as you would with cl.exe 
where everything after `/link` or `-link` is a linker option:

```bash
cccl main.c /W3 /link /LTCG /INCREMENTAL:NO
```

The second approach is to use the cccl option `--cccl-link` which passes just
the single next option to the linker. If using this approach, the above example
can be rewritten as follows:

```bash
cccl main.c --cccl-link /LTCG /W3 --cccl-link /INCREMENTAL:NO
```

Note that the first approach requires the compiler option `/W3` to be placed
before `/link`, whereas with the second approach the compiler option `/W3` can
be placed either before, after or as shown above right in between two linker
options.

Both of the above examples will thus invoke cl.exe as follows:

```
cl main.c /W3 /link /LTCG /INCREMENTAL:NO
```

### Verbosity

Visual C++ is unusually verbose for a compiler and displays the names of the
files it is compiling and sometimes "Creating library" and similiar messages.
For example, default output for compiling two or more files is shown below:

```bash
$ cccl main.c stuff.c -o runme.exe
main.c
stuff.c
Generating Code...
```

cccl supports the `--cccl-muffle` option which parses the output from cl.exe
and suppresses this extra verbiage. Adding this option to the example results
in silent output like traditional Unix compilers:

```bash
$ cccl main.c stuff.c -o runme.exe --cccl-muffle
```

The `--cccl-verbose` option will display exactly how cl.exe is invoked:

```bash
$ cccl main.c stuff.c -o runme.exe --cccl-verbose
cl "/nologo" "main.c" "stuff.c" "/Ferunme.exe"
main.c
stuff.c
Generating Code...
```

### Slashes and Dashes

All options provided to cccl beginning `/` or `-` will be converted to an appropriate
option beginning with `/` and then passed on to cl.exe.
However in the MinGW/MSYS and MSYS2 based operating systems, `/` options are handled differently
and so cccl will instead convert them to an option beginning with `-`.

For example, using `--cccl-verbose` to show how cccl converts on most operating systems:

```bash
$ cccl --cccl-verbose /EHsc -g runme.cpp -o runme.exe
cl "/nologo" "/EHsc" "/Zi" "runme.cpp" "/Ferunme.exe"
```

But on MSYS or MSYS2 based operating systems:

```bash
$ cccl --cccl-verbose /EHsc -g runme.cpp -o runme.exe
cl "-nologo" "-EHsc" "-Zi" "runme.cpp" "-Ferunme.exe"
```

These systems try to convert anything that looks like a path to a Windows equivalent
but only when passing options to a native Windows executable, such as cl.exe.
Absolute paths are problematic as they begin with a leading `/`.
Consider the input file specified with an absolute path in the next example:

```bash
$ cccl --cccl-verbose /EHsc -g /tmp/runme.cpp -o runme.exe
cl "-nologo" "-EHsc" "-Zi" "-tmp/runme.cpp" "-Ferunme.exe"
```
Note how `/tmp/runme.cpp` is modified to `-tmp/runme.cpp`, which is default handling
for cccl for unknown 'options' beginning with `/`, even though a path was intended in
this instance.

One solution is to use `--cccl-slash` which turns off the conversion
of `/` to `-` for unknown 'options':

```bash
$ cccl --cccl-verbose --cccl-slash /EHsc -g /tmp/runme.cpp -o runme.exe
cl "-nologo" "/EHsc" "-Zi" "/tmp/runme.cpp" "-Ferunme.exe"
# What the OS passes to cl.exe is
cl "-nologo" "C:/msys64/EHsc" "-Zi" "C:/msys64/tmp/runme.cpp" "-Ferunme.exe"
```

The operating system will then expand `/tmp/runme.cpp` into a Windows path, such as 
`C:/msys64/tmp/runme.cpp`. However, the operating system also thinks `/EHsc`
is a path and mistakenly converts it into something like `C:/msys64/EHsc`.
See https://www.msys2.org/docs/filesystem-paths/#automatic-unix-windows-path-conversion
for more details on this system's automatic path conversions.
The recommendation when using `--cccl-slash` is firstly to use `//` where a `/` option was
intended as these are converted back to `/` by the operating system and secondly
use a single leading `/` only for values that really are absolute paths.
So correct usage would then be:

```bash
$ cccl --cccl-verbose --cccl-slash //EHsc -g /tmp/runme.cpp -o runme.exe
cl "-nologo" "//EHsc" "-Zi" "/tmp/runme.cpp" "-Ferunme.exe"
# What the OS passes to cl.exe is
cl "-nologo" "/EHsc" "-Zi" "C:/msys64/tmp/runme.cpp" "-Ferunme.exe"
```

MSYS2 provides a way to exclude `/` conversion via an environment variable.
Please read their docs; below is another approach that could be used:

```bash
$ MSYS2_ARG_CONV_EXCL="/EHsc" cccl --cccl-verbose --cccl-slash /EHsc -g /tmp/runme.cpp -o runme.exe
cl "-nologo" "/EHsc" "-Zi" "/tmp/runme.cpp" "-Ferunme.exe"
# What the OS passes to cl.exe is
cl "-nologo" "/EHsc" "-Zi" "C:/msys64/tmp/runme.cpp" "-Ferunme.exe"
```

This operating system's conversion of absolute paths is very hit and miss,
consider a small tweak specifying the output file with an absolute path: 

```bash
$ cccl --cccl-verbose --cccl-slash //EHsc -g /tmp/runme.cpp -o /tmp/runme.exe
cl "-nologo" "//EHsc" "-Zi" "/tmp/runme.cpp" "-Fe/tmp/runme.exe"
# What the OS passes to cl.exe is
"-nologo" "/EHsc" "-Zi" "C:/msys64/tmp/runme.cpp" "-Fe/tmp/runme.exe"
```

The operating system does not convert the path to runme.exe.

The general recommendation for appropriate path conversion is look at all the
operating system's tools.
In this case `cygpath` is a great tool to do the correct conversion as follows:

```bash
$ cccl --cccl-verbose /EHsc -g $(cygpath -m /tmp/runme.cpp) -o $(cygpath -m /tmp/runme.exe)
cl "-nologo" "-EHsc" "-Zi" "C:/msys64/tmp/runme.cpp" "-FeC:/msys64/tmp/runme.exe"
```
Another wise choice is to use relative paths with forward slashes instead of absolute paths
as these are nearly always compatible on both Unix and Windows.

### Environment

cccl supports the `CCCL_OPTIONS` environment variable. The contents of the 
environment variable are treated as additional command line options to cccl. 
This can be handy for adding additional options or tweaking the verbosity 
options without changing the build system.  For example:

```bash
$ export CCCL_OPTIONS="--cccl-muffle --cccl-verbose /W3"
$ cccl -O2 main.c
cl "/nologo" "/W3" "/O2" "main.c"
```

and assuming `CCCL_OPTIONS` is not set, then the above is the same as:

```bash
$ cccl --cccl-muffle --cccl-verbose /W3 -O2 main.c
cl "/nologo" "/W3" "/O2" "main.c"
```

There is one notable difference using `CCCL_OPTIONS` compare to using actual options and 
that is the handling of spaces within options, for example `-I"My Headers"`. For this to 
work, you must pass it as a real cccl command line option and `CCCL_OPTIONS` can't be used.

### Options

The following Unix compiler cc/gcc options are understood by cccl:

 - **-ansi** Converts to cl.exe's **/Za**
 - **-c** Converts to cl.exe's **/C**
 - **-dumpmachine** Target machine information based on info in copyright banner
 - **-dumpversion** Version of cl.exe based on info in copyright banner
 - **-g**[0-9] Converts to cl.exe's **/Zi**
 - **-O0** Converts to cl.exe's **/Ot** optimization option
 - **-L<i>path</i>** Converts to linker option **/LIBPATH:_path_**
 - **-l<i>library</i>** Converts to **lib_library_.lib** (except **-link**)
 - **-m386** Converts to cl.exe's **/G3**
 - **-m486** Converts to cl.exe's **/G4**
 - **-mpentium** Converts to cl.exe's **/G5**
 - **-mpentiumpro** Converts to cl.exe's **/G6**
 - **-o** is converted to **/Fo** for object files and **/Fe** for executables
   and dlls
 - **-pedantic** Removed/ignored, cl.exe does not support any equivalent
 - **-std=_standard_** Converts to cl.exe' **/std:_standard_**
 - **-Wl,<option1>(,<option2>)** Options are passed to the linker
 - **-W** Removed/ignored
 - **-fno-strict-aliasing** Removed/ignored
 - **-isystem** Converted to **/I**
 - **-include** Converted to **/FI**
 - **-MT** Due to conflict with cl.exe's **/MT** option, there is no support
   and cccl exits
 - **-mno-cygwin** Removed/ignored
 - **-rpath** Removed/ignored
 - **-shared** Converts to cl.exe's **/LD** or **/LDd** if -g is used
 - **\*.(cc|cxx|C)** C++ source file is passed using **/Tp**

The following are cccl specific options:

 - **--help** Displays cccl help
 - **--cccl-link _OPTION_** Passes the option, _OPTION_, as a linker option
 - **--cccl-muffle** Removes cl.exe's verbiage (file names being compiled etc) 
   from being displayed
 - **--cccl-slash** Do not convert unknown options starting with **/** to **-**
 - **--cccl-verbose** Displays how cl.exe is invoked
 - **--cccl-version** Displays cccl's version string

The following are Visual C++ options

 - **-link** All remaining options are passed on as linker options
 - **/link** All remaining options are passed on as linker options

All other `-` options are passed untouched to cl.exe.  All `/` options are
passed unmodified to cl.exe.  All other non-options (file/path names) are
also passed on unmodified.

## History

### Version 0.01

Geoffrey Wossum (gwossum@acm.org) first wrote cccl as a Bourne shell script and
released it under the GPL license at [http://cccl.sf.net](http://cccl.sf.net)
on 9 August 2001.  The original cccl SourceForge project page is located at
[http://sf.net/projects/cccl](http://sf.net/projects/cccl).

### Version 0.03

This version contained a few patches and was released in January 2003 and was
the last version released on Sourceforge.

### 2003 to 2015

The original was forked a few times. The known public forks are:

* SWIFT http://swift.im/git/swift in the autoconf/cccl directory which was
  subsequently deleted.
* Open vSwitch
  https://github.com/openvswitch/ovs/commits/master/build-aux/cccl which
  built on top of the SWIFT fork.
* BalaBit which is described in a blog titled [Compiling autoconf/make projects under msvc](https://folti.blogs.balabit.com/2009/08/compiling-autoconfmake-projects-under-msvc-part-one/).
  The BalaBit fork is a lot more complex than the others and performance is consequently somewhat slower.
  The Git repo is at git://git.balabit.hu/folti/cccl.git.

### Version 1.0

William Fulton had been using cccl with some unpublished modifications for
testing and building [SWIG](http://www.swig.org) releases for many years. 
In 2015, these were made public and merged with the Open vSwitch fork and 
released on Github at https://github.com/swig/cccl under the newer GPL 
version 3 license.

The documentation was also converted to Markdown format and brought up to date.

#### New Features

The main improvements in version 1.0 over the original cccl release 0.03 are:

* Support for compiling and linking in a single step works with a mix of C, C++
  and object files as inputs.
* Support for spaces in paths, such as include paths and spaces in file names.
* Support for creating dynamic link libraries (dlls).
* Support for running under MinGW.
* Adding control of output verbosity.
* Simpler setup configuration by avoiding using link.exe directly, which
  previously may have incorrectly invoked the GNU linker.

### Version 1.1 (22 Aug 2019)

* Convert -dll to /LD to achieve compatibility with libtool.
* Ignore -rpath option.
* Previously -Wl options were ignored, now they are passed to the Microsoft
  linker.

### Version 1.2 (24 Apr 2021)

* Support -link option the same as /link
* Add conversion for -std option

### Version 1.3 (18 Nov 2022)

* Add support for space after -I and -L options
* Documentation updates

### Future

The move to Github at https://github.com/swig/cccl is hoped to inject some life 
into the project on a modern open source platform as the Sourceforge project 
had been defunct for 12 years.

The goal is for cccl to remain simple and lightweight so that the performance 
degradation added by the wrapper is kept small. Pull requests from users for 
bug fixes and improvements that meet these goals are encouraged.

## See Also

[wgcc](https://sourceforge.net/projects/interix-wgcc/) is another cccl like
tool for Interix but was superceded by
[parity](https://github.com/mduft/parity).
[msvcc.sh](https://github.com/libffi/libffi/blob/master/msvcc.sh)
