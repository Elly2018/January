#include "engine.h"
#include "../system/cli.h"
#include <memory>

void SaveAppConfig(std::weak_ptr<AppConfig> target){

}

std::shared_ptr<AppConfig> LoadAppConfig(){
    return std::make_shared<AppConfig>();
}

std::shared_ptr<AppContext> GenerateAppContext(){
    AppContext ctx = AppContext();

    return std::make_shared<AppContext>(ctx);
}

void EngineInit(){
    auto cli = GetCMD();
    auto clis = cli.lock();
    auto cliref = clis.get();
    std::string path = cliref->get<std::string>("p");
    std::printf("Path: %s\n", path.c_str());
}

void EngineLoop(){

}

void EngineDraw(){

}