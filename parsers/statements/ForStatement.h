#pragma once

#include <memory>
#include "../../structure/statements/ForStatement.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::ForStatement> ParseForStatement(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
