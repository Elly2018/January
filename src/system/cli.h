#pragma once
#include <memory>
#include <argh.h>

// Store CMD into memory
void InitCMD(int argc, char** argv);
// Get the parser from memory
argh::parser& GetCMD();