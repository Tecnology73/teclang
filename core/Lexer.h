#pragma once

#include <fstream>
#include <string>
#include "defs.h"

class Lexer {
    std::string source;
    unsigned sourceCursor = 0;

    std::string currentLine;
    unsigned line = 0;
    unsigned linePos = 0;
    unsigned tokenLength = 0;

    char currentChar = EOF;

public:
    Atom CurrentAtom;

public:
    explicit Lexer(const std::string &sourceFilePath);

    Atom GetNextToken();

    Atom PeekNextToken(unsigned short count = 1);

    const std::string &GetCurrentLine() const;

    bool IsAtEnd() const;

private:
    void readNextLine();

    char peekChar();

    void consumeChar();

    /// Discard everything following a comment symbol until a new line or end of file is reached.
    void consumeComment();

    /// Parse a string literal. Also deals with special characters like new line, tabs, etc.
    /// \return
    Atom parseStringLiteral();

    /// Parse for known operators.
    /// If we encounter a plus or minus before a digit, parse a number literal instead.
    /// \return
    Atom parsePunctuation();

    /// Parse for keywords and boolean literals.
    /// Anything else is determined to be an identifier (ex. variable, func name)
    /// \return
    Atom parseAlpha();

    /// Both integers and doubles can be prefixed with a single plus (+) or minus (-) symbol.
    /// Valid integers include: 1, -2, -3, 100, etc.
    /// Valid doubles include: 1.0, -1.1, +1.2, 10.304, .498, etc.
    /// \return
    Atom parseNumberLiteral();

    Atom makeAtom(const Token &token, const std::string &value = "");
};