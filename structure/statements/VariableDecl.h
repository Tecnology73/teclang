#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Statement.h"

struct TypeDef;

namespace Statements {
    struct VariableDecl : std::enable_shared_from_this<VariableDecl>, Statement {
        std::shared_ptr<TypeDef> Type;
        std::string Name;
        std::vector<std::shared_ptr<Expr>> ConstructorArgs;
        // Set to true if, in the source code, the variable was declared with parenthesis.
        // This is required as there might not be a value inside the parens.
        // E.g. string test();
        bool CallConstruct = false;

        llvm::Value *AllocInst = nullptr;

        VariableDecl(
            std::shared_ptr<TypeDef> type,
            std::string name,
            std::vector<std::shared_ptr<Expr>> constructorArgs = {},
            bool callConstruct = false
        );

        std::string print(unsigned indent) const override;

        llvm::Value *generate(Generator *generator, bool loadAddress = false) override;
    };
}

