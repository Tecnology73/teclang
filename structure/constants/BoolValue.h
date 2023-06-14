#pragma once

#include "../../core/Context.h"
#include "Constants.h"

namespace Constants {
    struct BoolValue : ConstantValue {
        bool Value;

        explicit BoolValue(bool value) :
            ConstantValue(Context::Instance->GetDeclaredType("bool")),
            Value(value) {}

        explicit BoolValue(const std::string &value) :
            ConstantValue(Context::Instance->GetDeclaredType("bool")),
            Value(value == "true") {}

        std::string print(unsigned indent) const override {
            std::string out = "false";
            if (Value)
                out = "true";

            return Expr::print(
                indent,
                "%o" + out + "%/"
            );
        }

        llvm::Value *generate(Generator *generator, bool loadAddress = true) override {
            return llvm::ConstantInt::getBool(Type->generate(generator), Value);
        }
    };
}