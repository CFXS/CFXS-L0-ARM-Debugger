#pragma once

namespace HWD::Probe {

    class IProbe {
    public:
        IProbe() {
            HWDLOG_PROBE_TRACE("IProbe::IProbe() @ 0x{0}", fmt::ptr(this));
        };
        virtual ~IProbe() {
            HWDLOG_PROBE_TRACE("IProbe::~IProbe() @ 0x{0}", fmt::ptr(this));
        };

        /// Get debug probe model name string
        virtual const std::string& GetModel() const = 0;

        /// Get debug probe serial number string
        virtual const std::string& GetSerialNumber() const = 0;

    private:
    };

} // namespace HWD::Probe
