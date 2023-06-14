#pragma once

#include <memory>
#include "../structure/Function.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Function> ParseFunction(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
