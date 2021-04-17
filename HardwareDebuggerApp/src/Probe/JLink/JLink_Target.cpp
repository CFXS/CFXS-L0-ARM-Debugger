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
        HWDLOG_PROBE_TRACE("[JLink@{0}] UpdateTargetInfo:", fmt::ptr(this));
        ErrorCode ec;

        uint32_t cpu_id                = m_Driver->target_Get_CPU_ID();
        DeviceCore core                = m_Driver->target_GetDeviceCore();
        int avail_sw_ram_breakpoints   = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_RAM);
        int avail_sw_flash_breakpoints = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_FLASH);
        int avail_hw_breakpoints       = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::HW);
        uint32_t mtb_address;     // MTB Trace module register address (0 = not supported)
        uint32_t etm_address = 0; // ETM Trace module register address (0 = not supported)
        uint32_t etb_address = 0; // ETB Trace module register address (0 = not supported)

        ec = m_Driver->target_GetDebugInfo(DebugInfoIndex::MTB_ADDR, &mtb_address); // MTB
        if (ec != ErrorCode::OK) {
            HWDLOG_PROBE_WARN("Failed to get MTB address - {0}", ec);
        }

        ec = m_Driver->target_GetDebugInfo(DebugInfoIndex::ETM_ADDR, &etm_address); // ETM
        if (ec != ErrorCode::OK) {
            HWDLOG_PROBE_WARN("Failed to get ETM address - {0}", ec);
        }

        ec = m_Driver->target_GetDebugInfo(DebugInfoIndex::ETB_ADDR, &etb_address); // ETB
        if (ec != ErrorCode::OK) {
            HWDLOG_PROBE_WARN("Failed to get ETB address - {0}", ec);
        }

        HWDLOG_PROBE_TRACE("{0} Target info", GetModelName());
        HWDLOG_PROBE_TRACE("[CPU]");
        HWDLOG_PROBE_TRACE(" - Core:  {0}", core);
        HWDLOG_PROBE_TRACE(" - CPUID: {0:#X}", cpu_id);
        HWDLOG_PROBE_TRACE(" - MTB:   {0}", mtb_address ? fmt::format("@ {0:#X}", mtb_address) : "Not supported");
        HWDLOG_PROBE_TRACE(" - ETM:   {0}", etm_address ? fmt::format("@ {0:#X}", etm_address) : "Not supported");
        HWDLOG_PROBE_TRACE(" - ETB:   {0}", etb_address ? fmt::format("@ {0:#X}", etb_address) : "Not supported");
        HWDLOG_PROBE_TRACE("[Available Breakpoints]");
        HWDLOG_PROBE_TRACE(" - SW RAM:   {0}", avail_sw_ram_breakpoints);
        HWDLOG_PROBE_TRACE(" - SW FLASH: {0}", avail_sw_flash_breakpoints);
        HWDLOG_PROBE_TRACE(" - HW:       {0}", avail_hw_breakpoints);
    }

} // namespace HWD::Probe