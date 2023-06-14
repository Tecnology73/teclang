#pragma once

#include <memory>
#include "../structure/TypeDef.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<TypeDef> ParseType(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
