#include "engine.h"
#include <memory>
#include "utility.h"
#include "../system/cli.h"
#include "gui/mainframe.h"

static JEngine jengine = JEngine();

#pragma region Private Utility

#pragma endregion

void SaveAppConfig(std::weak_ptr<AppConfig> target){
    fs::path home = get_home_directory();
}

std::shared_ptr<AppConfig> LoadAppConfig(){
    fs::path home = get_home_directory();
    home = home.append("january");
    if(!fs::exists(home)){
        fs::create_directory(home);
    }
    home = home.append("config.json");
    if(!fs::exists(home)){
        fs::create_directory(home);
    }

    return std::make_shared<AppConfig>();
}

std::shared_ptr<AppContext> GenerateAppContext(){
    AppContext ctx = AppContext();
    auto cli = GetCMD();
    if(cli.params().contains("p")){
        ctx.project_path = cli.params().at("p");
    }
    else if (cli.params().contains("path")){
        ctx.project_path = cli.params().at("path");
    }
    return std::make_shared<AppContext>(ctx);
}

void EngineInit(){
    jengine.config = LoadAppConfig();
    jengine.context = GenerateAppContext();

    // We check if page is first time fire
    JPageType page = jengine.config.get()->j_last_open;
    bool firstFire = UIPageFirstTimeFire(jengine, page);
    if(firstFire){
        std::vector<JViewType> views = GetDefaultViewByPage(page);
        UIGenerateViews(jengine, views);
    }else{
        std::string pageName = jengine.config.get()->j_page_name;
        UILoadPageFromDisk(jengine, pageName);
    }
}

void EngineDeInit(){
    jengine.config.reset();
    for(auto view : jengine.context.get()->views){
        view.reset();
    }
    jengine.context.get()->views.clear();
    jengine.context.reset();
}

void EngineUpdate(JWindow& win){

}

void EngineDraw(JWindow& win){
    UIDraw(win, jengine);
}