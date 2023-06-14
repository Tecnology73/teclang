#pragma once

#include <memory>
#include <vector>
#include "../structure/Expr.h"

struct Parser;

class Lexer;

namespace Source {
    bool ParseBody(
        Parser *parser,
        const std::shared_ptr<Lexer> &lexer,
        std::vector<std::shared_ptr<Expr>> &body,
        bool skipBraces = false
    );
}
