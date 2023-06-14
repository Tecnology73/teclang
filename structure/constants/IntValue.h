#pragma once

#include "Constants.h"
#include "../../core/Context.h"
#include "../../generators/TypeDef.h"
#include "../types/IntTypeDef.h"

namespace Constants {
    struct IntValue : ConstantValue {
        long Value;

        explicit IntValue(int value) :
            ConstantValue(Context::Instance->GetDeclaredType("int")),
            Value(value) {}

        explicit IntValue(const std::string &value) :
            ConstantValue(Context::Instance->GetDeclaredType("int")),
            Value(strtol(value.c_str(), nullptr, 10)) {}

        std::string print(unsigned indent) const override {
            return Expr::print(
                indent,
                "%o" + std::to_string(Value) + "%/"
            );
        }

        llvm::Value *generate(Generator *generator, bool loadAddress = true) override {
            auto intType = std::static_pointer_cast<IntTypeDef>(Type);

            return llvm::ConstantInt::get(
                intType->generate(generator),
                llvm::APInt(intType->BitLength, Value, !intType->IsUnsigned)
            );
        }

        llvm::Value *generate(Generator *generator, const std::shared_ptr<IntTypeDef> &targetType) const {
            return llvm::ConstantInt::get(
                targetType->generate(generator),
                llvm::APInt(targetType->BitLength, Value, !targetType->IsUnsigned)
            );
        }
    };
}