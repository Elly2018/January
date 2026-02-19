/**
MIT License

Copyright (c) 2026 ZhuElly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "blueprint.h"
#include <spdlog/spdlog.h>
#include <imgui.h>
#include "../../engine.h"

// Global engine access point
extern January::Engine::JEngine jengine;

January::Engine::View::JViewBlueprint::JViewBlueprint(const char* _title, int32_t _type) : JViewBase(_title, _type) {

}

January::Engine::View::JViewBlueprint::~JViewBlueprint(){

}

void January::Engine::View::JViewBlueprint::Init(){
    spdlog::info("Loaded View: Blueprint");
}
void January::Engine::View::JViewBlueprint::Update(){

}
void January::Engine::View::JViewBlueprint::Draw(){
    ImGui::Begin(title);
    
    ImGui::End();
}
void January::Engine::View::JViewBlueprint::DeInit(){

}