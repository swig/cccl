# cccl

## Authors
Geoffrey Wossum (gwossum@acm.org)
[http://cccl.sf.net](http://cccl.sf.net)

## Introduction
cccl is a wrapper around Microsoft Visual C++'s cl.exe.  It
converts Unix compiler parameters into parameters understood by cl.exe.
cccl's main use is for using Unix build processes with Microsoft compilers.
Using cccl in conjunction with ports of Unix utilities, it is possible to 
build many Unix packages using MSVC, without modifying the build process.

### Full Documentation
Documentation, including how to use autotools with MSVC by using cccl, is
now included!  The documentation is in HTML and can be found in 
doc/html/index.html.

### Motivation
Why would I build a wrapper script to make cl.exe act like gcc, when
gcc is available for Windows?  I had a cross-platform project that I wanted
to use autoconf/automake/libtool for the build process.  The obvious thing
to do would have been to use gcc on all platforms.  But on Windows I was
using some third-party libraries that didn't work with gcc, only with 
MSVC.  By creating cccl, I was able to use the same build process with the
MSVC compiler, with only minimal changes to the build process.  This 
saved a great deal of time.

Even though it's extremely simple and stupid, it saved me a lot of hassle
in the build process.  I tried to look for an already existing program to
do this, but none existed (that I could find).  I decided that since it 
proved so useful to me, I should release it to the world.

### Installing
cccl requires ports of the following Unix utilities:
  - bash
  - sed
  - tr

cccl is known to work on Cygwin and MinGW.

You can either copy the cccl script to somewhere on your system, or you can
do a `./configure && make install` if you have a Unixish enough environment
installed.  The configure and Makefile don't really do anything but copy
the cccl script, though.

## Usage
cccl will take it's command line parameters and convert the command line
parameters to something understood by cl.exe for both compiling and linking.

cccl can display the command line it is invoking before invoking by passing the
--cccl-verbose option or setting the environment variable V=1.

cccl assumes cl.exe are in your path.

If cccl sees a C++ file with an extension other than .cpp (i.e. .cc, .C, or
.cxx), then cccl will prepend a `/Tp` option to the cl.exe command line to force
cl.exe to process it as a C++ source file.

Other command line parameters are passed unchanged.

end of README


## Introduction to cccl (cc to cl.exe)

cccl makes MSVC's cl.exe act like Unix cc's by converting cc command line
parameters into one's cl.exe understands.  This is especially useful if
you want to use GNU autotools (autoconf/automake/libtool) with MSVC.

### Obtaining cccl

The cccl web page is at [http://cccl.sf.net](http://cccl.sf.net).
The cccl SourceForge project page is at 
[http://sf.net/projects/cccl](http://sf.net/projects/cccl).

### Getting Started

First, you'll want to [install cccl](#installing-cccl).  Then, you'll 
probably want to learn [using autotools and MSVC](#autotools-and-msvc) together.


## Installing cccl

### Prerequisites

cccl is a bash shell script which utilizes sed.  Therefore, you will need
at the very least Windows versions of bash and sed.  The easiest and best
source of these (as well as many other Unix tools) is RedHat's Cygwin.
Go to the (http://sources.redhat.com/cygwin)[Cygwin website] to
download and install Cygwin.  If your planning on using cccl to build 
autotools projects, make sure that you install autoconf, automake, and 
libtool as well.  

Some older distributions of Cygwin did not include libtool, so you'll 
need to install it from source.  You can use Cygwin's gcc to compile 
the libtool (it's just compiling a support library you won't use in
your MSVC projects).  Make sure that libtool installs to the same prefix
as autoconf and automake (most likely /usr).  Use the `--prefix=` option
in libtool's configure script to make sure this happens.  Otherwise,
you'll run into problems later when you try to build your autotools
project.

### Installing cccl

Once you have a working Cygwin environment, you have to options.  You may
either manually copy the *cccl* file to somewhere in your path, since
it's just a script.  

Alternatively, you may do the normal Unix `./configure ; make ; make install` 
routine.

### Setting Up Your Path

Obviously you'll want *cccl* to be in your path.  Since cccl directly
invokes *cl.exe* and *link.exe*, you'll need to make sure that these
are in your path as well.  This may have been done for you Visual Studio
install.  If not, there may be a file called *VCVARS32.BAT* which can
be run from an MS-DOS command prompt to set your path.  However, you'll
probably want *cl.exe* and *link.exe* to be permanantly in your path.
If they're not there already, refer to the documentation included with
Visual Studio on how to do this, as the process differs on each
Windows variant.

## Autotools and MSVC

Autotools (autoconf, automake, libtool) and MSVC were never made to work
together, but with cccl you can make them become reluctant friends.  If you 
don't already have Cygwin and cccl installed, [click here](#install). 

### configure.in

In order to use autoconf and MSVC, make sure the following lines are in
your configure.in:
```
AC_CANONICAL_SYSTEM
AC_CYGWIN
AC_OBJEXT
AC_EXEEXT
```

If your configure.in contains a reference to `AM_PROG_LIBTOOL`, add the
following line *before* `AM_PROG_LIBTOOL`
```
AC_LIBTOOL_WIN32_DLL
```

### Makefile.am

Believe it or not, your Makefile.am's will probably not require any
changes to work.

#### Convenience Libraries

Convenience libraries (noinst *.a targets) work fine without any changes.

#### Executable Targets

No changes required.  However, you may want to create a resource file
to add the explorer icon to the .exe file.  

TODO: Add link to page explaining how to do this.

#### Static Libraries (*.lib)

autotools can build static libraries fine.  However, you will probably
want the output file to have a different name under Windows (super.lib vs.
libsuper.a).  You can accomplish this using some automake conditionals.

#### Dynamic Link Libraries (*.dll)

autotools really doesn't support this very well 
(at least not in 2.13/1.4/1.3.5).  You'll probably just have to write
the rules to do this by hand, and select them using an automake 
conditional.

### Building Your Projects

Once you've generated your configure script and Makefiles by running
aclocal, autoheader, autoconf, and automake, you're ready to compile.

Before you run the configure script (from with Cygwin, of course), you need
to set the compiler environment variables to use cccl.

```sh
export CC=cccl
export CXX=cccl
```

If you have any C++ that uses exceptions (any code that uses the STL 
uses exceptions), you'll need to set CXXFLAGS properly to:

```sh
export CXXFLAGS="/GR /GX"
```

The `/GR` and `/GX` options are unrecognized by cccl and therefore will
be passed directly to cl.exe.

Now, cross your fingers, and `./configure` and `make`.


## cccl Usage

cccl is a wrapper around Microsoft's cl.exe.  It converts traditional
Unix cc command line parameters to their cl.exe equivalents.

Also, cc on Unix serves as a front end to the compiler and the linker.  
cl.exe, however, is mainly on the compiler (it does support a limited
compile and link).  The Microsoft linker, link.exe, usually must be
invoked explicitly.  cccl attempts to choose whether it should invoke
cl.exe or link.exe, adjusting the command line parameters as appropriate.
It probably does not do a very good job of this, but it seems to work
in most common cases.  Trying to compile and link in a single step will
probably make cccl very confused.

### Options

The following options are understood by cccl:

 - **-ansi** Converts to cl.exe's **/Za**
 - **-c** Converts to cl.exe's **/C**
 - **-g**[0-9] Converts to cl.exe's **/Zi** (cl.exe only supports one 
      debug level)
 - **-L_path_** Converts to link.exe's **/LIBPATH:_path_**
 - **-l_library_** Converts to **lib_library_.lib**
 - **-m386** Converts to cl.exe's **/G3**
 - **-m486** Converts to cl.exe's **/G4**
 - **-mpentium** Converts to cl.exe's **/G5**
 - **-mpentiumpro** Converts to cl.exe's **/G6**
 - **-o** If cccl decides to invoke cl.exe, **/Fo** will be given to cl.exe
   if the output file is an object file, otherwise **/Fe** will be given
   to cl.exe.  If cccl decides to run link.exe, then the output file
   will be given using link.exe's **/out**: option.
 - **-pedantic** Ignored, cl.exe does not support any equivalent.
 - **\*.(cc|cxx|C)** C++ source file, **/Tp** option given to cl.exe

All other options are passed untouched to either cl.exe or link.exe

cccl will give options to cl.exe to correctly compile C++ source files
even if they don't have a .cpp extension (.cc, .cxx, or .C).

If any object file (.o, .obj) or library file (.a, .lib) is seen in the 
command line, cccl will invoke link.exe.

