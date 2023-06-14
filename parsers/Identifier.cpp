#include "Identifier.h"
#include "Literal.h"
#include "BinaryOp.h"
#include "../structure/constants/IntValue.h"
#include "statements/VariableRef.h"
#include "statements/FunctionCall.h"
#include "expressions/Match.h"

namespace Source {
    std::shared_ptr<Expr> ParseIdentifier(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        switch (lexer->CurrentAtom.type) {
            case tok_bool_literal:
            case tok_int_literal:
            case tok_double_literal:
            case tok_string_literal:
                return Source::ParseLiteral(parser, lexer);
            case tok_identifier:
                if (lexer->PeekNextToken().type == tok_open_paren)
                    return Source::ParseFunctionCall(parser, lexer);

                return Source::ParseVariableRef(parser, lexer);
            case tok_plusplus:
            case tok_minusminus:
                lexer->GetNextToken(); // Consume '++'/'--'
                return std::make_shared<Constants::IntValue>(1);
            case tok_open_paren: {
                lexer->GetNextToken(); // Consume '('
                if (auto statement = Source::ParseBinaryOp(parser, lexer)) {
                    if (lexer->CurrentAtom.type != tok_open_brace)
                        lexer->GetNextToken(); // Consume ')' ??

                    return statement;
                }

                return nullptr;
            }
            case tok_match:
                return Source::ParseMatch(parser, lexer);
        }

        parser->PrintError("Unexpected token!");
        return nullptr;
    }
}