#pragma once

#include <utility>
#include "../TypeDef.h"

struct IntTypeDef : TypeDef {
    unsigned short BitLength;
    bool IsUnsigned;

    explicit IntTypeDef(unsigned short bitLength, bool isUnsigned = false, bool isPointer = false) :
        TypeDef("int", isPointer),
        BitLength(bitLength),
        IsUnsigned(isUnsigned) {}

    std::string GetRealName() const;

    std::string print(unsigned int indent, bool onlyType) const override;

    llvm::Type *generate(Generator *generator) const override;
};