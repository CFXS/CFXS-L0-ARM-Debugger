#pragma once

namespace HWD::Probe {

    enum class TargetStatus {
        CONNECTED,
        DISCONNECTED,
        CONNECTION_ERROR,
    };

    class IProbe {
    public:
        IProbe() {
            HWDLOG_PROBE_TRACE("IProbe::IProbe() @ {0}", fmt::ptr(this));
        };
        virtual ~IProbe() {
            HWDLOG_PROBE_TRACE("IProbe::~IProbe() @ {0}", fmt::ptr(this));
        };

        /// \return is probe ready for use
        virtual bool IsInitialized() const = 0;

        /// Connect to probe
        /// \return success
        virtual bool OpenConnection() = 0;

        /// Disconnect from probe
        /// \return success
        virtual bool CloseConnection() = 0;

        /// \return is connected to debug probe
        virtual bool IsConnectionOpen() const = 0;

        /// \return debug probe model name string
        virtual const std::string& GetModelName() const = 0;

        /// \return debug probe serial number string
        virtual const std::string& GetSerialNumberString() const = 0;

        /// \return target status
        virtual TargetStatus GetTargetStatus() const = 0;

    private:
    };

} // namespace HWD::Probe
