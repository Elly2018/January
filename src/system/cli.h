#pragma once
#include <memory>
#include <argh.h>

void InitCMD(int argc, char** argv);
argh::parser& GetCMD();