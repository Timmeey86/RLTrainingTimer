#include <pch.h>

#include "imgui_disable.h"
#include "imgui.h"
#include "imgui_internal.h"

ImGui::Disable::Disable(const bool should_disable_, float alpha) : should_disable(should_disable_)
{
    if (should_disable)
    {
        PushItemFlag(ImGuiItemFlags_Disabled, true);
        PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    }
}

ImGui::Disable::~Disable()
{
    if (should_disable)
    {
        PopItemFlag();
        PopStyleVar();
    }
}