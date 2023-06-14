#pragma once

#include <memory>
#include <vector>
#include "../structure/statements/Statement.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::Statement> ParseExpression(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
