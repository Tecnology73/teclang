#include "VariableDecl.h"
#include "../TypeDef.h"
#include "../BinaryOp.h"
#include "../expressions/Match.h"
#include "../../structure/operations/BinaryOp.h"

namespace Source {
    std::shared_ptr<Statements::VariableDecl> ParseVariableDecl(Parser *parser, const std::shared_ptr<Lexer> &lexer) {
        auto typeDef = Source::ParseTypeDef(parser, lexer);

        if (lexer->CurrentAtom.type != tok_identifier) {
            parser->PrintError("Expected variable name!");
            return nullptr;
        }

        std::string varName = lexer->CurrentAtom.value;
        std::vector<std::shared_ptr<Expr>> constructorArgs;
        bool callConstruct = false;

        // Parse value
        // 1. int num; // default value
        // 2. int num(); // default value
        // 3. int num(10); // value = 10
        // 4. User user; // null
        // 5. User user(); // default constructor
        // 6. User user(IsLocked: true); // default constructor + assign values
        if (lexer->GetNextToken().type == tok_open_paren) {
            callConstruct = true;
            lexer->GetNextToken(); // Consume '('

            while (lexer->CurrentAtom.type != tok_close_paren) {
                if (auto arg = Source::ParseBinaryOp(parser, lexer)) {
                    constructorArgs.push_back(arg);
                    
                    if (lexer->CurrentAtom.type != tok_close_paren && lexer->CurrentAtom.type != tok_comma) {
                        parser->PrintError("Expected ',' or ')'");
                        return nullptr;
                    }
                }

                if (lexer->CurrentAtom.type == tok_comma) {
                    lexer->GetNextToken(); // Consume ','
                }
            }

            lexer->GetNextToken(); // Consume ')'
        }

        // Don't allow an assignment if we're calling a constructor.
        if (callConstruct && lexer->CurrentAtom.type != tok_semicolon) {
            parser->PrintError("Expected semicolon!");
            return nullptr;
        }

        if (lexer->CurrentAtom.type == tok_assign) {
            if (lexer->PeekNextToken().type != tok_match) {
                parser->PrintError("Unexpected token!");
                return nullptr;
            }

            lexer->GetNextToken(); // Consume '='
            constructorArgs.push_back(Source::ParseMatch(parser, lexer));
        }

        if (lexer->CurrentAtom.type != tok_semicolon) {
            parser->PrintError("Expected semicolon!");
            return nullptr;
        }

        return std::make_shared<Statements::VariableDecl>(typeDef, varName, constructorArgs, callConstruct);
    }
}
