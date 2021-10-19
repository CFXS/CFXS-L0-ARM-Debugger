// ---------------------------------------------------------------------
// CFXS Hardware Debugger <https://github.com/CFXS/CFXS-Hardware-Debugger>
// Copyright (C) 2021 | CFXS
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

#ifdef __cplusplus
//////////////////////////////////////////////////////////////////////////////
// Platform specific library load
#if defined(CFXS_PLATFORM_WINDOWS)
#include <Windows.h>

namespace HWD {

    class _DynamicLibrary {
        friend class DynamicLibrary;

        using LibraryHandle_t = HMODULE;
#ifdef UNICODE
        using PathString_t = LPCWSTR;
#else
        using PathString_t = LPCSTR;
#endif // !UNICODE

    private:
        static inline LibraryHandle_t Load(PathString_t path) {
            return LoadLibrary(path);
        }

        static inline bool Unload(LibraryHandle_t handle) {
            if (handle) {
                return FreeLibrary(handle);
            } else {
                return false;
            }
        }

        static inline auto GetFunctionAddress(LibraryHandle_t libraryHandle, const char* functionName) {
            return GetProcAddress(libraryHandle, functionName);
        }

        static int GetErrorCode() {
            return ::GetLastError();
        }

        static std::string GetErrorString() {
            DWORD errorMessageID = GetErrorCode();

            if (!errorMessageID)
                return {};

            LPSTR messageBuffer = nullptr;
            size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                         NULL,
                                         errorMessageID,
                                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                         (LPSTR)&messageBuffer,
                                         0,
                                         NULL);

            std::string message(messageBuffer, size);
            LocalFree(messageBuffer);

            message.erase(std::find_if(message.rbegin(),
                                       message.rend(),
                                       [](char ch) {
                                           return !std::isspace(ch);
                                       })
                              .base(),
                          message.end());

            message.erase(message.end() - 1);

            return message;
        }
    };

} // namespace HWD

#else
#error "Unsupported platform"
#endif
#endif

namespace HWD {

    class DynamicLibrary {
    public:
        DynamicLibrary(const std::string& path) {
            HWDLOG_CORE_TRACE("Loading dynamic library \"{0}\"", path);
            m_Handle = _DynamicLibrary::Load(path.c_str());

            if (m_Handle == nullptr) {
                HWDLOG_CORE_ERROR(" - Failed to load library: ({0}) - {1}", GetErrorCode(), GetErrorString());
            }
        }

        ~DynamicLibrary() {
            if (m_Handle) {
                _DynamicLibrary::Unload(m_Handle);
            }
        }

        inline bool IsLoaded() {
            return m_Handle != nullptr;
        }

        inline int GetErrorCode() {
            // m_Handle == nullptr not handled
            return _DynamicLibrary::GetErrorCode();
        }

        inline std::string GetErrorString() {
            // m_Handle == nullptr not handled
            return _DynamicLibrary::GetErrorString();
        }

        template<typename FunctionSignature>
        FunctionSignature GetFunction(const char* functionName) {
            if (!m_Handle) {
                HWDLOG_CORE_ERROR("Failed to load function \"{0}\" - library not loaded", functionName);
                return nullptr;
            }

            auto func = reinterpret_cast<FunctionSignature>(_DynamicLibrary::GetFunctionAddress(m_Handle, functionName));

            if (!func) {
                HWDLOG_CORE_ERROR("Failed to load function \"{0}\": [{1}]({2})", functionName, GetErrorCode(), GetErrorString());
                return nullptr;
            }

            return func;
        }

    private:
        _DynamicLibrary::LibraryHandle_t m_Handle;
    };

} // namespace HWD