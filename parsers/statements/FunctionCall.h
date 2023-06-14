#pragma once

#include <memory>
#include "../../structure/statements/references/FunctionCall.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::References::FunctionCall>
    ParseFunctionCall(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
