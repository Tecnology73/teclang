#pragma once

#include <string>

enum Token {
    tok_eof = INT_MIN,

    // Keywords
    tok_func,     // func
    tok_return,   // return
    tok_if,       // if
    tok_else,     // else
    tok_for,      // for
    tok_in,       // in
    tok_break,    // break
    tok_continue, // continue
    tok_extern,   // extern
    tok_type,     // type
    tok_delete,   // delete
    tok_defer,    // defer
    tok_match,    // match
    tok_enum,     // enum

    // Types
    tok_int,    // int
    tok_double, // double
    tok_string, // string
    tok_bool,   // bool
    tok_char,   // char
    tok_void,   // void

    // Literals
    tok_identifier,
    tok_int_literal,    // 10
    tok_double_literal, // 10.0
    tok_string_literal, // "value"
    tok_bool_literal,   // true|false

    // Operators
    tok_plus,          // +
    tok_minus,         // -
    tok_multiply,      // *
    tok_divide,        // /
    tok_open_paren,    // (
    tok_close_paren,   // )
    tok_open_bracket,  // [
    tok_close_bracket, // ]
    tok_open_brace,    // {
    tok_close_brace,   // }
    tok_semicolon,     // ;
    tok_colon,         // :
    tok_question_mark, // ?
    tok_ampersand,     // &
    tok_percent,       // %
    tok_pipe,          // |
    tok_caret,         // ^
    tok_assign,        // =
    tok_exclamation,   // !
    tok_period,        // .
    tok_comma,         // ,
    tok_less_than,     // <
    tok_greater_than,  // >
    tok_lt_equal,      // <=
    tok_gt_equal,      // >=
    tok_plus_equal,    // +=
    tok_minus_equal,   // -=
    tok_multi_equal,   // *=
    tok_divide_equal,  // /=
    tok_mod_equal,     // %=
    tok_and_equal,     // &=
    tok_or_equal,      // |=
    tok_xor_equal,     // ^=
    tok_and,           // &&
    tok_or,            // ||
    tok_plusplus,      // ++
    tok_minusminus,    // --
    tok_not_equal,     // !=
    tok_equal,         // ==
    tok_spread,        // ..
    tok_arrow,         // =>
};

struct Atom {
    Token type = tok_eof;
    // Used for identifiers & literals
    std::string value;
    // Line in source file
    unsigned line;
    // The range the character falls on, on the line
    unsigned lineStart;
    unsigned lineEnd;
};

struct TypeInfo {
    std::string name;
    std::string subType;
    uint64_t arraySize = 0;
    bool isArray = false;
};