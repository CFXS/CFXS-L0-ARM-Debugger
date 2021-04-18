#pragma once

#include <Probe/IProbe.hpp>
#include "Driver/JLink_Driver.hpp"

namespace HWD::Probe {

    class JLink final : public IProbe {
        friend struct ProbeCallbackEntry;

        using MessageCallback       = Driver::JLink_Types::LogCallback;
        using FlashProgressCallback = Driver::JLink_Types::FlashProgressCallback;

    public:
        // !!! UPDATE InitializeProbeCallbackArray IF CHANGED !!!
        static constexpr auto MAX_DISCOVERABLE_PROBE_COUNT = 8; // limit max discoverable probe count

    public:
        virtual ~JLink();

        /////////////////////////////////////////
        void Process() override;
        /////////////////////////////////////////
        // Probe overrides
        bool Probe_IsReady() const override;
        bool Probe_Connect() override;
        bool Probe_Disconnect() override;
        bool Probe_IsConnected() const override;

        const std::string& JLink::GetModelName() const override {
            return m_ModelName;
        }

        const std::string& JLink::GetSerialNumberString() const override {
            return m_SerialNumberString;
        }
        /////////////////////////////////////////
        // Probe target overrides
        virtual bool Target_SelectDevice(const Target::DeviceDescription& device) override;
        virtual bool Target_SelectDebugInterface(DebugInterface interface) override;
        virtual bool Target_IsConnected() const override;
        virtual bool Target_Connect() override;
        virtual bool Target_Erase() override;
        virtual bool Target_WriteProgram(const uint8_t* data, uint32_t size) override;
        virtual bool Target_Reset(bool haltAfterReset = true) override;
        virtual bool Target_StartTerminal(void* params = nullptr) override;
        virtual const char* Target_GetTerminalBuffer() override;

        uint8_t Target_ReadMemory_8(uint32_t address, bool* success = nullptr) override;
        uint16_t Target_ReadMemory_16(uint32_t address, bool* success = nullptr) override;
        uint32_t Target_ReadMemory_32(uint32_t address, bool* success = nullptr) override;
        uint64_t Target_ReadMemory_64(uint32_t address, bool* success = nullptr) override;
        int Target_ReadMemoryTo(uint32_t address, void* to, uint32_t bytesToRead, AccessWidth accessWidth) override;
        bool Target_WriteMemory_32(uint32_t address, uint32_t val) override;
        uint32_t Target_Get_ROM_Table_Address() override;
        virtual bool Target_Halt() override;
        virtual bool Target_Run() override;
        virtual bool Target_IsRunning() override;
        virtual float Target_GetFlashProgress() override;
        /////////////////////////////////////////

    public:
        static std::vector<JLink*> s_GetConnectedProbes();

    private:
        static void s_InitializeProbeCallbackArray();

    private:
        /// \param probeInfo probe data
        /// \param probeIndex index to use for mapping callbacks (probe index in s_Probes)
        JLink(const Driver::JLink_Types::ProbeInfo& probeInfo, int probeIndex);

        std::shared_ptr<Driver::JLink_Driver> JLink::GetDriver() const {
            return m_Driver;
        }

        uint32_t JLink::GetRawSerialNumber() const {
            return m_RawSerialNumber;
        }

        // probe callbacks
        void Probe_LogCallback(const char* message);
        void Probe_WarningCallback(const char* message);
        void Probe_ErrorCallback(const char* message);
        void Probe_FlashProgressCallback(const char* action, const char* prog, int percentage);

        // specific config
        void Probe_DisableFlashProgressPopup();

        // Private probe stuff
        void UpdateProbeInfo();

        // Private target stuff
        void PrepareTarget();
        void UpdateTargetInfo();

    private:
        // An array is used instead of a vector to be able to set error/warning/log handlers for each instance safely (because of threads)
        static std::array<JLink*, MAX_DISCOVERABLE_PROBE_COUNT> s_Probes; // list of all JLink probes

    private:
        std::shared_ptr<Driver::JLink_Driver> m_Driver;
        int m_ProbeIndex;

        // terminal
        bool m_TerminalEnabled = true;
        std::vector<char> m_TerminalBuffer;

        // flash
        float m_FlashProgress = 0;

        // probe
        Driver::JLink_Types::ProbeCapabilities m_ProbeCapabilities = {}; // initialize to 0

    private: // generic properties
        uint32_t m_RawSerialNumber       = 0;
        std::string m_ModelName          = "Unknown";
        std::string m_SerialNumberString = "Unknown";
    };

} // namespace HWD::Probe