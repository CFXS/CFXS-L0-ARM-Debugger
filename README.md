# CFXS L0 ARM Debugger For Embedded Devices
![CFXS L0](https://github.com/CFXS/CFXS-L0-ARM-Debugger/blob/master/Logo.png)

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
- `Other probes too... 2023 maybe?`

---

# Why this project exists
The main reason is the existance of very expensive paid software like IAR that just does not work.
CFXS L0 will be a simple to use tool for getting stuff done fast, easy and without excessive headaches.
