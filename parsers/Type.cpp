#include "Type.h"
#include "statements/VariableDecl.h"
#include "../core/Context.h"

namespace Source {
    std::shared_ptr<TypeDef> ParseType(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_type) {
            parser->PrintError("Expected \"type\" keyword!");
            return nullptr;
        }

        if (lexer->GetNextToken().type != tok_identifier || lexer->CurrentAtom.value.empty()) {
            parser->PrintError("Expected type name!");
            return nullptr;
        }

        auto typeDef = Context::Instance->GetDeclaredType(lexer->CurrentAtom.value);
        if (typeDef && typeDef->IsDeclared) {
            parser->PrintError("This type has already been declared!");
            return nullptr;
        } else if (!typeDef)
            typeDef = std::make_shared<TypeDef>(lexer->CurrentAtom.value);

        while (lexer->GetNextToken().type != tok_close_brace) {
            if (lexer->CurrentAtom.type == tok_open_brace)
                continue;

            if (auto field = Source::ParseVariableDecl(parser, lexer)) {
                if (!typeDef->AddField(field)) {
                    parser->PrintError("Field has already been defined!");
                    return nullptr;
                }

                continue;
            }

            return nullptr;
        }

        lexer->GetNextToken(); // Consume '}'

        return typeDef;
    }
}