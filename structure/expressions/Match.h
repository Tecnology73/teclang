#pragma once

#include "../Expr.h"
#include "./MatchCase.h"

namespace Expressions {
    struct Match : Expr {
        std::shared_ptr<Expr> Value;
        std::vector<std::shared_ptr<MatchCase>> Cases;

        explicit Match(
            std::shared_ptr<Expr> value,
            std::vector<std::shared_ptr<MatchCase>> cases
        );

        std::string print(unsigned indent) const override;

        llvm::Value *generate(Generator *generator, const std::shared_ptr<Statements::VariableDecl> &var);
    };
}