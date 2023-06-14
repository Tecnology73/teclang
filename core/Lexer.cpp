#include "Lexer.h"

#define EOL -2

Lexer::Lexer(const std::string &sourceFilePath) {
    std::ifstream t(sourceFilePath);
    t.seekg(0, std::ios::end);

    size_t size = t.tellg();
    source = std::string(size, ' ');

    t.seekg(0);
    t.read(&source[0], size);

    readNextLine();
}

Atom Lexer::GetNextToken() {
    consumeChar();

    while (currentChar == EOL || currentChar == '\0' || isspace(currentChar)) {
        tokenLength = 0;
        consumeChar();
    }

    if (currentChar == EOF)
        return makeAtom(tok_eof);

    // Comments
    if (currentChar == '#') {
        consumeComment();

        return GetNextToken();
    }

    // String Literals
    if (currentChar == '"')
        return parseStringLiteral();

    // Operators
    if (ispunct(currentChar))
        return parsePunctuation();

    // Keywords/Identifiers/Literals
    if (isalpha(currentChar))
        return parseAlpha();

    // Number literal
    if (isdigit(currentChar))
        return parseNumberLiteral();

    return makeAtom(tok_eof);
}

Atom Lexer::PeekNextToken(unsigned short count) {
    unsigned sourceCursor = Lexer::sourceCursor;
    unsigned line = Lexer::line;
    unsigned linePos = Lexer::linePos;
    unsigned tokenLength = Lexer::tokenLength;
    std::string currentLine = Lexer::currentLine;
    Atom currentAtom = Lexer::CurrentAtom;

    Atom atom;
    do {
        atom = GetNextToken();
    } while (--count > 0);

    Lexer::sourceCursor = sourceCursor;
    Lexer::line = line;
    Lexer::linePos = linePos;
    Lexer::tokenLength = tokenLength;
    Lexer::currentLine = currentLine;
    Lexer::CurrentAtom = currentAtom;

    return atom;
}

const std::string &Lexer::GetCurrentLine() const {
    return currentLine;
}

bool Lexer::IsAtEnd() const {
    return linePos == currentLine.size() && sourceCursor >= source.size();
}

///
/// Private
//

void Lexer::readNextLine() {
    if (IsAtEnd())
        return;

    currentLine = "";
    while (sourceCursor < source.size() &&
           source[sourceCursor] != EOF &&
           source[sourceCursor] != '\n' &&
           source[sourceCursor] != '\r')
        currentLine += source[sourceCursor++];

    linePos = 0;
    line++;
}

char Lexer::peekChar() {
    if (linePos >= currentLine.size())
        return EOL;

    return currentLine[linePos];
}

void Lexer::consumeChar() {
    if (linePos >= currentLine.size()) {
        sourceCursor++;
        readNextLine();

        if (sourceCursor > source.size())
            currentChar = EOF;
        else
            currentChar = EOL;

        return;
    }

    if (sourceCursor > source.size()) {
        currentChar = EOF;
        return;
    }

    currentChar = currentLine[linePos++];
    tokenLength++;
}

/// Discard everything following a comment symbol until a new line or end of file is reached.
void Lexer::consumeComment() {
    do {
        consumeChar();
    } while (currentChar != '\n' && currentChar != '\r' && currentChar != EOF && currentChar != EOL);
}

/// Parse a string literal. Also deals with special characters like new line, tabs, etc.
/// \return
Atom Lexer::parseStringLiteral() {
    std::string val;
    val += (char) currentChar;

    do {
        consumeChar();

        // Handle escaping the next character
        if (currentChar == '\\') {
            consumeChar();

            if (currentChar == 'n') val += '\n';
            else if (currentChar == 'r') val += '\r';
            else if (currentChar == 't') val += '\t';
            else if (currentChar == '"') val += '"'; // Escaped quote
            else {
                // Not escaping any special char
                val += '\\';
                val += currentChar;
            }

            continue;
        }

        val += (char) currentChar;

        if (currentChar == '"')
            return makeAtom(tok_string_literal, val);
    } while (currentChar != EOF && currentChar != '\n');

    return makeAtom(tok_eof);
}

/// Parse for known operators.
/// If we encounter a plus or minus before a digit, parse a number literal instead.
/// \return
Atom Lexer::parsePunctuation() {
    char nextChar = peekChar();

    if (((currentChar == '+' || currentChar == '-') && (isdigit(nextChar) || nextChar == '.')) ||
        (currentChar == '.' && isdigit(nextChar)))
        return parseNumberLiteral();

    // DoubleExpr char operators
    // WARNING: Ugly code ahead... Please close eyes to keep sanity!
    if (currentChar == '+' && nextChar == '=')
        return consumeChar(), makeAtom(tok_plus_equal);
    if (currentChar == '-' && nextChar == '=')
        return consumeChar(), makeAtom(tok_minus_equal);
    if (currentChar == '*' && nextChar == '=')
        return consumeChar(), makeAtom(tok_multi_equal);
    if (currentChar == '/' && nextChar == '=')
        return consumeChar(), makeAtom(tok_divide_equal);
    if (currentChar == '&' && nextChar == '&')
        return consumeChar(), makeAtom(tok_and);
    if (currentChar == '|' && nextChar == '|')
        return consumeChar(), makeAtom(tok_or);
    if (currentChar == '+' && nextChar == '+')
        return consumeChar(), makeAtom(tok_plusplus);
    if (currentChar == '-' && nextChar == '-')
        return consumeChar(), makeAtom(tok_minusminus);
    if (currentChar == '!' && nextChar == '=')
        return consumeChar(), makeAtom(tok_not_equal);
    if (currentChar == '=' && nextChar == '=')
        return consumeChar(), makeAtom(tok_equal);
    if (currentChar == '.' && nextChar == '.')
        return consumeChar(), makeAtom(tok_spread);
    if (currentChar == '<' && nextChar == '=')
        return consumeChar(), makeAtom(tok_lt_equal);
    if (currentChar == '>' && nextChar == '=')
        return consumeChar(), makeAtom(tok_gt_equal);
    if (currentChar == '%' && nextChar == '=')
        return consumeChar(), makeAtom(tok_mod_equal);
    if (currentChar == '&' && nextChar == '=')
        return consumeChar(), makeAtom(tok_and_equal);
    if (currentChar == '|' && nextChar == '=')
        return consumeChar(), makeAtom(tok_or_equal);
    if (currentChar == '^' && nextChar == '=')
        return consumeChar(), makeAtom(tok_xor_equal);
    if (currentChar == '=' && nextChar == '>')
        return consumeChar(), makeAtom(tok_arrow);

    // Single char operators
    if (currentChar == '+') return makeAtom(tok_plus);
    if (currentChar == '-') return makeAtom(tok_minus);
    if (currentChar == '*') return makeAtom(tok_multiply);
    if (currentChar == '/') return makeAtom(tok_divide);
    if (currentChar == '(') return makeAtom(tok_open_paren);
    if (currentChar == ')') return makeAtom(tok_close_paren);
    if (currentChar == '[') return makeAtom(tok_open_bracket);
    if (currentChar == ']') return makeAtom(tok_close_bracket);
    if (currentChar == '{') return makeAtom(tok_open_brace);
    if (currentChar == '}') return makeAtom(tok_close_brace);
    if (currentChar == ';') return makeAtom(tok_semicolon);
    if (currentChar == ':') return makeAtom(tok_colon);
    if (currentChar == '?') return makeAtom(tok_question_mark);
    if (currentChar == '&') return makeAtom(tok_ampersand);
    if (currentChar == '|') return makeAtom(tok_pipe);
    if (currentChar == '^') return makeAtom(tok_caret);
    if (currentChar == '=') return makeAtom(tok_assign);
    if (currentChar == '!') return makeAtom(tok_exclamation);
    if (currentChar == '.') return makeAtom(tok_period);
    if (currentChar == ',') return makeAtom(tok_comma);
    if (currentChar == '<') return makeAtom(tok_less_than);
    if (currentChar == '>') return makeAtom(tok_greater_than);
    if (currentChar == '%') return makeAtom(tok_percent);

    // Unknown operator
    return makeAtom(tok_identifier, std::string(1, currentChar));
}

/// Parse for keywords and boolean literals.
/// Anything else is determined to be an identifier (ex. variable, func name)
/// \return
Atom Lexer::parseAlpha() {
    std::string val;
    val += currentChar;

    char nextChar;
    while ((nextChar = peekChar()) != EOL && isalnum(nextChar) || nextChar == '_') {
        consumeChar();
        val += currentChar;
    }

    // Keywords
    if (val == "func") return makeAtom(tok_func);
    if (val == "return") return makeAtom(tok_return);
    if (val == "int") return makeAtom(tok_int, val);
    if (val == "int8") return makeAtom(tok_int, val);
    if (val == "int16") return makeAtom(tok_int, val);
    if (val == "int32") return makeAtom(tok_int, val);
    if (val == "int64") return makeAtom(tok_int, val);
    if (val == "double") return makeAtom(tok_double, val);
    if (val == "bool") return makeAtom(tok_bool, val);
    if (val == "string") return makeAtom(tok_string, val);
    if (val == "void") return makeAtom(tok_void, val);
    if (val == "if") return makeAtom(tok_if);
    if (val == "else") return makeAtom(tok_else);
    if (val == "for") return makeAtom(tok_for);
    if (val == "in") return makeAtom(tok_in);
    if (val == "break") return makeAtom(tok_break);
    if (val == "continue") return makeAtom(tok_continue);
    if (val == "type") return makeAtom(tok_type);
    if (val == "delete") return makeAtom(tok_delete);
    if (val == "defer") return makeAtom(tok_defer);
    if (val == "match") return makeAtom(tok_match);
    if (val == "enum") return makeAtom(tok_enum);

    // Boolean Literals
    if (val == "true" || val == "false")
        return makeAtom(tok_bool_literal, val);

    return makeAtom(tok_identifier, val);
}

/// Both integers and doubles can be prefixed with a single plus (+) or minus (-) symbol.
/// Valid integers include: 1, -2, -3, 100, etc.
/// Valid doubles include: 1.0, -1.1, +1.2, 10.304, .498, etc.
/// \return
Atom Lexer::parseNumberLiteral() {
    std::string val;
    val += currentChar;
    // DoubleExpr can start with . instead of a digit
    bool isDouble = currentChar == '.';
    char nextChar;

    while ((nextChar = peekChar()) != EOL && (isdigit(nextChar) || nextChar == '.' || nextChar == '_')) {
        consumeChar();

        if (currentChar == '.' && peekChar() == '.') {
            // What looked like a double is actually a spread operator.
            // Decrease the current location in the stream, so we can parse the spread operator again.
            linePos--;
            break;
        }

        if (isDouble && currentChar == '.') {
            // Don't allow multiple periods in a double.
            linePos--;
            break;
        }

        // Ignore underscores in numbers
        if (currentChar == '_')
            continue;

        val += currentChar;
        isDouble = isDouble || currentChar == '.';
    }

    if (isDouble)
        return makeAtom(tok_double_literal, val);

    return makeAtom(tok_int_literal, val);
}

Atom Lexer::makeAtom(const Token &token, const std::string &value) {
    unsigned tokenStart = linePos + 1 - tokenLength;
    tokenLength = 0;

    return CurrentAtom = {
        token,
        value,
        line,
        tokenStart,
        linePos + 1
    };
}