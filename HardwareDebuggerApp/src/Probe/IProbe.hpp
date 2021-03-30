#pragma once

namespace HWD::Probe {

    class IProbe {
    public:
        IProbe()          = default;
        virtual ~IProbe() = default;

        /// Get debug probe model name string
        virtual const std::string& GetModel() const = 0;

        /// Get debug probe serial number string
        virtual const std::string& GetSerialNumber() const = 0;

    private:
    };

} // namespace HWD::Probe
