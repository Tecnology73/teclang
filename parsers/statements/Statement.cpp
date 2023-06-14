#include "Statement.h"
#include "Return.h"
#include "VariableDecl.h"
#include "FunctionCall.h"
#include "../BinaryOp.h"
#include "../Identifier.h"
#include "IfStatement.h"
#include "ForStatement.h"
#include "Break.h"
#include "Continue.h"

namespace Source {
    std::shared_ptr<Expr> parseIdentifier(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        switch (lexer->PeekNextToken().type) {
            case tok_identifier:    // type identifier
                return Source::ParseVariableDecl(parser, lexer);
            case tok_open_bracket: //  type[] identifier
                // If there is nothing between the brackets, we're declaring a variable.
                if (lexer->PeekNextToken(2).type == tok_close_bracket)
                    return Source::ParseVariableDecl(parser, lexer);

                break;
            case tok_open_paren:    // identifier(
                return Source::ParseFunctionCall(parser, lexer);
            case tok_period:        // identifier.
                // Nested variable usage (user.name = "something")
                break;
        }

        return Source::ParseBinaryOp(parser, lexer);
    }

    std::shared_ptr<Expr> ParseStatement(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        switch (lexer->CurrentAtom.type) {
            case tok_bool:
            case tok_int:
            case tok_double:
            case tok_string:
            case tok_identifier:
                return parseIdentifier(parser, lexer);
            case tok_return:
                return Source::ParseReturn(parser, lexer);
            case tok_if:
                return Source::ParseIfStatement(parser, lexer);
            case tok_for:
                return Source::ParseForStatement(parser, lexer);
            case tok_break:
                return Source::ParseBreak(parser, lexer);
            case tok_continue:
                return Source::ParseContinue(parser, lexer);
            case tok_match:
                break;
        }

        return Source::ParseIdentifier(parser, lexer);
    }
}