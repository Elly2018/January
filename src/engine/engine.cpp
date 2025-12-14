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
    //std::string path = cli.get<std::string>("p");
    std::printf("params size: %ld\n", cli.params().size());
    std::printf("flags size: %ld\n", cli.flags().size());
    std::printf("pos size: %ld\n", cli.pos_args().size());
}

void EngineLoop(){

}

void EngineDraw(){

}