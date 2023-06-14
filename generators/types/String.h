#pragma once

#include "../../core/Generator.h"

/*
 * type string {
 *   void* .addr;
 *   int   .allocatedChunks;
 *   int   length;
 * }
 */
struct String {
    // Size of a single chunk in bytes.
    // This is used to allocate memory when resizing a string.
    static const int CHUNK_SIZE = 32;

    static void generate(Generator *generator) {
        auto stringType = std::make_shared<TypeDef>(
            "string",
            std::vector<std::shared_ptr<Statements::VariableDecl>>{
                std::make_shared<Statements::VariableDecl>(Context::Instance->GetDeclaredType("void*"),
                                                           ".addr"),
                std::make_shared<Statements::VariableDecl>(
                    Context::Instance->GetDeclaredType("int"),
                    ".allocatedChunks"
                ),
                std::make_shared<Statements::VariableDecl>(Context::Instance->GetDeclaredType("int"), "length"),
            },
            std::vector<std::shared_ptr<TypeDef>>{}
        );

        auto stringStruct = stringType->generate(generator);

        stringType->CompiledFunctions["constructor"] = constructor(generator, stringType, stringStruct);
        /*stringType->CompiledFunctions["resize"] = */resize(generator, stringType, stringStruct);
        stringType->CompiledFunctions["value"] = value(generator, stringType, stringStruct);
        stringType->CompiledFunctions["+="] = append(generator, stringType, stringStruct);

        Context::Instance->AddDeclaredType(stringType);
        stringType->IsUserDefined = false;
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
        const std::shared_ptr<TypeDef> &stringType,
        llvm::Type *stringStruct
    ) {
        auto prevBlock = generator->Builder->GetInsertBlock();
        // Initialize the string struct
        // TODO: Allow an initial value to be passed to the constructor
        // Create the function
        auto funcType = llvm::FunctionType::get(
            generator->GetCompiledType("string")->getPointerTo(0),
            {},
            true
        );
        auto func = llvm::Function::Create(
            funcType,
            llvm::GlobalValue::PrivateLinkage,
            "string_Constructor",
            *generator->Module
        );

        // Generate the body
        auto bb = llvm::BasicBlock::Create(*generator->Context, "", func);
        generator->Builder->SetInsertPoint(bb);

        // Allocate memory for the string type
        auto strInstance = generator->Builder->CreateBitCast(
            generator->Builder->CreateCall(
                generator->GetLibraryFunction("malloc"),
                llvm::ConstantExpr::getSizeOf(stringStruct)
            ),
            stringStruct->getPointerTo(0)
        );
        // Allocate 1 chunk of memory
        // calloc is used to initialize the memory to 0
        auto addrPtr = generator->Builder->CreateCall(
            generator->GetLibraryFunction("calloc"),
            {
                generator->Builder->getInt64(1),
                generator->Builder->getInt64(CHUNK_SIZE),
            }
        );
        generator->Builder->CreateStore(
            addrPtr,
            generator->Builder->CreateStructGEP(stringStruct, strInstance, stringType->GetFieldIndex(".addr"))
        );
        // Set the allocated chunks to 1
        generator->Builder->CreateStore(
            generator->Builder->getInt64(1),
            generator->Builder
                     ->CreateStructGEP(stringStruct, strInstance, stringType->GetFieldIndex(".allocatedChunks"))
        );
        // Set the length to 0
        generator->Builder->CreateStore(
            generator->Builder->getInt64(0),
            generator->Builder->CreateStructGEP(stringStruct, strInstance, stringType->GetFieldIndex("length"))
        );

        generator->Builder->CreateRet(strInstance);
        generator->Builder->SetInsertPoint(prevBlock);

        return func;
    }

    /**
     * Resizes the string to the given size.
     * If the given size will not fit in the allocated memory, the string will be reallocated.
     * TODO: Shrink the string if the given size is smaller than the current allocation.
     *
     * @param generator
     * @param stringType
     * @param stringStruct
     */
    static llvm::Function *resize(
        Generator *generator,
        const std::shared_ptr<TypeDef> &stringType,
        llvm::Type *stringStruct
    ) {
        auto prevBlock = generator->Builder->GetInsertBlock();
        auto func = createFunc(
            generator,
            "string_Resize",
            "void",
            {
                stringStruct->getPointerTo(), // string
                generator->GetCompiledType("int") // newLength
            }
        );

        // Get the arguments
        auto string = func->getArg(0);
        // The new length of the string (excluding the null terminator)
        auto newLength = func->getArg(1);

        // Check if the allocatedSize is enough
        auto notEnoughBB = llvm::BasicBlock::Create(*generator->Context, "notEnough", func);
        auto enoughBB = llvm::BasicBlock::Create(*generator->Context, "enough", func);

        auto allocatedChunksPtr = generator->Builder->CreateStructGEP(
            stringStruct,
            string,
            stringType->GetFieldIndex(".allocatedChunks")
        );
        auto allocatedChunks = generator->Builder->CreateLoad(
            allocatedChunksPtr->getType()->getPointerElementType(),
            allocatedChunksPtr,
            "allocatedChunks"
        );
        auto lengthPtr = generator->Builder->CreateStructGEP(
            stringStruct,
            string,
            stringType->GetFieldIndex("length")
        );
        // Calculate how many chunks are required for the new length
        // +1 is added to round up
        auto requiredChunks = generator->Builder->CreateAdd(
            generator->Builder->CreateSDiv(
                // +1 for null terminator
                generator->Builder->CreateAdd(
                    newLength,
                    generator->Builder->getInt64(1)
                ),
                generator->Builder->getInt64(CHUNK_SIZE),
                "requiredChunks"
            ),
            generator->Builder->getInt64(1)
        );
        auto isEnough = generator->Builder->CreateICmpSGT(requiredChunks, allocatedChunks, "isEnough");
        generator->Builder->CreateCondBr(isEnough, notEnoughBB, enoughBB);

        //
        // Not enough - Resize the string
        //
        generator->Builder->SetInsertPoint(notEnoughBB);
        auto addrPtr = generator->Builder->CreateStructGEP(
            stringStruct,
            string,
            stringType->GetFieldIndex(".addr")
        );
        // Allocate more memory
        // requiredChunks * CHUNK_SIZE * sizeof(int8) + 1
        auto allocationSize = generator->Builder->CreateMul(
            generator->Builder->CreateMul(
                requiredChunks,
                generator->Builder->getInt64(CHUNK_SIZE)
            ),
            llvm::ConstantExpr::getSizeOf(generator->GetCompiledType("int8")),
            "allocationSize"
        );
        generator->Builder->CreateCall(
            generator->GetLibraryFunction("printf"),
            {
                generator->Builder->CreateGlobalString("[REALLOC] Resizing string from %d to %d chunks (%d)\n"),
                allocatedChunks,
                requiredChunks,
                allocationSize
            }
        );
        // realloc(allocationSize)
        auto newAddr = generator->Builder->CreateCall(
            generator->GetLibraryFunction("realloc"),
            {
                generator->Builder->CreateBitCast(
                    generator->Builder->CreateLoad(
                        addrPtr->getType()->getPointerElementType(),
                        addrPtr
                    ),
                    generator->GetCompiledType("void*")
                ),
                allocationSize
            },
            "newAddr"
        );
        // string.addr = newAddr
        generator->Builder->CreateStore(newAddr, addrPtr);
        // string.allocatedChunks = requiredChunks
        generator->Builder->CreateStore(requiredChunks, allocatedChunksPtr);
        generator->Builder->CreateBr(enoughBB);

        //
        // Enough size
        //
        generator->Builder->SetInsertPoint(enoughBB);
        // string.length = newLength
        generator->Builder->CreateStore(newLength, lengthPtr);

        // Return
        generator->Builder->CreateRetVoid();
        generator->Builder->SetInsertPoint(prevBlock);

        return func;
    }

    static llvm::Function *append(
        Generator *generator,
        const std::shared_ptr<TypeDef> &stringType,
        llvm::Type *stringStruct
    ) {
        auto prevBlock = generator->Builder->GetInsertBlock();
        auto func = createFunc(
            generator,
            "string_Append",
            "void",
            {
                stringStruct->getPointerTo(), // self
                generator->GetCompiledType("int8*") // str
            }
        );

        // Get the arguments
        auto self = func->getArg(0);
        auto str = func->getArg(1);

        // Get the length of the string
        auto appendLength = generator->Builder->CreateCall(
            generator->GetLibraryFunction("strlen"),
            str
        );

        // Resize the string
        auto lengthPtr = generator->Builder->CreateStructGEP(
            stringStruct,
            self,
            stringType->GetFieldIndex("length")
        );
        generator->Builder->CreateCall(
            generator->Module->getFunction("string_Resize"),
            {
                self,
                generator->Builder->CreateAdd(
                    generator->Builder->CreateLoad(
                        lengthPtr->getType()->getPointerElementType(),
                        lengthPtr
                    ),
                    appendLength
                )
            }
        );

        // Append the string
        auto addrPtr = generator->Builder->CreateStructGEP(
            stringStruct,
            self,
            stringType->GetFieldIndex(".addr")
        );
        auto addr = generator->Builder->CreateLoad(
            addrPtr->getType()->getPointerElementType(),
            addrPtr
        );
        generator->Builder->CreateCall(
            generator->Module->getFunction("strcat"),
            {
                generator->Builder->CreateBitCast(addr, generator->GetCompiledType("int8*")),
                generator->Builder->CreateBitCast(str, generator->GetCompiledType("int8*"))
            }
        );

        generator->Builder->CreateRetVoid();
        generator->Builder->SetInsertPoint(prevBlock);

        return func;
    }

    static llvm::Function *value(
        Generator *generator,
        const std::shared_ptr<TypeDef> &stringType,
        llvm::Type *stringStruct
    ) {
        auto prevBlock = generator->Builder->GetInsertBlock();
        auto func = createFunc(
            generator,
            "string_Value",
            "int8*",
            {
                stringStruct->getPointerTo(), // self
            }
        );

        auto addr = generator->Builder->CreateStructGEP(
            stringStruct,
            func->getArg(0),
            stringType->GetFieldIndex(".addr")
        );

        generator->Builder->CreateRet(generator->Builder->CreateLoad(addr->getType()->getPointerElementType(), addr));
        generator->Builder->SetInsertPoint(prevBlock);

        return func;
    }
};