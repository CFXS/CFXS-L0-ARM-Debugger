#pragma once

#include "../IProbe.hpp"
#include "Driver/JLink_Driver.hpp"

namespace HWD::Probe {

    class JLink_Probe : public IProbe {
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

        std::shared_ptr<Driver::JLink_Driver> GetDriver() const {
            return m_Driver;
        }

        auto GetRawSerialNumber() const {
            return m_RawSerialNumber;
        }

    private:
        std::shared_ptr<Driver::JLink_Driver> m_Driver;

        decltype(Driver::JLink_Types::ProbeInfo::serialNumber) m_RawSerialNumber = 0;

        std::string m_ModelName          = "Unknown";
        std::string m_SerialNumberString = "Unknown";
    };

} // namespace HWD::Probe