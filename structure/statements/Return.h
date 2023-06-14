#pragma once

#include <memory>
#include <utility>
#include "Statement.h"
#include "../../core/Generator.h"

namespace Statements {
    struct Return : Statement {
        std::shared_ptr<Expr> Statement;

        explicit Return(std::shared_ptr<Expr> statement) :
            Statement(std::move(statement)) {}

        std::string print(unsigned indent) const override {
            return Expr::print(
                indent,
                "%mreturn%/ " + Statement->print(0)
            );
        }

        llvm::Value *generate(Generator *generator, bool loadAddress) override {
            return generator->Builder->CreateRet(Statement->generate(generator));
        }
    };
}