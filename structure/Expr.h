#pragma once

#include <string>
#include <llvm/IR/Value.h>
#include "../core/Generator.h"

namespace Statements { struct VariableDecl; }

struct Expr {
    virtual ~Expr() = default;

    virtual std::string print(unsigned indent) const {
        return print(indent, "(Expr)");
    }

    std::string print(unsigned indent, const std::string &str) const {
        return std::string(indent, '\t') + str;
    }

    virtual llvm::Value *generate(Generator *generator, bool loadAddress = true) {
        return nullptr;
    }

    virtual llvm::Value *generate(
        Generator *generator,
        const std::shared_ptr<Statements::VariableDecl> &prevRef,
        llvm::Value *prevValue
    ) {
        return nullptr;
    }

    /*
     * Operators
     */

    virtual llvm::Value *add(Generator *generator, const std::shared_ptr<Expr> &rhs) {
        return nullptr;
    }
};