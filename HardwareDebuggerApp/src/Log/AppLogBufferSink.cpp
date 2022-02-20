// [CFXS] //
#include "AppLogBufferSink.hpp"

//////////////////////////////////////////////////////////////////////
std::vector<QString> s_AppLogBuffer;
size_t s_AppLogBufferTotalSize;
//////////////////////////////////////////////////////////////////////

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