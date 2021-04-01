#pragma once

#include "../IProbe.hpp"

namespace HWD::Probe {

    /// Segger J-Link Devices
    class JLink : public IProbe {
    public:
        JLink();
        virtual ~JLink();

        const std::string& GetModel() const override;
        const std::string& GetSerialNumber() const override;

    private:
        std::string m_ModelName    = "Unknown";
        std::string m_SerialNumber = "Unknown";
    };

} // namespace HWD::Probe