#pragma once

#include "Constants.h"

namespace Constants {
    struct StringValue : ConstantValue {
        std::string Value;

        explicit StringValue(std::string value);

        std::string print(unsigned indent) const override;

        llvm::Value *generate(Generator *generator, bool loadAddress = false) override;
    };
}