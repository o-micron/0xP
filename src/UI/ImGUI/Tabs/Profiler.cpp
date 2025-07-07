/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#include <UI/ImGUI/Tabs/Profiler.h>

#include <Utilities/XPProfiler.h>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wall"
    #pragma clang diagnostic ignored "-Weverything"
#endif
#include <implot.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

XPProfilerUITab::XPProfilerUITab(XPRegistry* const registry)
  : XPUITab(registry)
{
}

void
XPProfilerUITab::renderView(XPScene* scene, uint32_t& openViewsMask, float deltaTime)
{
    XP_UNUSED(scene)
    XP_UNUSED(openViewsMask)
    XP_UNUSED(deltaTime)

    auto&  timelines = XPProfiler::instance().getTimelines();
    ImVec2 available = ImGui::GetContentRegionAvail();
    ImVec2 graphSize(available.x, (available.y / timelines.size()) - (timelines.size() - 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    {
        if (ImPlot::BeginPlot("##Profiler", ImGui::GetContentRegionAvail(), 0)) {
            ImPlot::SetupAxes("step", "Milliseconds");
            ImPlot::SetupAxesLimits(0, XP_PROFILER_TIMELINE_WIDTH, 0, 100);
            const auto& xAxis = XPProfiler::instance().getXAxis();
            for (const auto& timeline : timelines) {
                ImPlot::PlotLine(timeline.first.function,
                                 xAxis.data(),
                                 timeline.second.values.data(),
                                 static_cast<int>(xAxis.size()),
                                 ImPlotLineFlags_None,
                                 0,
                                 sizeof(uint32_t));
            }
            ImPlot::EndPlot();
        }
    }
    ImGui::PopStyleVar();
}

void
XPProfilerUITab::onEvent(XPScene* scene, SDL_Event* event){ XP_UNUSED(scene) XP_UNUSED(event) }

uint32_t XPProfilerUITab::getViewMask() const
{
    return XPUiViewMaskProfiler;
}

ImGuiWindowFlags
XPProfilerUITab::getWindowFlags() const
{
    return ImGuiWindowFlags_NoCollapse;
}

const char*
XPProfilerUITab::getTitle() const
{
    return XP_TAB_PROFILER_TITLE;
}

ImVec2
XPProfilerUITab::getWindowPadding() const
{
    return ImVec2(0.0f, 0.0f);
}
