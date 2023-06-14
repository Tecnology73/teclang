#pragma once

#include <memory>
#include <vector>
#include "../structure/Expr.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Expr> ParseIdentifier(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
