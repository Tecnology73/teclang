#pragma once

#include <memory>
#include "../../structure/expressions/Match.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Expressions::Match> ParseMatch(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
