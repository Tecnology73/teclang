#pragma once

#include <memory>
#include "../../structure/statements/references/VariableRef.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::References::VariableRef>
    ParseVariableRef(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
