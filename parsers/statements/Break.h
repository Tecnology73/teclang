#pragma once

#include <memory>
#include "../../structure/statements/Break.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::Break> ParseBreak(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_break) {
            parser->PrintError("(Break) Expected \"break\" keyword!");
            return nullptr;
        }

        if (lexer->GetNextToken().type != tok_semicolon) {
            parser->PrintError("(Break) Expected semicolon!");
            return nullptr;
        }

        return std::make_shared<Statements::Break>();
    }
}
