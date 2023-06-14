#include "Return.h"
#include "../BinaryOp.h"

namespace Source {
    std::shared_ptr<Statements::Return> ParseReturn(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_return) {
            parser->PrintError("Expected return statement!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume 'return'

        std::shared_ptr<Expr> statement = Source::ParseBinaryOp(parser, lexer);
        if (lexer->CurrentAtom.type != tok_semicolon) {
            parser->PrintError("Expected semicolon at end of expression!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume ';'

        return std::make_shared<Statements::Return>(statement);
    }
}
