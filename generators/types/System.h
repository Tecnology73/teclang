#pragma once

#include "../../core/Generator.h"

/**
 * type System {}
 *
 * func<System> log(...args) {
 *      printf(...args);
 * }
 */
struct System {
    static void generate(Generator *generator) {
        auto systemType = std::make_shared<TypeDef>(
            "System",
            std::vector<std::shared_ptr<Statements::VariableDecl>>{},
            std::vector<std::shared_ptr<TypeDef>>{}
        );

        auto systemStruct = systemType->generate(generator);

        // stringType->CompiledFunctions["constructor"] = constructor(generator, stringType, stringStruct);
        systemType->CompiledFunctions["log"] = log(generator, systemType, systemStruct);

        Context::Instance->AddDeclaredType(systemType);
        systemType->IsUserDefined = false;
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

    static llvm::Function *log(
        Generator *generator,
        const std::shared_ptr<TypeDef> &systemType,
        llvm::Type *systemStruct
    ) {
        auto prevBlock = generator->Builder->GetInsertBlock();
        auto func = createFunc(
            generator,
            "system_Log",
            "void",
            {},
            true
        );

        auto addr = generator->Builder->CreateStructGEP(
            systemStruct,
            func->getArg(0),
            systemType->GetFieldIndex(".addr")
        );

        generator->Builder->CreateRet(generator->Builder->CreateLoad(addr->getType()->getPointerElementType(), addr));
        generator->Builder->SetInsertPoint(prevBlock);

        return func;
    }
};