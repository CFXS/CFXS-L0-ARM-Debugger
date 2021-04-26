#pragma once

namespace HWD::Cortex {

    class SWO_Decoder {
    public:
        SWO_Decoder();

        /// Process SWO stream data
        /// \param data pointer to SWO data buffer
        /// \param n number of bytes in data buffer
        void ProcessData(const uint8_t* data, size_t n);

    private:
        /// Reset decoder state (after decoded packet)
        void Reset();

        /// Clear sync flag (Sync should be cleared after a failed decode or unknown packet header)
        /// ClearSync calls Reset
        void ClearSync();

        /// Set sync flag (After a synchronization packet has been received)
        void SetSync();

        /// \return true if decoder has received a synchronization packet since initializing or cleared sync
        bool IsInSync();

    private:
        bool m_InSync = false; // A synchronization packet has been received
    };

}; // namespace HWD::Cortex