#pragma once

#include <vector>
#include <llvm/IR/Function.h>
#include "../generators/TypeDef.h"
#include "../structure/Function.h"

namespace Generators {
    struct Function {
        static llvm::Function *Generate(Generator *generator, const std::shared_ptr<::Function> &function) {
            if (function->CompiledFunction)
                return function->CompiledFunction;

            generator->OpenScope();

            auto prevInsertBlock = generator->Builder->GetInsertBlock();
            auto func = GeneratePrototype(generator, function);
            // Create the entry block
            auto bb = llvm::BasicBlock::Create(*generator->Context, "", func);
            generator->Builder->SetInsertPoint(bb);

            // Initialize the arguments
            unsigned i = 0;
            for (auto &arg: func->args())
                function->GetArg(i++)->generate(generator, &arg);

            // Generate the body
            for (const auto &item: function->Body)
                item->generate(generator);

            generator->CloseScope();
            // Restore the insert block
            generator->Builder->SetInsertPoint(prevInsertBlock);

            function->CompiledFunction = func;

            return func;
        }

        static llvm::Function *GeneratePrototype(Generator *generator, const std::shared_ptr<::Function> &function) {
            std::vector<llvm::Type *> argTypes;

            for (unsigned i = 0; i < function->Args.size(); i++) {
                auto argType = function->GetArg(i)->Type->generate(generator);

                if (i == 0 && function->OwnerType != nullptr)
                    argTypes.push_back(argType->getPointerTo(0));
                else
                    argTypes.push_back(argType);
            }

            auto funcName = function->Name;
            if (function->OwnerType) {
                // Constructors return the type they're in
                if (!function->ReturnType && funcName == "constructor") {
                    function->ReturnType = function->OwnerType;
                }


                funcName = function->OwnerType->Name + "_" + funcName;
            }

            auto funcType = llvm::FunctionType::get(
                function->ReturnType->generate(generator),
                argTypes,
                false
            );
            auto func = llvm::Function::Create(
                funcType,
                funcName == "main" ? llvm::Function::ExternalLinkage : llvm::Function::PrivateLinkage,
                funcName,
                *generator->Module
            );

            // Name the arguments
            unsigned i = 0;
            for (auto &item: func->args())
                item.setName(function->Args[i++]->Name);

            return func;
        }
    };
}