#pragma once

#include <memory>
#include "../structure/types/EnumTypeDef.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<EnumTypeDef> ParseEnum(Parser *parser, const std::shared_ptr<Lexer> &lexer);
}
