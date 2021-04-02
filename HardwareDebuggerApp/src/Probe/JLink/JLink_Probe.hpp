#pragma once

#include "../IProbe.hpp"
#include "Driver/JLink_Driver.hpp"

namespace HWD::Probe {

    class JLink_Probe final : public IProbe {
    public:
        virtual ~JLink_Probe();

        bool IsInitialized() const override;
        bool OpenConnection() override;
        bool CloseConnection() override;
        bool IsConnectionOpen() const override;
        const std::string& GetModelName() const override;
        const std::string& GetSerialNumberString() const override;
        TargetStatus GetTargetStatus() const override;

    public:
        static const std::vector<JLink_Probe*>& GetConnectedProbes();

    private:
        JLink_Probe(const Driver::JLink_Types::ProbeInfo& probeInfo);

        std::shared_ptr<Driver::JLink_Driver> GetDriver() const;
        uint32_t GetRawSerialNumber() const;

        void DisableFlashProgressPopup();

    private:
        std::shared_ptr<Driver::JLink_Driver> m_Driver;

    private: // generic properties
        uint32_t m_RawSerialNumber       = 0;
        std::string m_ModelName          = "Unknown";
        std::string m_SerialNumberString = "Unknown";
    };

} // namespace HWD::Probe