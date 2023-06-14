#include "ForStatement.h"
#include "./Statement.h"
#include "../TypeDef.h"
#include "../Body.h"
#include "VariableDecl.h"
#include "../BinaryOp.h"

namespace Source {
    std::shared_ptr<Statements::ForStatement> ParseForStatement(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_for) {
            parser->PrintError("(For) Expected \"for\" keyword!");
            return nullptr;
        }

        if (lexer->GetNextToken().type != tok_open_paren) {
            parser->PrintError("(For) Expected opening parenthesis!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume '('
        auto typeDef = Source::ParseTypeDef(parser, lexer);

        if (lexer->CurrentAtom.type != tok_identifier) {
            parser->PrintError("(For) Expected variable name!");
            return nullptr;
        }

        auto head = std::make_shared<Statements::VariableDecl>(typeDef, lexer->CurrentAtom.value);

        if (lexer->GetNextToken().type != tok_in) {
            parser->PrintError("(For) Expected \"in\" keyword!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume 'in'
        auto value = Source::ParseBinaryOp(parser, lexer);

        // Parse tail. e.g i += 2
        std::shared_ptr<Expr> tail;
        if (lexer->CurrentAtom.type == tok_semicolon) {
            lexer->GetNextToken(); // Consume ';'

            tail = Source::ParseStatement(parser, lexer);
            if (!tail)
                return nullptr;
        }

        if (lexer->CurrentAtom.type == tok_close_paren)
            lexer->GetNextToken(); // Consume ')'

        // Parse body
        std::vector<std::shared_ptr<Expr>> body;
        // Add just the 1 statement on the following line if there are no braces
        auto skipBraces = lexer->CurrentAtom.type != tok_open_brace;
        if (!Source::ParseBody(parser, lexer, body, skipBraces))
            return nullptr;

        return std::make_shared<Statements::ForStatement>(
            head,
            std::dynamic_pointer_cast<Operations::BinaryOp>(value),
            tail,
            body
        );
    }
}
