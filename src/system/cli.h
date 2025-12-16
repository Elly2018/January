#pragma once
#include <memory>
#include <optional>
#include <argh.h>

// Store CMD into memory
void InitCMD(int argc, char** argv);
// Get the parser from memory
argh::parser& GetCMD();

bool GetCMDFlag(std::string title);

bool GetCMDFlag(std::string title, std::string second);

std::optional<std::string> GetCMDParam(std::string title);

std::optional<std::string> GetCMDParam(std::string title, std::string second);