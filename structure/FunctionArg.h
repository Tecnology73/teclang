#pragma once

#include "TypeDef.h"
#include "./constants/Constants.h"
#include "./statements/VariableDecl.h"

struct FunctionArg : Statements::VariableDecl {
    FunctionArg(std::shared_ptr<TypeDef> type, std::string name, std::shared_ptr<Constants::ConstantValue> value) :
        Statements::VariableDecl(std::move(type), std::move(name), {value}) {}

    std::string print(unsigned indent) const override {
        std::string out = std::string(indent, '\t') +
                          Type->print(indent, true) + ' ' +
                          Name;

        for (const auto &arg: ConstructorArgs) {
            if (!arg) continue;
            out += " = " + arg->print(indent);
        }

        return out;
    }

    llvm::Value *generate(Generator *generator, llvm::Argument *arg) {
        generator->CurrentScope->AddVar(Name, shared_from_this());

        if (arg->getType()->isPointerTy()) {
            return AllocInst = generator->Builder->CreateInBoundsGEP(
                arg->getType()->getPointerElementType(),
                arg,
                llvm::ConstantInt::getSigned(generator->GetCompiledType("int"), arg->getArgNo()),
                Name
            );
        }

        return AllocInst = generator->Builder->CreateAlloca(arg->getType(), nullptr, Name);
    }
};