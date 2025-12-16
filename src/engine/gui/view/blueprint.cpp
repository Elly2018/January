#include "blueprint.h"
#include <spdlog/spdlog.h>

JViewBlueprint::JViewBlueprint(const char* _title, int32_t _type) : JViewBase(_title, _type) {

}

JViewBlueprint::~JViewBlueprint(){

}

void JViewBlueprint::Init(){
    spdlog::info("Loaded View: Blueprint");
}
void JViewBlueprint::Update(){

}
void JViewBlueprint::Draw(){

}
void JViewBlueprint::DeInit(){

}