#include "VariableRef.h"
#include "../BinaryOp.h"
#include "../../structure/statements/references/ArrayRef.h"

namespace Source {
    std::shared_ptr<Statements::References::VariableRef>
    ParseVariableRef(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_identifier) {
            parser->PrintError("Expected variable name!");
            return nullptr;
        }

        std::string varName = lexer->CurrentAtom.value;

        lexer->GetNextToken();
        std::shared_ptr<Statements::References::Reference> nextKey;
        if (lexer->CurrentAtom.type == tok_open_bracket) {
            lexer->GetNextToken(); // Consume '['
            nextKey = std::make_shared<Statements::References::ArrayRef>(Source::ParseBinaryOp(parser, lexer));

            if (lexer->CurrentAtom.type != tok_close_bracket) {
                parser->PrintError("Expected closing bracket!");
                return nullptr;
            }

            lexer->GetNextToken();
        }

        while (lexer->CurrentAtom.type == tok_period) {
            if (lexer->GetNextToken().type != tok_identifier) {
                parser->PrintError("Expected property name!");
                return nullptr;
            }

            auto v = ParseVariableRef(parser, lexer);
            if (!v) return nullptr;
            if (nextKey)
                nextKey->NextKey = v;
            else
                nextKey = v;
        }

        return std::make_shared<Statements::References::VariableRef>(varName, nextKey);
    }
}
