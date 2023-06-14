#include "BinaryOp.h"
#include "Identifier.h"
#include "../structure/operations/BinaryOp.h"

namespace Source {
    int getPrecedence(Token token) {
        if (token == tok_plusplus)
            return 109;
        if (token == tok_minusminus)
            return 108;

        if (token == tok_exclamation)
            return 99;

        if (token == tok_percent)
            return 89;
        if (token == tok_multiply)
            return 88;
        if (token == tok_divide)
            return 87;
        if (token == tok_plus)
            return 86;
        if (token == tok_minus)
            return 85;

        if (token == tok_less_than)
            return 74;
        if (token == tok_lt_equal)
            return 73;
        if (token == tok_greater_than)
            return 72;
        if (token == tok_gt_equal)
            return 71;

        if (token == tok_equal)
            return 69;
        if (token == tok_not_equal)
            return 68;

        if (token == tok_and)
            return 59;
        if (token == tok_or)
            return 58;

        if (token == tok_plus_equal)
            return 49;
        if (token == tok_minus_equal)
            return 48;
        if (token == tok_multi_equal)
            return 47;
        if (token == tok_divide_equal)
            return 46;
        if (token == tok_mod_equal)
            return 45;
        if (token == tok_assign)
            return 44;

        if (token == tok_ampersand)
            return 39;
        if (token == tok_caret)
            return 38;
        if (token == tok_pipe)
            return 36;

        if (token == tok_question_mark)
            return 29;
        if (token == tok_colon)
            return 28;
        if (token == tok_spread)
            return 27;

        return -1;
    }

    std::shared_ptr<Expr> parseBinaryOpRhs(
        Parser *parser,
        const std::shared_ptr<Lexer> &lexer,
        int precedence,
        std::shared_ptr<Expr> lhs
    ) {
        while (true) {
            int tokenPrecedence = getPrecedence(lexer->CurrentAtom.type);

            if (tokenPrecedence < precedence)
                return lhs;

            Token opCode = lexer->CurrentAtom.type;

            // Little hack to get ++ and -- to parse as intended
            // When ParseIdentifier is called on a ++ or -- token, it returns a Constant::IntValue(1)
            // We override the operand in this case because `num++` is just short for `num += 1`
            if (opCode == tok_plusplus) {
                opCode = tok_plus_equal;
            } else if (opCode == tok_minusminus) {
                opCode = tok_minus_equal;
            } else
                lexer->GetNextToken();

            auto rhs = Source::ParseIdentifier(parser, lexer);
            if (!rhs)
                return nullptr;

            int nextPrecedence = getPrecedence(lexer->CurrentAtom.type);

            if ((tokenPrecedence >= 108 && tokenPrecedence <= 109) &&
                (nextPrecedence >= 44 && nextPrecedence <= 49)) {
                parser->PrintError("(BinaryOp) Left hand side is not assignable!");
                return nullptr;
            }

            // Right-to-left for = += -= *= /= %=
            if ((tokenPrecedence >= 45 && tokenPrecedence <= 49) && (nextPrecedence >= 44 && nextPrecedence <= 49))
                tokenPrecedence = 0;

            // If both sides are assignment, then parse right-to-left
            if (tokenPrecedence == 44 && nextPrecedence == 44)
                tokenPrecedence = 0;

            if (tokenPrecedence <= nextPrecedence) {
                rhs = parseBinaryOpRhs(parser, lexer, tokenPrecedence + 1, rhs);
                if (!rhs)
                    return nullptr;
            }

            lhs = std::make_shared<Operations::BinaryOp>(opCode, lhs, rhs);
        }
    }

    std::shared_ptr<Expr> ParseBinaryOp(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        auto lhs = Source::ParseIdentifier(parser, lexer);
        if (!lhs)
            return nullptr;

        return parseBinaryOpRhs(parser, lexer, 0, lhs);
    }
}
