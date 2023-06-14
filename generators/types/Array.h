#pragma once

#include "../../core/Generator.h"

/*
 * type Array {
 *   void* .addr;
 *   int   length;
 * }
 */
struct Array {
    static void generate(Generator *generator) {
        auto arrayType = std::make_shared<TypeDef>(
            "Array",
            std::vector<std::shared_ptr<Statements::VariableDecl>>{
                std::make_shared<Statements::VariableDecl>(
                    Context::Instance->GetDeclaredType("void*"),
                    ".addr"
                ),
                std::make_shared<Statements::VariableDecl>(
                    Context::Instance->GetDeclaredType("int"),
                    "length"
                ),
            },
            std::vector<std::shared_ptr<TypeDef>>{}
        );

        // auto arrayStruct = arrayType->generate(generator);
        // arrayType->CompiledFunctions["constructor"] = constructor(generator, arrayType, arrayStruct);

        Context::Instance->AddDeclaredType(arrayType);
        arrayType->IsUserDefined = false;
    }

    static llvm::Function *createFunc(
        Generator *generator,
        const std::string &name,
        const std::string &returnType,
        const std::vector<llvm::Type *> &args,
        bool variableArgs = false
    ) {
        // Create the function
        auto funcType = llvm::FunctionType::get(
            generator->GetCompiledType(returnType),
            args,
            variableArgs
        );
        auto func = llvm::Function::Create(
            funcType,
            llvm::GlobalValue::PrivateLinkage,
            name,
            *generator->Module
        );

        // Generate the body
        auto bb = llvm::BasicBlock::Create(*generator->Context, "", func);
        generator->Builder->SetInsertPoint(bb);

        return func;
    }

    static llvm::Function *constructor(
        Generator *generator,
        const std::shared_ptr<TypeDef> &arrayType,
        llvm::Type *arrayStruct
    ) {
        auto prevBlock = generator->Builder->GetInsertBlock();
        // Create the function
        auto funcType = llvm::FunctionType::get(
            generator->GetCompiledType("Array")->getPointerTo(0),
            {
                generator->GetCompiledType("int"), // count
                generator->GetCompiledType("int"), // size
            },
            false
        );
        auto func = llvm::Function::Create(
            funcType,
            llvm::GlobalValue::PrivateLinkage,
            "Array_Constructor",
            *generator->Module
        );

        // Generate the body
        auto bb = llvm::BasicBlock::Create(*generator->Context, "", func);
        generator->Builder->SetInsertPoint(bb);

        // Allocate memory for the array
        auto arrMemory = generator->Builder->CreateBitCast(
            generator->Builder->CreateCall(
                generator->GetLibraryFunction("malloc"),
                llvm::ConstantExpr::getSizeOf(arrayStruct)
            ),
            arrayStruct->getPointerTo(0)
        );
        // Allocate enough memory based on the size and count.
        // calloc is used to initialize the memory to 0.
        auto addrPtr = generator->Builder->CreateCall(
            generator->GetLibraryFunction("calloc"),
            {
                // func->getArg(0),
                // func->getArg(1)
                llvm::ConstantInt::get(
                    generator->GetCompiledType("int"),
                    20
                ),
                llvm::ConstantInt::get(
                    generator->GetCompiledType("int"),
                    16
                ),
            }
        );
        generator->Builder->CreateStore(
            addrPtr,
            generator->Builder->CreateStructGEP(arrayStruct, arrMemory, arrayType->GetFieldIndex(".addr"))
        );
        // Set the length to 0
        generator->Builder->CreateStore(
            func->getArg(0),
            generator->Builder->CreateStructGEP(arrayStruct, arrMemory, arrayType->GetFieldIndex("length"))
        );

        generator->Builder->CreateRet(arrMemory);
        generator->Builder->SetInsertPoint(prevBlock);

        return func;
    }
};