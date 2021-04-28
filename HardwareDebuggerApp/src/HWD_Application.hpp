#pragma once

#include <Test/RVeips_ProbeTest.hpp>

#include "Core/Application.hpp"

namespace HWD {

    class HWD_Application : public Application {
    public:
        HWD_Application(int argc, char** argv);
        virtual ~HWD_Application();

        virtual void OnCreate() override;
        virtual void OnDestroy() override;

    private:
        std::unique_ptr<Test::RVeips_ProbeTest> m_RihardsTest;
    };

} // namespace HWD