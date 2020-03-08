# Segatools

## Compiling on Windows:
### Prerequisites 
Download MinGW-W64 from [SourceForge](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/)
Download the latest msi release of Meson from [their GitHub releases](https://github.com/mesonbuild/meson/releases)
Download the latest version of Chocolatey from [their website](https://chocolatey.org/install)

Install MinGW-W64 and Meson.
In a Administrative PowerShell window type `choco install ninja`

Go to your MinGW-W64 folder (I made mine C:\mingw64 when I installed it) and double click on mingw-w64.bat.
To verify everything's working, type gcc --version. It should print out the version number of GCC, the C++ compiler included with MinGW.

Go to the cloned directory inside of the command prompt window that popped up.
Now, type:

```
meson --cross cross-mingw-64.txt _build64
ninja -C _build64
```
If you need the 32-bit files for whatever reason, substitute 64 for 32 in each line.

If you make changes and want to re-build it, you only need to do `ninja -C _build64` and it will rebuild everything.

If you want to generate Visual Studio projects, type:
```
meson --backend vs2019 --cross cross-mingw-64.txt _build64
```
(substituting vs2019 for any version from vs2015 to 2019 as needed).
I personally couldn't get it to compile using VS, so I recommend making the VS projects go to `_build64VS` instead of `_build64`, that way you can make changes inside Visual Studio, then compile using ninja in the `_build64` folder
