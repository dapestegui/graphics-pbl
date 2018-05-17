//
//  menu.cpp
//  CGV
//
//  Created by Elias Piovani on 14/05/2018.
//  Copyright © 2018 Elias Piovani. All rights reserved.
//
#ifndef IMGUI_API
#include "imgui.h"
#endif
#include "menu.hpp"

int ShowExampleMenuFile()
{
    int menu = 0;
    ImGui::MenuItem("(dummy menu)", NULL, false, false);
    if (ImGui::MenuItem("Open PGN", "Ctrl+O")) {
        {
            menu = 10;
        }
    }
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        
        ImGui::EndMenu();
    }
//    ImGui::Separator();
//    if (ImGui::BeginMenu("Options"))
//    {
//        static bool enabled = true;
//        ImGui::MenuItem("Enabled", "", &enabled);
//        ImGui::BeginChild("child", ImVec2(0, 60), true);
//        for (int i = 0; i < 10; i++)
//            ImGui::Text("Scrolling Text %d", i);
//        ImGui::EndChild();
//        static float f = 0.5f;
//        static int n = 0;
//        static bool b = true;
//        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
//        ImGui::InputFloat("Input", &f, 0.1f);
//        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
//        ImGui::Checkbox("Check", &b);
//        ImGui::EndMenu();
//    }
//    if (ImGui::BeginMenu("Colors"))
//    {
//        float sz = ImGui::GetTextLineHeight();
//        for (int i = 0; i < ImGuiCol_COUNT; i++)
//        {
//            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
//            ImVec2 p = ImGui::GetCursorScreenPos();
//            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x+sz, p.y+sz), ImGui::GetColorU32((ImGuiCol)i));
//            ImGui::Dummy(ImVec2(sz, sz));
//            ImGui::SameLine();
//            ImGui::MenuItem(name);
//        }
//        ImGui::EndMenu();
//    }
//    if (ImGui::BeginMenu("Disabled", false)) // Disabled
//    {
//        IM_ASSERT(0);
//    }
//    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {
        menu = 155;
    }
    
    return menu;
}
