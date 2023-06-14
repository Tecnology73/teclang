#include "Literal.h"
#include "../core/Parser.h"
#include "../structure/constants/BoolValue.h"
#include "../structure/constants/IntValue.h"
#include "../structure/constants/DoubleValue.h"
#include "../structure/constants/StringValue.h"

namespace Source {
    std::shared_ptr<Constants::ConstantValue> ParseLiteral(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        std::shared_ptr<Constants::ConstantValue> value = nullptr;
        switch (lexer->CurrentAtom.type) {
            case tok_bool_literal:
                value = std::make_shared<Constants::BoolValue>(lexer->CurrentAtom.value);
                break;
            case tok_int_literal:
                value = std::make_shared<Constants::IntValue>(lexer->CurrentAtom.value);
                break;
            case tok_double_literal:
                value = std::make_shared<Constants::DoubleValue>(lexer->CurrentAtom.value);
                break;
            case tok_string_literal: {
                value = std::make_shared<Constants::StringValue>(
                    lexer->CurrentAtom.value.substr(1, lexer->CurrentAtom.value.length() - 2)
                );
                break;
            }
            default:
                parser->PrintError("Unexpected token!");
                return nullptr;
        }

        lexer->GetNextToken(); // Consume literal

        return value;
    }
}