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

#include "STLink.hpp"

#include <set>
#include <QString>

namespace L0::Probe {

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr bool LOG_ENABLED       = true;
    static constexpr bool WARN_LOG_ENABLED  = true;
    static constexpr bool ERROR_LOG_ENABLED = true;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    void STLink::L0_Load() {
        InitializeChipIDs();
    }

    void STLink::L0_Unload() {
    }

    STLink::STLink() {
        LOG_PROBE_TRACE("[STLink@{0}] Constructor", fmt::ptr(this));
        m_DeviceAssigned = false;
    }

    STLink::~STLink() {
        LOG_PROBE_TRACE("[STLink@{0}] Destructor", fmt::ptr(this));

        if (Probe_IsConnected())
            Probe_Disconnect();
    }

    //////////////////////////////////////////////////////////////////////

    // Select working device by serial number
    void STLink::L0_SelectDevice(const QString& serialNumber) {
        if (Probe_IsConnected())
            Probe_Disconnect();

        m_SerialNumberString = serialNumber;
        m_DeviceAssigned     = true;
    }

    bool STLink::Probe_IsReady() const {
        return m_DeviceAssigned;
    }

    /////////////////////////////////////////////////////////////
    // Probe

    const QString& STLink::GetModelName() const {
        return m_ModelName;
    }

    const QString& STLink::GetSerialNumberString() const {
        return m_SerialNumberString;
    }

    bool STLink::Probe_Connect() {
        if (!Probe_IsReady()) {
            LOG_PROBE_WARN("[STLink@{0}] Not ready", fmt::ptr(this));
            return false;
        }

        if (Probe_IsConnected()) {
            LOG_PROBE_WARN("[STLink@{0}] Not connecting to probe - already connected", fmt::ptr(this));
            return true;
        }

        m_Driver = stlink_open_usb(UERROR, CONNECT_NORMAL, m_SerialNumberString.toUtf8().data(), 0, 0);
        if (!m_Driver) {
            LOG_PROBE_ERROR("[STLink@{0}] Failed to connect to probe - {1}", fmt::ptr(this), m_SerialNumberString);
            return false;
        } else {
            LOG_PROBE_TRACE("[STLink@{0}] Connected to probe - {1}", fmt::ptr(this), m_SerialNumberString);
        }

        return true;
    }

    bool STLink::Probe_Disconnect() {
        if (!Probe_IsReady()) {
            LOG_PROBE_WARN("[STLink@{0}] Not ready", fmt::ptr(this));
            return false;
        }

        if (Probe_IsConnected()) {
            LOG_PROBE_TRACE("[STLink@{0}] Disconnect from probe", fmt::ptr(this));
            stlink_close(m_Driver);
            m_Driver = nullptr;
        } else {
            LOG_PROBE_WARN("[STLink@{0}] Not disconnecting - already disconnected", fmt::ptr(this));
        }
        return true;
    }

    bool STLink::Probe_IsConnected() const {
        return m_Driver != nullptr;
    }

    ////////////////////////////////////////////////////////////////////
    // Target

    // TODO: Implement error reason callback for higher level callers
    bool STLink::Target_SelectDevice(const Target::DeviceDescription& device) {
        //char resp[256];
        //ErrorCode ec = GetDriver()->probe_ExecuteCommand(fmt::format("Device = {0}\n", device.GetName()).c_str(), resp, 256);
        //
        //LOG_PROBE_TRACE("[STLink@{0}]:", fmt::ptr(this));
        //if (ec == ErrorCode::OK) {
        //    LOG_PROBE_TRACE("Selected target device \"{0}\"", device.GetName());
        //    LOG_PROBE_TRACE("Target memory regions:");
        //    for (auto& reg : device.GetMemoryRegions()) {
        //        LOG_PROBE_TRACE(" - {0:3}\t{1:6}\t0x{2:08X} - 0x{3:08X}",
        //                        To_C_String(reg.GetAccessPermissions()),
        //                        reg.GetName(),
        //                        reg.GetAddress(),
        //                        reg.GetAddress() + reg.GetSize());
        //    }
        //    return true;
        //} else {
        //    LOG_PROBE_ERROR("Failed to select target device \"{0}\" - {1}", device.GetName(), ec);
        //    return false;
        //}
        return false;
    }

    // TODO: Implement error reason callback for higher level callers
    bool STLink::Target_SelectDebugInterface(DebugInterface interface) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        switch (interface) {
            case DebugInterface::SWD: {
                m_DebugInterface = interface;
                return true;
            }
            case DebugInterface::JTAG: {
                LOG_PROBE_ERROR("[STLink@{0}] Failed to select target interface \"JTAG\" - not supported", fmt::ptr(this));
                return false;
            }
            default:
                LOG_PROBE_ERROR("[STLink@{0}] Failed to select target interface \"{1}\" - unknown interface", fmt::ptr(this), interface);
                return false;
        }
        return false;
    }

    bool STLink::Target_IsConnected() const {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        cortex_m3_cpuid_t temp;
        return stlink_cpu_id(m_Driver, &temp) == 0;
    }

    // TODO: Implement error reason callback for higher level callers
    bool STLink::Target_Connect() {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }

        auto res = stlink_target_connect(m_Driver, CONNECT_UNDER_RESET);

        if (res) {
            LOG_PROBE_ERROR("[STLink@{0}] Could not connect to target - {1}", fmt::ptr(this), res);
            return false;
        }

        if (m_DebugInterface != DebugInterface::SWD) {
            LOG_PROBE_ERROR("[STLink@{0}] Debug interface not set to SWD");
            L0_DEBUGBREAK();
            return false;
        }
        res = stlink_enter_swd_mode(m_Driver);
        if (res) {
            LOG_PROBE_ERROR("[STLink@{0}] Failed to enter SWD mode - {1}", fmt::ptr(this), res);
            return false;
        }

        return true;
    }

    // TODO: Implement error reason callback for higher level callers
    bool STLink::Target_Erase() {
        //ErrorCode ec = GetDriver()->target_Erase();
        //
        //if (ec == ErrorCode::OK) {
        //    LOG_PROBE_TRACE("[STLink@{0}] Target program erased", fmt::ptr(this));
        //    return true;
        //} else {
        //    LOG_PROBE_ERROR("[STLink@{0}] Failed to erase target program - {1}", fmt::ptr(this), ec);
        //    return false;
        //}
        return false;
    }

    // TODO: Implement error reason callback for higher level callers
    bool STLink::Target_WriteProgram(const uint8_t* data, uint32_t size) {
        //GetDriver()->target_BeginDownload(DownloadFlags::ALLOW_FLASH | DownloadFlags::ALLOW_BUFFERED_RAM);
        //
        //ErrorCode ret = GetDriver()->target_WriteProgram(0, size, (void*)data);
        //if ((int)ret >= 0) {
        //    ret = GetDriver()->target_EndDownload();
        //
        //    if ((int)ret >= 0 || (int)ret == -2) { // return code -2 is error code for target program matching requested program
        //        LOG_PROBE_TRACE("[STLink@{0}] Target programmed", fmt::ptr(this));
        //        return true;
        //    } else {
        //        const char* reason;
        //
        //        switch ((int)ret) {
        //            case -3: reason = "program/erase failed";
        //            case -4: reason = "verification failed";
        //            default: reason = "error";
        //        }
        //
        //        LOG_PROBE_ERROR("[STLink@{0}] Failed to program target - {1}", fmt::ptr(this), reason);
        //        return false;
        //    }
        //} else {
        //    LOG_PROBE_ERROR("[STLink@{0}] Failed to program target - {1}", fmt::ptr(this), ret);
        //    return false;
        //}
        return false;
    }

    // TODO: Implement error reason callback for higher level callers
    bool STLink::Target_Reset(bool haltAfterReset) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }

        auto res = stlink_reset(m_Driver, haltAfterReset ? RESET_SOFT_AND_HALT : RESET_AUTO);

        if (res) {
            LOG_PROBE_ERROR("[STLink@{0}] Failed to reset target - {1}", fmt::ptr(this), res);
            return false;
        }
        return true;
    }

    uint8_t STLink::Target_ReadMemory_8(uint32_t address, bool* success) {
        uint8_t ret;
        bool s = Target_ReadMemoryTo(address, &ret, sizeof(ret), AccessWidth::_1);
        if (success)
            *success = s;
        return ret;
    }

    uint16_t STLink::Target_ReadMemory_16(uint32_t address, bool* success) {
        uint16_t ret;
        bool s = Target_ReadMemoryTo(address, &ret, sizeof(ret), AccessWidth::_1);
        if (success)
            *success = s;
        return ret;
    }

    uint32_t STLink::Target_ReadMemory_32(uint32_t address, bool* success) {
        uint32_t ret;
        bool s = Target_ReadMemoryTo(address, &ret, sizeof(ret), AccessWidth::_1);
        if (success)
            *success = s;
        return ret;
    }

    uint64_t STLink::Target_ReadMemory_64(uint32_t address, bool* success) {
        uint64_t ret;
        bool s = Target_ReadMemoryTo(address, &ret, sizeof(ret), AccessWidth::_1);
        if (success)
            *success = s;
        return ret;
    }

    int STLink::Target_ReadMemoryTo(uint32_t address, void* to, uint32_t bytesToRead, AccessWidth accessWidth) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        auto readOrig = bytesToRead;
        while (bytesToRead & 3)
            bytesToRead++;
        auto res = stlink_read_mem32(m_Driver, address, bytesToRead);
        if (res) {
            LOG_PROBE_ERROR("[STLink@{}] Failed to read {} bytes from 0x{:X} - {}", fmt::ptr(this), bytesToRead, address, res);
            return 0;
        }
        memcpy(to, m_Driver->q_buf, readOrig);
        return readOrig;
    }

    bool STLink::Target_WriteMemory_8(uint32_t address, uint8_t value) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        memcpy(m_Driver->q_buf, &value, sizeof(value));
        m_Driver->q_len = 1;
        auto res        = stlink_write_mem8(m_Driver, address, sizeof(value));
        if (res) {
            LOG_PROBE_ERROR("[STLink@{}] Failed to write {} bytes to 0x{:X} - {}", fmt::ptr(this), 1, address, res);
            return false;
        }
        return true;
    }

    bool STLink::Target_WriteMemory_32(uint32_t address, uint32_t value) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        memcpy(m_Driver->q_buf, &value, sizeof(value));
        m_Driver->q_len = 1;
        auto res        = stlink_write_mem8(m_Driver, address, sizeof(value));
        if (res) {
            LOG_PROBE_ERROR("[STLink@{}] Failed to write {} bytes to 0x{:X} - {}", fmt::ptr(this), 1, address, res);
            return false;
        }
        return true;
    }

    bool STLink::Target_WriteMemory_64(uint32_t address, uint64_t value) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        memcpy(m_Driver->q_buf, &value, sizeof(value));
        m_Driver->q_len = 1;
        auto res        = stlink_write_mem8(m_Driver, address, sizeof(value));
        if (res) {
            LOG_PROBE_ERROR("[STLink@{}] Failed to write {} bytes to 0x{:X} - {}", fmt::ptr(this), 1, address, res);
            return false;
        }
        return true;
    }

    bool STLink::Target_WriteMemory_16(uint32_t address, uint16_t value) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        memcpy(m_Driver->q_buf, &value, sizeof(value));
        m_Driver->q_len = 1;
        auto res        = stlink_write_mem8(m_Driver, address, sizeof(value));
        if (res) {
            LOG_PROBE_ERROR("[STLink@{}] Failed to write {} bytes to 0x{:X} - {}", fmt::ptr(this), 1, address, res);
            return false;
        }
        return true;
    }

    bool STLink::Target_Halt(uint32_t waitHaltTimeout) {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }

        auto res = stlink_step(m_Driver);

        if (res) {
            LOG_PROBE_ERROR("[STLink@{0}] Failed to halt target - {1}", fmt::ptr(this), res);
            return false;
        }
        return true;
    }
    bool STLink::Target_Run() {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }

        auto res = stlink_run(m_Driver, RUN_NORMAL);

        if (res) {
            LOG_PROBE_ERROR("[STLink@{0}] Failed to run target - {1}", fmt::ptr(this), res);
            return false;
        }
        return true;
    }
    bool STLink::Target_IsRunning() {
        if (!Probe_IsConnected()) {
            L0_ASSERT(0, "Probe not connected");
            return false;
        }
        return !stlink_is_core_halted(m_Driver);
    }

    float STLink::Target_GetFlashProgress() {
        // return 0;
        //return m_FlashProgress;
        return 0;
    }

    uint64_t STLink::Target_ReadPC(bool* success) {
        // return GetDriver()->target_ReadRegister((Driver::STLink_Types::CPU_Registers::ARM)Driver::STLink_Types::Cortex_M4::R15);
        return 0;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Process

    static uint8_t s_SWO_Buffer[4096];
    void STLink::Process() {
        //uint32_t bytesRead = sizeof(s_SWO_Buffer); // bytes to read
        //GetDriver()->target_SWO_Read(s_SWO_Buffer, 0, &bytesRead);
        //// bytesRead now contains amount of bytes that were read
        //
        //if (bytesRead) {
        //    // have to manually flush SWO buffer - STLink DLL does not do this automatically
        //    // flush [bytesRead] number of bytes from internal buffers
        //    GetDriver()->target_SWO_Control(SWO_Command::FLUSH, &bytesRead);
        //
        //    SWO_Process(s_SWO_Buffer, bytesRead);
        //}
    }

} // namespace L0::Probe
