#include <iostream>
#include "Parser.h"
#include "Context.h"
#include "../parsers/Function.h"
#include "../parsers/Type.h"
#include "./debug/Console.h"
#include "../parsers/EnumDef.h"

Parser::Parser(const std::string &sourceFilePath) {
    Lexer = std::make_shared<::Lexer>(sourceFilePath);
}

int Parser::Parse() {
    Lexer->GetNextToken();

    while (Lexer->CurrentAtom.type != tok_eof) {
        switch (Lexer->CurrentAtom.type) {
            case tok_func:
                if (!handleFunction()) return -1;
                break;
            case tok_type:
                if (!handleType()) return -1;
                break;
            case tok_enum:
                if (!handleEnum()) return -1;
                break;
            default:
                PrintError("Unhandled top level expression!");
                return -1;
        }
    }

    return 0;
}

int Parser::PrintError(const std::string &message, int returnCode) const {
    std::cerr << "Error (" << Lexer->CurrentAtom.line << ':' << Lexer->CurrentAtom.lineStart << "): " << message
              << '\n' << Lexer->GetCurrentLine() << '\n'
              << std::string(Lexer->CurrentAtom.lineStart - 1, ' ')
              << '^'
              << std::endl;

    return returnCode;
}

/*
 * Private
 */

bool Parser::handleFunction() {
    if (auto func = Source::ParseFunction(this, Lexer)) {
        bool result;
        if (func->OwnerType)
            result = func->OwnerType->AddFunction(func);
        else
            result = Context::Instance->AddFunction(func);

        if (result)
            Console::Print(func->print());

        return result;
    }

    return false;
}

bool Parser::handleType() {
    if (auto type = Source::ParseType(this, Lexer)) {
        if (Context::Instance->AddDeclaredType(type, true)) {
            Console::Print(type->print());
            return true;
        }
    }

    return false;
}

bool Parser::handleEnum() {
    if (auto enumType = Source::ParseEnum(this, Lexer)) {
        if (Context::Instance->AddDeclaredType(enumType, true)) {
            Console::Print(enumType->print());
            return true;
        }
    }

    return false;
}
