#include "FunctionArg.h"
#include "../structure/types/ArrayTypeDef.h"
#include "../core/Context.h"

namespace Source {
    std::shared_ptr<TypeDef> ParseTypeDef(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.value.empty()) {
            parser->PrintError("Expected type!");
            return nullptr;
        }

        auto typeValue = lexer->CurrentAtom.value;
        auto nextToken = lexer->PeekNextToken();
        if (nextToken.type == tok_multiply) {
            typeValue += "*";
            lexer->GetNextToken(); // Consume the asterisk
        }

        auto typeDef = Context::Instance->GetDeclaredType(typeValue);
        if (!typeDef) {
            typeDef = std::make_shared<TypeDef>(typeValue, lexer->CurrentAtom.type == tok_multiply);
            Context::Instance->AddDeclaredType(typeDef);
        }

        while (lexer->GetNextToken().type == tok_open_bracket) {
            if (lexer->GetNextToken().type != tok_close_bracket) {
                parser->PrintError("Expected closing brace for array type!");
                return nullptr;
            }

            typeDef = std::make_shared<ArrayTypeDef>(typeDef);
            Context::Instance->GetDeclaredType("Array")->CloneOnto(typeDef);
        }

        return typeDef;
    }
}