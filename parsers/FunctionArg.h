#pragma once

#include <memory>
#include "../structure/FunctionArg.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<FunctionArg> ParseFunctionArg(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
