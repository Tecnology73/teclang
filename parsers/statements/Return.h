#pragma once

#include <memory>
#include "../../structure/statements/Return.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::Return> ParseReturn(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
