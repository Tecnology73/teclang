#include "IfStatement.h"
#include "./Statement.h"
#include "../Body.h"
#include "../BinaryOp.h"

namespace Source {
    std::shared_ptr<Statements::IfStatement> parseElseStatement(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        // Parse `else if`
        if (lexer->CurrentAtom.type == tok_if)
            return ParseIfStatement(parser, lexer);

        if (lexer->CurrentAtom.type != tok_open_brace) {
            parser->PrintError("(Else) Expected opening brace!");
            return nullptr;
        }

        // Parse body
        std::vector<std::shared_ptr<Expr>> body;
        if (!Source::ParseBody(parser, lexer, body))
            return nullptr;

        // Parse else
        std::shared_ptr<Statements::IfStatement> elseStatement;
        if (lexer->CurrentAtom.type == tok_else) {
            lexer->GetNextToken(); // Consume 'else'
            elseStatement = parseElseStatement(parser, lexer);
        }

        return std::make_shared<Statements::IfStatement>(nullptr, body, elseStatement);
    }

    /*
     * ElseStatement = "else" "{" Body "}"
     *              | "else" IfStatement
     *              ;
     * IfStatement = "if" "(" Expr ")" "{" Body "}" ElseStatement?
     *           | "if" Expr "{" Body "}" ElseStatement?
     *           ;
     */
    std::shared_ptr<Statements::IfStatement> ParseIfStatement(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_if) {
            parser->PrintError("(If) Expected \"if\" keyword!");
            return nullptr;
        }

        if (lexer->GetNextToken().type != tok_open_paren) {
            parser->PrintError("(If) Expected opening parenthesis!");
            return nullptr;
        }
        lexer->GetNextToken(); // Consume '('

        // Parse condition
        auto condition = Source::ParseBinaryOp(parser, lexer);
        if (!condition)
            return nullptr;

        if (lexer->CurrentAtom.type != tok_close_paren) {
            parser->PrintError("(If) Expected closing parenthesis!");
            return nullptr;
        }

        // Parse body
        std::vector<std::shared_ptr<Expr>> body;
        // Add just the 1 statement on the following line if there are no braces
        auto skipBraces = lexer->GetNextToken().type != tok_open_brace;
        if (!Source::ParseBody(parser, lexer, body, skipBraces))
            return nullptr;

        // Parse else
        std::shared_ptr<Statements::IfStatement> elseStatement;
        if (lexer->CurrentAtom.type == tok_else) {
            lexer->GetNextToken(); // Consume 'else'
            elseStatement = parseElseStatement(parser, lexer);
        }

        return std::make_shared<Statements::IfStatement>(condition, body, elseStatement);
    }
}
