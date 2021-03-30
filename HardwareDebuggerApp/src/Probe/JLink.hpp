#pragma once

#include "IProbe.hpp"

namespace HWD::Probe {

    /// Segger J-Link Devices
    class JLink : public IProbe {
    public:
        JLink();
        virtual ~JLink();

        const std::string& GetModel() const;
        const std::string& GetSerialNumber() const;

    private:
    };

} // namespace HWD::Probe