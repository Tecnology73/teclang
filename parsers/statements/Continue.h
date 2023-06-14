#pragma once

#include <memory>
#include "../../structure/statements/Continue.h"

struct Parser;

class Lexer;

namespace Source {
    std::shared_ptr<Statements::Continue> ParseContinue(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_continue) {
            parser->PrintError("(Continue) Expected \"continue\" keyword!");
            return nullptr;
        }

        if (lexer->GetNextToken().type != tok_semicolon) {
            parser->PrintError("(Continue) Expected semicolon!");
            return nullptr;
        }

        return std::make_shared<Statements::Continue>();
    }
}
