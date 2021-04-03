#pragma once

#include <Target/TargetDevices.hpp>

namespace HWD::Probe {

    class IProbe {
    public:
        enum class DebugInterface { SWD };
        inline const char* TargetInterfaceToString(DebugInterface interface) {
            if (interface == DebugInterface::SWD)
                return "SWD";
            else
                return "Unknown";
        }

    public:
        IProbe()          = default;
        virtual ~IProbe() = default;

        /// Process loop
        virtual void Process() = 0;

        /// Is probe ready for use
        /// \return is probe ready for use
        virtual bool Probe_IsReady() const = 0;

        /// Connect to probe hardware
        /// \return true if connected successfully
        virtual bool Probe_Connect() = 0;

        /// Disconnect from probe hardware
        /// \return true if disconnected successfully
        virtual bool Probe_Disconnect() = 0;

        /// Is connected to probe hardware
        /// \return true if debug probe connection open
        virtual bool Probe_IsConnected() const = 0;

        /// Get probe model name
        /// \return debug probe model name string
        virtual const std::string& GetModelName() const = 0;

        /// Get probe serial number string
        /// \return debug probe serial number string
        virtual const std::string& GetSerialNumberString() const = 0;

        /// Select target device
        /// \return true if selected/supported
        virtual bool Target_SelectDevice(const TargetDeviceDescription& device) = 0;

        /// Select probe-target interface
        /// \return true if selected successfully
        virtual bool Target_SelectDebugInterface(DebugInterface interface) = 0;

        /// Probe connection to target open
        /// \return true if target connected
        virtual bool Target_IsConnected() const = 0;

        /// Start probe connection to target
        /// \return true if connected successfully
        virtual bool Target_Connect() = 0;

        /// Erase target program
        /// \return true if erased succesfully
        virtual bool Target_Erase() = 0;

        /// Program target
        /// \return true if programmed succesfully
        virtual bool Target_WriteProgram(const uint8_t* data, uint32_t size) = 0;

        /// Reset target
        /// \return true if reset successfully
        virtual bool Target_Reset(bool haltAfterReset = true) = 0;

        /// Start terminal driver
        /// \return true if started successfully
        virtual bool Target_StartTerminal(void* params = nullptr) = 0;

        /// Get terminal buffer
        virtual const char* Target_GetTerminalBuffer() = 0;

        /// Read 32bit value from address
        virtual bool Target_ReadMemory_32(uint32_t address, uint32_t* readTo) = 0;

        /// Read 64bit value from address
        virtual bool Target_ReadMemory_64(uint32_t address, uint64_t* readTo) = 0;

        /// Write 32bit value from address
        virtual bool Target_WriteMemory_32(uint32_t address, uint32_t value) = 0;

        virtual bool Target_Halt()              = 0;
        virtual bool Target_Run()               = 0;
        virtual bool Target_IsRunning()         = 0;
        virtual float Target_GetFlashProgress() = 0;

    private:
    };

} // namespace HWD::Probe
