#include "system/window.h"

int main(){
    auto win = std::move(JInit());
    JDemo(win);
    JDeInit(win);
    win.reset();
}