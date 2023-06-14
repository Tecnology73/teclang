#pragma once

#include <memory>
#include "../structure/Expr.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Expr> ParseBinaryOp(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
