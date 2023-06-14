#pragma once

#include <memory>
#include <vector>
#include "../structure/constants/Constants.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Constants::ConstantValue> ParseLiteral(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
