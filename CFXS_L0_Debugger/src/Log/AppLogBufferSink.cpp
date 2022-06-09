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
#include "AppLogBufferSink.hpp"

//////////////////////////////////////////////////////////////////////
std::vector<QString> s_AppLogBuffer;
size_t s_AppLogBufferTotalSize;
//////////////////////////////////////////////////////////////////////

void e_ClearApplicationLog() {
    s_AppLogBuffer.clear();
    s_AppLogBufferTotalSize = 0;
    emit _L0_AppLogBufferSinkNotifier::GetInstance()->Updated(s_AppLogBuffer, s_AppLogBufferTotalSize);
}

const std::vector<QString>& L0_AppLogBufferSink::GetLogBuffer() {
    return s_AppLogBuffer;
}

size_t L0_AppLogBufferSink::GetBufferSize() {
    return s_AppLogBufferTotalSize;
}

size_t L0_AppLogBufferSink::GetEntryCount() {
    return s_AppLogBuffer.size();
}

std::vector<QString>& L0_AppLogBufferSink::GetLogBufferRef() {
    return s_AppLogBuffer;
}

size_t& L0_AppLogBufferSink::GetBufferSizeRef() {
    return s_AppLogBufferTotalSize;
}