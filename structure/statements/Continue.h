#pragma once

#include "Statement.h"

namespace Statements {
    struct Continue : Statement {
        explicit Continue() {}

        std::string print(unsigned indent) const override {
            return Expr::print(indent, "%mcontinue%/");
        }

        llvm::Value *generate(Generator *generator, bool loadAddress) override {
            return generator->Builder->CreateBr(generator->CurrentScope->GetLoopTailLabel());
        }
    };
}