#pragma once

#include <memory>
#include "../../structure/statements/IfStatement.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::IfStatement> ParseIfStatement(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
