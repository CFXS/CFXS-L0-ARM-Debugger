#pragma once

#include "IProbe.hpp"

namespace HWD::Probe {

    /// Segger J-Link Devices
    class JLink : public IProbe {
    public:
        JLink();
        virtual ~JLink();

        const std::string& GetModel() const override;
        const std::string& GetSerialNumber() const override;

    private:
        std::string m_Model        = "SEGGER J-Link Mini";
        std::string m_SerialNumber = "801022602";
    };

} // namespace HWD::Probe