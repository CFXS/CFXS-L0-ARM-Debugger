# CFXS L0 ARM Debugger For Embedded Devices
![CFXS L0](https://github.com/CFXS/CFXS-L0-ARM-Debugger/blob/master/Logo.png)

# Build/Development Requirements
- Still working on these...

# Planned Features
- Will not crash every 5-15 minutes
- Dark theme because it's not 2010 anymore
- Live variable/expression Watch
- Option to translate values in a Stack View window to function names (match stack value to function code ranges to obtain possible function name from a value in the stack. This is useful when you are not able to obtain the call stack because of a corrupted stack or whatever else, so you have to go through the stack and manually find Link Register values)
- STL container support in Watch window
- User definable custom data structure decoding in Watch window
- Macros and plugins for custom extended functionality (`Lua`)
- External memory view/watch data interfaces (Internal JTAG/SWD and option to add external interfaces like a custom serial or network protocol for use with devices that don't allow RAM reads while the core is running. In that case it would be possible to set up a blocking memory read function on the target device that would send a block of memory to L0 through serial or network for live watch use or whatever else)
 
# Debug Probe Support
- `SEGGER J-Link`
- `Other probes too... 2023 maybe?`

---

# Why this project exists
The main reason is the existance of very expensive paid software like IAR that just does not work.

CFXS L0 will be a simple to use tool for getting stuff done fast, easy and without excessive headaches.
