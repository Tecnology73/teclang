#pragma once

#include "Constants.h"

namespace Constants {
    struct DoubleValue : ConstantValue {
        double Value;

        explicit DoubleValue(double value) :
            ConstantValue(Context::Instance->GetDeclaredType("double")),
            Value(value) {}

        explicit DoubleValue(const std::string &value) :
            ConstantValue(Context::Instance->GetDeclaredType("double")),
            Value(strtod(value.c_str(), nullptr)) {}

        std::string print(unsigned indent) const override {
            return Expr::print(
                indent,
                "%o" + std::to_string(Value) + "%/"
            );
        }

        llvm::Value *generate(Generator *generator, bool loadAddress = true) override {
            return llvm::ConstantFP::get(
                Type->generate(generator),
                Value
            );
        }
    };
}