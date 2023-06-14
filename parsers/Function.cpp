#include "Function.h"
#include "FunctionArg.h"
#include "TypeDef.h"
#include "Body.h"
#include "../structure/types/ArrayTypeDef.h"

namespace Source {
    std::shared_ptr<Function> ParseFunction(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        if (lexer->CurrentAtom.type != tok_func) {
            parser->PrintError("Expected \"func\" keyword!");
            return nullptr;
        }

        std::shared_ptr<TypeDef> ownerType = nullptr;
        if (lexer->GetNextToken().type == tok_less_than) {
            lexer->GetNextToken(); // Consume '<'
            ownerType = Source::ParseTypeDef(parser, lexer);

            if (std::dynamic_pointer_cast<ArrayTypeDef>(ownerType)) {
                parser->PrintError("Cannot add member function to array of type!");
                return nullptr;
            }

            if (lexer->CurrentAtom.type != tok_greater_than) {
                parser->PrintError("Expected \">\" after type!");
                return nullptr;
            }

            lexer->GetNextToken(); // Consume '>'
        }

        if (lexer->CurrentAtom.type != tok_identifier) {
            parser->PrintError("Expected function name!");
            return nullptr;
        }

        // Function name
        std::string funcName = lexer->CurrentAtom.value;

        if (lexer->GetNextToken().type != tok_open_paren) {
            parser->PrintError("Expected opening parenthesis!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume '('

        // Parse arguments
        std::vector<std::shared_ptr<FunctionArg>> args;
        while (lexer->CurrentAtom.type != tok_close_paren) {
            if (lexer->CurrentAtom.type == tok_comma) {
                lexer->GetNextToken(); // Consume ','
                continue;
            }

            if (auto arg = Source::ParseFunctionArg(parser, lexer)) {
                args.push_back(arg);
                continue;
            }

            if (lexer->CurrentAtom.type == tok_close_paren)
                break;

            parser->PrintError("Expected argument type!");
            return nullptr;
        }

        lexer->GetNextToken(); // Consume ')'

        // Parse return type
        std::shared_ptr<TypeDef> returnType = nullptr;
        if (lexer->CurrentAtom.type != tok_open_brace)
            returnType = Source::ParseTypeDef(parser, lexer);

        // Parse body
        std::vector<std::shared_ptr<Expr>> body;
        if (!Source::ParseBody(parser, lexer, body))
            return nullptr;

        return std::make_shared<Function>(funcName, args, returnType, body, ownerType);
    }
}