#pragma once

namespace HWD::Probe {

    enum class TargetStatus {
        CONNECTED,          // connected to targer
        DISCONNECTED,       // not connected to target
        NO_TARGET_DETECTED, // target not present
        CONNECTION_ERROR,   // error while connecting to target
    };

    class IProbe {
    public:
        IProbe()          = default;
        virtual ~IProbe() = default;

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
