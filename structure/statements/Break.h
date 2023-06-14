#pragma once

#include "Statement.h"

namespace Statements {
    struct Break : Statement {
        explicit Break() {}

        std::string print(unsigned indent) const override {
            return Expr::print(indent, "%mbreak%/");
        }

        llvm::Value *generate(Generator *generator, bool loadAddress) override {
            return generator->Builder->CreateBr(generator->CurrentScope->GetLoopExitLabel());
        }
    };
}