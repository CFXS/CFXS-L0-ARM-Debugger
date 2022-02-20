# CFXS Embedded C/C++ Debugger [ARM Cortex-M/A]
![CFXS L0](https://github.com/CFXS/CFXS-Hardware-Debugger/blob/master/Logo.png)

# Build/Development Requirements
- Still working on these...

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
The main reason is the existance of very expensive paid software like IAR that just does not work.
We use IAR at work because from what we have tested - it is the best option (until this project reaches a working state)

Problems with IAR:
- The whole screen flashes for ~10 seconds while it transforms all of its windows from editor to debugger mode
- The LiveWatch window also just flashes constantly for no good reason (probably because of the outdated UI framework)
- When a project gets very complicated, IAR decides to reset the CPU when trying to step/run after hitting a breakpoint or pausing execution.
- In some cases it fails to understand its own ELF file and crashes instantly on load
- The code editor/project part of IAR is just tragic
- The debugger is the laggiest thing you will ever experience in your entire lifetime (upload to debug time is usually >30 seconds and not because of program size or debug probe flashing speed)
- Hard to use, as it constantly crashes when you look at it the wrong way
- IAR crashes at least a few times/day + exactly when you are about to catch a hard to repeat bug
- /r/softwaregore: `"Some things are better left unseen. You wouldn't be able to make a worse "IDE" even if you tried."`
- A custom debugger has to be developed so we can move from the IAR compiler to an actual proper functional compiler like GCC or Clang

Other debuggers probably are not as bad, but the debuggers I have tried using are just not good enough for our debug flow (as fast and simple as possible with custom extendable functionality, without 27.3 crashes/second and with a Watch window that works for C++ STL stuff)
