#pragma once

#include <string>
#include <memory>
#include "../statements/Statement.h"
#include "../statements/VariableDecl.h"
#include "../../core/defs.h"

namespace Operations {
    struct BinaryOp : Statements::Statement {
        Token OpCode;
        std::shared_ptr<Expr> LHS, RHS;

        explicit BinaryOp(
            Token opCode,
            std::shared_ptr<Expr> lhs,
            std::shared_ptr<Expr> rhs
        );

        std::string print(unsigned int indent) const override;

        llvm::Value *generate(Generator *generator, bool loadAddress = false) override;

        virtual llvm::Value *generate(
            Generator *generator,
            const std::shared_ptr<Statements::VariableDecl> &prevRef,
            llvm::Value *prevValue
        ) const;
    };
}