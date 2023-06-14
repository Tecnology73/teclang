#pragma once

#include <string>
#include "../Expr.h"

namespace Statements {
    struct Statement : Expr {
        std::string print(unsigned indent) const override {
            return Expr::print(indent, "(Statement)");
        }
    };
}