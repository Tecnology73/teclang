#pragma once

#include <string>
#include <vector>
#include <map>
#include "../TypeDef.h"

struct EnumTypeDef : TypeDef {
    explicit EnumTypeDef(std::string name) : TypeDef(std::move(name)) {}

    std::string print(unsigned indent = 0, bool onlyType = false) const override;

    // llvm::Type *generate(Generator *generator) const override;
};
