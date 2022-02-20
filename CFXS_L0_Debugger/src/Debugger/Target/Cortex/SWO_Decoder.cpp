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
#include "SWO_Decoder.hpp"

namespace L0::Cortex {

    SWO_Decoder::SWO_Decoder() {
        //
    }

    ///////////////////////////////////////

    void SWO_Decoder::ProcessData(const uint8_t* data, size_t n) {
        //
    }

    inline void SWO_Decoder::Reset() {
    }

    inline void SWO_Decoder::ClearSync() {
    }

    inline void SWO_Decoder::SetSync() {
    }

    inline bool SWO_Decoder::IsInSync() {
    }

} // namespace L0::Cortex