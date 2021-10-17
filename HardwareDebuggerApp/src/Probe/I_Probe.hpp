#pragma once

#include <Target/SupportedDevices.hpp>

namespace HWD::Probe {

    class I_Probe {
        static I_Probe*& GetCurrentProbeRef() {
            static I_Probe* s_CurrentProbe = nullptr;
            return s_CurrentProbe;
        }

    public:
        enum class AccessWidth { _1 = 1, _2 = 2, _4 = 4 };

        enum class DebugInterface { SWD };
        static const char* DebugInterfaceToString(DebugInterface interface) {
            if (interface == DebugInterface::SWD)
                return "SWD";
            else
                return "Unknown";
        }

        static I_Probe* GetCurrentProbe() {
            return GetCurrentProbeRef();
        }

        static void SetCurrentProbe(I_Probe* probe) {
            HWDLOG_PROBE_INFO("Set current probe to {0} from thread {1}", fmt::ptr(probe), std::this_thread::get_id());
            GetCurrentProbeRef() = probe;
        }

    public:
        I_Probe()          = default;
        virtual ~I_Probe() = default;

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
        virtual bool Target_SelectDevice(const Target::DeviceDescription& device) = 0;

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

        /// Read 8bit value from address
        virtual uint8_t Target_ReadMemory_8(uint32_t address, bool* success = nullptr) = 0;

        /// Read 16bit value from address
        virtual uint16_t Target_ReadMemory_16(uint32_t address, bool* success = nullptr) = 0;

        /// Read 32bit value from address
        virtual uint32_t Target_ReadMemory_32(uint32_t address, bool* success = nullptr) = 0;

        /// Read 64bit value from address
        virtual uint64_t Target_ReadMemory_64(uint32_t address, bool* success = nullptr) = 0;

        /// Read Program Counter
        virtual uint64_t Target_ReadPC(bool* success = nullptr) = 0;

        /// Read multiple values to address
        /// \return bytes read
        virtual int Target_ReadMemoryTo(uint32_t address, void* to, uint32_t bytesToRead, AccessWidth accessWidth) = 0;

        /// Write 32bit value to address
        /// \return true on success
        virtual bool Target_WriteMemory_32(uint32_t address, uint32_t val) = 0;

        virtual bool Target_Halt()              = 0;
        virtual bool Target_Run()               = 0;
        virtual bool Target_IsRunning()         = 0;
        virtual float Target_GetFlashProgress() = 0;

    private:
    };

} // namespace HWD::Probe

template<>
struct fmt::formatter<HWD::Probe::I_Probe::DebugInterface> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(HWD::Probe::I_Probe::DebugInterface const& ifc, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{0}", HWD::Probe::I_Probe::DebugInterfaceToString(ifc));
    }
};