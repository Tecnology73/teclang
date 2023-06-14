#pragma once

#include <memory>
#include "../../structure/statements/VariableDecl.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::VariableDecl> ParseVariableDecl(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
