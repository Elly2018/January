#include "system/cli.h"
#include "system/window.h"

int main(int argc, char** argv){
    InitCMD(argc, argv);
    JInit();
    JMainloop();
    JDeInit();
}