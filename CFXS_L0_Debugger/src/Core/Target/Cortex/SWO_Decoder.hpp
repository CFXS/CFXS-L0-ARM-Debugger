// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
#pragma once

namespace L0::Cortex {

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

}; // namespace L0::Cortex