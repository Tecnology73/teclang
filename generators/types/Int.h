#pragma once

#include "../../core/Generator.h"

struct Int {
    static void generate(Generator *generator) {
        auto intType = Context::Instance->GetDeclaredType("int");

        intType->CompiledFunctions["+="] = add(generator, intType);
    }

    static llvm::Function *add(Generator *generator, const std::shared_ptr<TypeDef> &type) {
        auto funcType = llvm::FunctionType::get(
                type->generate(generator),
                {
                        type->generate(generator)->getPointerTo(0),
                        type->generate(generator)
                },
                false
        );
        auto func = llvm::Function::Create(
                funcType,
                llvm::Function::PrivateLinkage,
                "int_Add",
                *generator->Module
        );
        func->addFnAttr(llvm::Attribute::AlwaysInline);

        auto a = func->getArg(0);
        auto b = func->getArg(1);
        auto prevInsertBlock = generator->Builder->GetInsertBlock();

        auto bb = llvm::BasicBlock::Create(*generator->Context, "", func);
        generator->Builder->SetInsertPoint(bb);

        auto sum = generator->Builder->CreateAdd(
                generator->Builder->CreateLoad(
                        a->getType()->getPointerElementType(),
                        a
                ),
                b
        );
        generator->Builder->CreateStore(sum, a);

        // return sum;
        generator->Builder->CreateRet(sum);
        generator->Builder->SetInsertPoint(prevInsertBlock);

        return func;
    }
};