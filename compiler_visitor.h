#pragma once

#include "compiler_result.h"

#include "antlr4-runtime.h"

CompilationResults generate_byte_code(antlr4::tree::ParseTree* tree);