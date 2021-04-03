#pragma once

#include "Core/Application.hpp"
#include <Test/RVeips_ProbeTest.hpp>

namespace HWD {

    class HWD_Application : public Application {
    public:
        HWD_Application(int argc, char** argv);
        virtual ~HWD_Application();

        virtual void OnCreate() override;
        virtual void OnDestroy() override;
        virtual void OnUpdate() override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent() override;

    private:
        std::unique_ptr<Test::RVeips_ProbeTest> m_RihardsTest;
    };

} // namespace HWD