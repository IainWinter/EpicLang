#pragma once

#include "compiler_result.h"

#include <string_view>

CompilationResults compile(std::string_view text, const std::vector<ExternalFunction>& external_functions);