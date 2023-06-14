#pragma once

#include <string>
#include <memory>
#include "Lexer.h"

struct Parser {
    std::shared_ptr<Lexer> Lexer;

    explicit Parser(const std::string &sourceFilePath);

    int Parse();

    int PrintError(const std::string &message, int returnCode = 1) const;

private:
    bool handleFunction();

    bool handleType();

    bool handleEnum();
};
