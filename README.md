# CFXS Embedded C/C++ Debugger [ARM Cortex-M/A]
![CFXS HWD](https://github.com/CFXS/CFXS-Hardware-Debugger/blob/master/Logo.png)

# Build/Development Requirements
- `JLink_x64.dll` located in "C:\CFXS\JLink_x64.dll" (Temporary. DLL file can be found in the installation files of `SEGGER Embedded Studio`/`JLink Tools`/`IAR Embedded Workbench`)
- `VSCode` or `Visual Studio`
- `ninja-build` (https://ninja-build.org/)
- `Visual Studio (Windows MSVC Compiler)` (https://visualstudio.microsoft.com/)
- `CMake` (https://cmake.org/)
- `Python` (https://www.python.org/)

# Planned Features
- Will not crash every 5-15 minutes
- Dark theme enabled by default
- Live variable/expression Watch
- Option to translate values in a Stack View window to function names (match stack value to function code ranges to obtain possible function name from a value in the stack. This is useful when you are not able to obtain the call stack because of a corrupted stack or whatever else, so you have to go through the stack and manually find Link Register values)
- STL container support in Watch window
- User definable custom data structure decoding in Watch window
- Macros and plugins for custom extended functionality (`Lua`)
- Some type of VSCode integration somehow? (at least "Open current file in VSCode")
 
# Debug Probe Support
- `SEGGER J-Link`
- `Other probes too... 2022 maybe?`

---

# Why this project exists
The main reason is the existance of IAR

At work we use IAR Embedded Workbench for our hardware related projects. It truly is a sad excuse for an IDE/Debugger.
- When a project gets very complicated, IAR decides to reset the CPU when trying to step/run after hitting a breakpoint or pausing execution.
- The code editor/project part of IAR is just tragic
- The debugger is the laggiest thing you will ever experience in your entire lifetime (upload to debug time is usually >30 seconds)
- Hard to use, as it constantly crashes when you look at it the wrong way
- Has cost us in total at least 6+ months of work time just from having to struggle with it being absolutely unusable with all the crashing and not showing some important debug data until the target CPU is halted (so what do you do in cases where the target is in a state where it is not able to be halted? This happened at work one time - as HWD was in still in early development, I threw together a quick test build of HWD and was able to solve the problem using a live Program Counter log to see where the CPU locked up and find + fix the problem **in 2 minutes instead of struggling for 2 hours with no results**)
- IAR crashes at least a few times/day + exactly when you are about to catch a hard to repeat bug
- Reporting issues to IAR result in them telling us to update to the latest IAR release, which still does not fix anything
- A custom debugger has to be developed so we can move from the IAR compiler to an actual proper functional compiler like GCC
- /r/softwaregore: `"Some things are better left unseen. You wouldn't be able to make a worse "IDE" even if you tried." (IAR)`

Other debuggers probably are not as bad, but I have tried other debuggers and they are just not good enough for our debug flow (as fast and simple as possible with custom extendable functionality)

Also other debuggers do not really support C++ good enough for our requirements. All of our ARM based projects are C++, so a debugger than can handle C++ data structure viewing is required (vector, map, list, other STL containers).
