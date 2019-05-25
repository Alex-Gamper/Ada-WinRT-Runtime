## Ada-WinRt-Runtime

This project provides a Ada Runtime that can be used with the WinRT bindings and the resulting application
will be deployable on Windows Store. Note the current FSF/GNAT runtime makes use of Windows Api's that are
not compatible with deployment to Windows Store.

Note : this is a work in progress (a formal announcement will be made once complete, and this message will
be removed, so please check back on the status periodically, thanks)

### Project Status

Project has been released and considered usable. 

### Prerequisites

You will need a working gcc/gnat **x64** Windows build environment. This release has **NOT** yet been
tested against 32 bit environments. You may need to build gcc/gnat **x64** from source if required.
Details/scripts on how to do this can be found on the mingw64 website, alternatively install using MSYS2

### Project Dependencies

None

### Configuration Instructions

The Winrt_Runtime.gpr needs to know where to install the project files/artifacts, this is specified by the
variable named Base_Installation_Dir within the gpr file. You will need to change this value to suit your
environment.

For example

* MSYS2 (gcc version 8.3) - Base_Installation_Dir := "lib\gcc\x86_64-w64-mingw32\8.3.1/";
* MSYS2 (gcc version 9.1) - Base_Installation_Dir := "lib\gcc\x86_64-w64-mingw32\9.1.1/";
* AdaCore CE 2018 (gcc version 7.3) - Base_Installation_Dir := "lib\gcc\x86_64-pc-mingw32\7.3.1/";
* AdaCore CE 2019 (gcc version ?.?) - Base_Installation_Dir := "lib\gcc\x86_64-pc-mingw32\?.?.?/";

### Build instructions

From a Visual Studio command prompt that has your gcc/gnat x64 distribution/build environments bin directory in its path,
simply run the following command. Note that the Visual Studio command prompt is needed for the Link command.

* .\Build.cmd

Alternatively, run the folloing commands from a visual studio command prompt

* .\gprbuild -p -P Winrt_runtime.gpr
* link /lib /machine:x64 /nodefaultlib /out:.\lib\libother.a .\lib\\*.o
* link /lib /machine:x64 /nodefaultlib /out:.\libgnarl\libgnarl.a .\libgnarl\\*.o
* .\gprinstall -f -p -P Winrt_runtime.gpr


### Notes

If you have previously built this runtime for a different target/version. Then please clean your previous env
by executing the following command

* .\gprclean -P Winnrt_runtime.gpr

### Feedback

Welcome
