#include "system/cli.h"
#include "system/window.h"

int main(int argc, char** argv){
    InitCMD(argc, argv);
    auto win = std::move(JInit());
    JMainloop(win);
    JDeInit(win);
    win.reset();
    DeInitCMD();
}