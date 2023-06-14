#include "Expression.h"
#include "../core/Parser.h"
#include "Literal.h"

namespace Source {
    std::shared_ptr<Statements::Statement> ParseExpression(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        switch (lexer->CurrentAtom.type) {
            case tok_bool_literal:
            case tok_int_literal:
            case tok_double_literal:
            case tok_string_literal:
                return Source::ParseLiteral(parser, lexer);
        }

        parser->PrintError("Unknown expression!");
        return nullptr;
    }
}