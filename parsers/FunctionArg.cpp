#include "FunctionArg.h"
#include "../core/Parser.h"
#include "TypeDef.h"
#include "Literal.h"

namespace Source {
    std::shared_ptr<FunctionArg> ParseFunctionArg(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        auto typeDef = Source::ParseTypeDef(parser, lexer);
        if (!typeDef)
            return nullptr;

        if (lexer->CurrentAtom.type != tok_identifier) {
            parser->PrintError("Expected argument name!");
            return nullptr;
        }

        std::string argName = lexer->CurrentAtom.value;
        std::shared_ptr<Constants::ConstantValue> value = nullptr;

        // Argument default value
        if (lexer->GetNextToken().type == tok_assign) {
            lexer->GetNextToken(); // Consume '='
            value = Source::ParseLiteral(parser, lexer);
        }

        return std::make_shared<FunctionArg>(typeDef, argName, value);
    }
}