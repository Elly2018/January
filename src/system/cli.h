#pragma once
#include <memory>
#include <cmdparser.hpp>

void InitCMD(int argc, char** argv);
void DeInitCMD();
std::weak_ptr<cli::Parser> GetCMD();