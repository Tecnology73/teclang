#pragma once

#include <llvm/IR/Type.h>
#include "../core/Generator.h"
#include "../structure/TypeDef.h"
#include "../structure/statements/VariableDecl.h"
#include "../structure/constants/IntValue.h"

namespace Generators {
    struct Constructor {
        static llvm::Function *Generate(Generator *generator, const ::TypeDef *typeDef) {
            if (auto func = generator->Module->getFunction(typeDef->Name + "_Constructor"))
                return func;

            generator->OpenScope();

            auto prevInsertBlock = generator->Builder->GetInsertBlock();
            auto func = GeneratePrototype(generator, typeDef);
            // Create the entry block
            auto bb = llvm::BasicBlock::Create(*generator->Context, "", func);
            generator->Builder->SetInsertPoint(bb);

            // Generate the body
            // Allocate memory for the Type
            auto type = typeDef->generate(generator);
            auto self = generator->Builder->CreateBitCast(
                generator->Builder->CreateCall(
                    generator->GetLibraryFunction("malloc"),
                    llvm::ConstantExpr::getSizeOf(type)
                ),
                type->getPointerTo(0),
                "self"
            );

            for (const auto &field: typeDef->Fields) {
                llvm::Value *value = nullptr;
                if (field->Type->IsPointer && !field->CallConstruct) {
                    value = llvm::ConstantInt::getNullValue(field->Type->generate(generator)->getPointerTo(0));
                } else if (!field->Type->IsComplexType) {
                    if (!field->CallConstruct)
                        continue; // No value provided to initialize with
                    if (field->ConstructorArgs.empty()) {
                        // TODO: Initialize with default value
                        continue;
                    }

                    value = field->ConstructorArgs[0]->generate(generator);
                } else if (field->CallConstruct) {
                    auto constructor = Generators::Constructor::Generate(generator, field->Type);
                    std::vector<llvm::Value *> args;

                    for (const auto &arg: field->ConstructorArgs)
                        args.push_back(arg->generate(generator));

                    value = generator->Builder->CreateCall(constructor, args);
                }

                // Value is not being initialized.
                if (!value) continue;

                auto valuePtr = generator->Builder->CreateInBoundsGEP(
                    self->getType()->getPointerElementType(),
                    self,
                    {
                        llvm::ConstantInt::get(generator->GetCompiledType("int"), 0),
                        llvm::ConstantInt::get(
                            generator->GetCompiledType("int32"),
                            typeDef->GetFieldIndex(field->Name)
                        ),
                    },
                    field->Name
                );

                field->AllocInst = valuePtr;
                generator->Builder->CreateStore(value, valuePtr);
            }

            // Return the constructed Type
            generator->Builder->CreateRet(self);

            generator->CloseScope();
            // Restore the insert block
            generator->Builder->SetInsertPoint(prevInsertBlock);

            return func;
        }

        static llvm::Function *Generate(Generator *generator, const std::shared_ptr<::TypeDef> &typeDef) {
            return Generate(generator, typeDef.get());
        }

    private:
        static llvm::Function *GeneratePrototype(Generator *generator, const ::TypeDef *typeDef) {
            auto funcName = typeDef->Name + "_Constructor";
            auto funcType = llvm::FunctionType::get(
                typeDef->generate(generator)->getPointerTo(0),
                {},
                false
            );

            return llvm::Function::Create(
                funcType,
                llvm::Function::PrivateLinkage,
                funcName,
                *generator->Module
            );
        }
    };
}