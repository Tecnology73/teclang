#pragma once

#include <string>
#include "defs.h"

class Debug {
public:
    static std::string debugAtom(const Atom &atom) {
        // Literals
        if (atom.type == tok_identifier) return "(ident) " + atom.value;
        if (atom.type == tok_int_literal) return "(int) " + atom.value;
        if (atom.type == tok_double_literal) return "(double) " + atom.value;
        if (atom.type == tok_string_literal) return "(string) " + atom.value;
        if (atom.type == tok_bool_literal) return "(bool) " + atom.value;

        return debugToken(atom.type);
    }

    static std::string debugToken(const Token &token) {
        // Keywords
        if (token == tok_func) return "func";
        if (token == tok_return) return "return";
        if (token == tok_if) return "if";
        if (token == tok_else) return "else";
        if (token == tok_for) return "for";
        if (token == tok_in) return "in";
        if (token == tok_extern) return "extern";

        // Types
        if (token == tok_int) return "int";
        if (token == tok_double) return "double";
        if (token == tok_string) return "string";
        if (token == tok_bool) return "bool";
        if (token == tok_char) return "char";

        // Operators
        if (token == tok_plus) return "+";
        if (token == tok_minus) return "-";
        if (token == tok_multiply) return "*";
        if (token == tok_divide) return "/";
        if (token == tok_open_paren) return "(";
        if (token == tok_close_paren) return ")";
        if (token == tok_open_bracket) return "[";
        if (token == tok_close_bracket) return "]";
        if (token == tok_open_brace) return "{";
        if (token == tok_close_brace) return "}";
        if (token == tok_semicolon) return ";";
        if (token == tok_colon) return ":";
        if (token == tok_question_mark) return "?";
        if (token == tok_ampersand) return "&";
        if (token == tok_pipe) return "|";
        if (token == tok_assign) return "=";
        if (token == tok_exclamation) return "!";
        if (token == tok_plus_equal) return "+=";
        if (token == tok_minus_equal) return "-=";
        if (token == tok_multi_equal) return "*=";
        if (token == tok_divide_equal) return "/=";
        if (token == tok_and) return "&&";
        if (token == tok_or) return "||";
        if (token == tok_plusplus) return "++";
        if (token == tok_minusminus) return "--";
        if (token == tok_not_equal) return "!=";
        if (token == tok_equal) return "==";
        if (token == tok_spread) return "..";
        if (token == tok_less_than) return "<";
        if (token == tok_greater_than) return ">";
        if (token == tok_lt_equal) return "<=";
        if (token == tok_gt_equal) return ">=";

        return "!! UNKNOWN Token !!";
    }
};