#include "FunctionCall.h"
#include "../BinaryOp.h"

namespace Source {
    std::shared_ptr<Statements::References::FunctionCall>
    ParseFunctionCall(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_identifier) {
            parser->PrintError("(FunctionCall) Expected function name!");
            return nullptr;
        }

        std::string functionName = lexer->CurrentAtom.value;
        std::vector<std::shared_ptr<Expr>> arguments;

        if (lexer->GetNextToken().type != tok_open_paren) {
            parser->PrintError("(FunctionCall) Expected opening parenthesis for a function call!");
            return nullptr;
        }

        // Parse arguments
        lexer->GetNextToken();
        while (lexer->CurrentAtom.type != tok_close_paren && !lexer->IsAtEnd()) {
            if (auto statement = Source::ParseBinaryOp(parser, lexer)) {
                arguments.push_back(statement);

                if (lexer->CurrentAtom.type == tok_comma)
                    lexer->GetNextToken();

                continue;
            }

            return nullptr;
        }

        if (lexer->CurrentAtom.type != tok_close_paren) {
            parser->PrintError("(FunctionCall) Expected closing parenthesis!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume ')'

        /*if (lexer->GetNextToken().type != tok_semicolon) {
            parser->PrintError("(FunctionCall) Expected semicolon!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume ';'*/

        return std::make_shared<Statements::References::FunctionCall>(functionName, arguments);
    }
}
