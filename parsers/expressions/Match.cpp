#include "Match.h"
#include "../BinaryOp.h"
#include "../Body.h"
#include "../../structure/statements/Return.h"

namespace Source {
    /*
     * MatchExpr = "match" "(" Expr ")" "{" MatchCase* "}"
     *         | "match" Expr "{" MatchCase* "}"
     *         ;
     * MatchCase = Expr ("," Expr)* "=>" MatchBody ","
     *        | "_" "=>" MatchBody ","
     *        ;
     * MatchBody = "=>" "{" Body "}"
     *       | "=>" Body
     *       ;
     */
    std::shared_ptr<Expressions::Match> ParseMatch(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_match) {
            parser->PrintError("(Match) Expected \"match\" keyword!");
            return nullptr;
        }

        bool valueIsWrapped = lexer->GetNextToken().type == tok_open_paren;
        if (valueIsWrapped)
            lexer->GetNextToken(); // Consume '('

        // Parse value
        auto value = Source::ParseBinaryOp(parser, lexer);
        if (!value)
            return nullptr;

        if (valueIsWrapped) {
            if (lexer->CurrentAtom.type != tok_close_paren) {
                parser->PrintError("(Match) Expected closing parenthesis!");
                return nullptr;
            }

            lexer->GetNextToken(); // Consume ')'
        }

        if (lexer->CurrentAtom.type != tok_open_brace) {
            parser->PrintError("(Match) Expected opening brace!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume '{'

        // Parse cases
        std::vector<std::shared_ptr<Expressions::MatchCase>> cases;
        while (lexer->CurrentAtom.type != tok_close_brace) {
            // For the last case, the comma is optional.
            if (!cases.empty()) {
                if (lexer->CurrentAtom.type != tok_comma) {
                    parser->PrintError("(Match) Expected comma!");
                    return nullptr;
                }

                lexer->GetNextToken(); // Consume ','
            }

            // Just 1 value for the case block - expression => expression
            // Multiple values for the case block - expression, expression => expression
            std::vector<std::shared_ptr<Expr>> caseValues;
            bool isDefault = false;
            while (lexer->CurrentAtom.type != tok_arrow) {
                // If the value is an underscore, it's a default case.
                // The default case cannot have multiple values.
                if (lexer->CurrentAtom.type == tok_identifier && lexer->CurrentAtom.value == "_") {
                    isDefault = true;
                    lexer->GetNextToken(); // Consume '_'
                    break;
                }

                // 1 or more values that are comma separated
                caseValues.push_back(Source::ParseBinaryOp(parser, lexer));

                if (lexer->CurrentAtom.type != tok_comma)
                    break;

                lexer->GetNextToken(); // Consume ','
            }

            // Parse the case body
            if (lexer->CurrentAtom.type != tok_arrow) {
                parser->PrintError("(Match) Expected \"=>\"!");
                return nullptr;
            }

            lexer->GetNextToken(); // Consume '=>'

            // Parse body
            std::vector<std::shared_ptr<Expr>> caseBody;
            auto skipBraces = lexer->CurrentAtom.type != tok_open_brace;
            if (!Source::ParseBody(parser, lexer, caseBody, skipBraces))
                return nullptr;

            // Ensure the last statement of the body is a return statement
            auto lastStatement = caseBody.back();
            if (std::dynamic_pointer_cast<Statements::Return>(lastStatement) == nullptr) {
                caseBody.pop_back();
                caseBody.push_back(std::make_shared<Statements::Return>(lastStatement));
            }

            cases.push_back(std::make_shared<Expressions::MatchCase>(caseValues, caseBody, isDefault));
        }

        if (lexer->CurrentAtom.type != tok_close_brace) {
            parser->PrintError("(Match) Expected closing brace!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume '}'

        return std::make_shared<Expressions::Match>(value, cases);
    }
}
