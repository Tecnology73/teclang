#pragma once

#include <utility>
#include "../TypeDef.h"

struct ArrayTypeDef : TypeDef {
    const std::shared_ptr<TypeDef> InnerType;
    std::shared_ptr<Expr> Length;

    explicit ArrayTypeDef(std::shared_ptr<TypeDef> innerType, std::shared_ptr<Expr> length = nullptr);

    llvm::Type *generate(Generator *generator) const override;

    std::string print(unsigned indent, bool onlyType) const override;
};