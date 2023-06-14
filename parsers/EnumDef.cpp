#include "EnumDef.h"
#include "../core/Context.h"
#include "./BinaryOp.h"
#include "../structure/constants/IntValue.h"
#include "../structure/operations/BinaryOp.h"
#include "../structure/statements/references/VariableRef.h"

namespace Source {
    std::shared_ptr<EnumTypeDef> ParseEnum(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_enum) {
            parser->PrintError("(Enum) Expected \"enum\" keyword!");
            return nullptr;
        }

        if (lexer->GetNextToken().type != tok_identifier || lexer->CurrentAtom.value.empty()) {
            parser->PrintError("(Enum) Expected enum name!");
            return nullptr;
        }

        auto enumDef = std::make_shared<EnumTypeDef>(lexer->CurrentAtom.value);
        if (lexer->GetNextToken().type != tok_open_brace) {
            parser->PrintError("(Enum) Expected opening brace!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume '{'
        auto value = -1;
        while (lexer->CurrentAtom.type != tok_close_brace) {
            if (lexer->CurrentAtom.type != tok_identifier) {
                parser->PrintError("(Enum) Unexpected token!");
                return nullptr;
            }

            auto var = std::make_shared<Statements::VariableDecl>(
                Context::Instance->GetDeclaredType("int"),
                lexer->CurrentAtom.value
            );

            if (lexer->GetNextToken().type == tok_assign) {
                if (lexer->GetNextToken().type != tok_int_literal) {
                    parser->PrintError("Enum value can only be an int literal!");
                    return nullptr;
                }

                value = strtol(lexer->CurrentAtom.value.c_str(), nullptr, 10);
                lexer->GetNextToken(); // Consume value
            } else
                value += 1;

            var->ConstructorArgs.push_back(std::make_shared<Constants::IntValue>(value));
            enumDef->AddField(var);

            if (lexer->CurrentAtom.type == tok_comma) {
                lexer->GetNextToken(); // Consume ','
            }
        }

        lexer->GetNextToken(); // Consume '}'

        return enumDef;
    }
}