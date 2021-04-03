# CFXS Embedded C/C++ Debugger [ARM Cortex-M/A]
![CFXS HWD](https://github.com/CFXS/CFXS-Hardware-Debugger/blob/master/Logo.png)

# Build/Development Requirements
- `JLink_x64.dll` located in "C:\CFXS\JLink_x64.dll" (Temporary. DLL file can be found in the installation files of SEGGER Embedded Studio)
- `VSCode` or `Visual Studio`
- `ninja-build` (https://ninja-build.org/)
- `Visual Studio (Windows MSVC Compiler)` (https://visualstudio.microsoft.com/)
- `CMake` (https://cmake.org/)
- `Python` (https://www.python.org/)

# Planned Features
- Will not crash every 5-15 minutes
- Dark theme enabled by default
- Live variable/expression Watch
- STL container support in Watch window
- User definable custom data structure decoding in Watch window
- Macros and plugins for custom extended functionality (`Lua`)
- Some type of VSCode integration somehow? (at least "Open current file in VSCode")
 
# Debug Probe Support
- `SEGGER J-Link`
- `Other probes too... some time next year maybe?`

---

# Why this project exists
When your embedded C/C++ IDE/Debugger crashes enough times a day and is just difficult to work with, you might just want to make your own debugger.  
So this is it - my own debug tool.  
The solution to all problems of IDEs/debuggers I have tested - a debugger with some IDE functionality that is actually easy to use and powerful enough for serious debugging (hopefully)
