// [source]
#include "JLink.hpp"

namespace HWD::Probe {

    using namespace Driver::JLink_Types;

    void JLink::PrepareTarget() {
        HWDLOG_PROBE_TRACE("[JLink@{0}] Prepare target", fmt::ptr(this));
        ErrorCode ec;

        // Configure CoreSight
        ec = m_Driver->target_CoreSight_Configure("PerformTIFInit=1");
        if (ec != ErrorCode::OK) {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to configure CoreSight - {1}", fmt::ptr(this), ErrorCodeToString(ec));
        } else {
            HWDLOG_PROBE_TRACE("[JLink@{0}] CoreSight configured", fmt::ptr(this));
        }

        UpdateTargetInfo();
    }

    void JLink::UpdateTargetInfo() {
        uint32_t cpu_id                = m_Driver->target_Get_CPU_ID();
        DeviceCore core                = m_Driver->target_GetDeviceCore();
        int avail_sw_ram_breakpoints   = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_RAM);
        int avail_sw_flash_breakpoints = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_FLASH);
        int avail_hw_breakpoints       = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::HW);

        HWDLOG_PROBE_TRACE("[JLink@{0}] {1} Target info", fmt::ptr(this), GetModelName());
        HWDLOG_PROBE_TRACE("[JLink@{0}] [CPU]", fmt::ptr(this));
        HWDLOG_PROBE_TRACE("[JLink@{0}]  - Core:  {1}", fmt::ptr(this), DeviceCoreToString(core));
        HWDLOG_PROBE_TRACE("[JLink@{0}]  - CPUID: {1:#X}", fmt::ptr(this), cpu_id);
        HWDLOG_PROBE_TRACE("[JLink@{0}] [Available Breakpoints]", fmt::ptr(this));
        HWDLOG_PROBE_TRACE("[JLink@{0}]  - SW RAM:   {1}", fmt::ptr(this), avail_sw_ram_breakpoints);
        HWDLOG_PROBE_TRACE("[JLink@{0}]  - SW FLASH: {1}", fmt::ptr(this), avail_sw_flash_breakpoints);
        HWDLOG_PROBE_TRACE("[JLink@{0}]  - HW:       {1}", fmt::ptr(this), avail_hw_breakpoints);
    }

} // namespace HWD::Probe