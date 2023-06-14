#include "Body.h"
#include "./statements/Statement.h"
#include "./statements/FunctionCall.h"

namespace Source {
    bool ParseBody(
        Parser *parser,
        const std::shared_ptr<Lexer> &lexer,
        std::vector<std::shared_ptr<Expr>> &body,
        bool skipBraces
    ) {
        if (!skipBraces) {
            if (lexer->CurrentAtom.type != tok_open_brace) {
                parser->PrintError("(Body) Expected opening brace!");
                return false;
            }

            lexer->GetNextToken(); // Consume '{'
        }

        while (lexer->CurrentAtom.type != tok_close_brace && !lexer->IsAtEnd()) {
            if (auto statement = Source::ParseStatement(parser, lexer)) {
                body.push_back(statement);

                // The function call parser doesn't consume the semicolon because we allow function calls inline
                // ex. if (put())
                if (std::dynamic_pointer_cast<Statements::References::FunctionCall>(statement)) {
                    if (lexer->CurrentAtom.type != tok_semicolon && lexer->PeekNextToken().type != tok_semicolon) {
                        parser->PrintError("(Body) Expected semicolon after function call!");
                        return false;
                    }
                }

                if (lexer->CurrentAtom.type == tok_semicolon)
                    lexer->GetNextToken(); // Consume ';'

                // When we skip the braces, we only allow a single statement
                if (skipBraces)
                    break;

                continue;
            }

            return false;
        }

        if (skipBraces)
            return true;

        if (lexer->CurrentAtom.type != tok_close_brace) {
            parser->PrintError("(Body) Expected closing brace!");
            return false;
        }

        lexer->GetNextToken(); // Consume '}'

        return true;
    }
}