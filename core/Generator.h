#pragma once

#include <memory>
#include <map>
#include <string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
//#include <llvm/Passes/OptimizationLevel.h>
// #include <llvm/IR/PassManager.h>
// #include <llvm/Passes/PassBuilder.h>
#include "./Parser.h"
#include "ContextScope.h"

struct Generator {
    explicit Generator();

    int Generate();

    int DumpModule(const std::string &outFilePath/*, llvm::OptimizationLevel optimizationLevel*/);

    bool HasCompiledType(const std::string &name) const;

    llvm::Type *SetCompiledType(const std::string &name, llvm::Type *type);

    llvm::Type *GetCompiledType(const std::string &name) const;

    llvm::Function *SetLibraryFunction(const std::string &name, llvm::Function *function);

    llvm::Function *GetLibraryFunction(const std::string &name) const;

    std::shared_ptr<ContextScope> OpenScope();

    std::shared_ptr<ContextScope> CloseScope();

    int PrintError(const std::string &message, int returnCode = 2) const;

public:
    llvm::LLVMContext *Context;
    llvm::IRBuilder<> *Builder;
    std::unique_ptr<llvm::Module> Module;
    // The currently active scope
    std::shared_ptr<ContextScope> CurrentScope;
    // A list of all built-in functions (malloc, free, etc.)
    std::map<std::string, llvm::Function *> LibraryFunctions;

private:
    void prepareBuiltInTypes();

    void prepareBuiltInFuncs();

    void declareFunc(
        const std::string &name,
        llvm::Type *returnType,
        const std::vector<llvm::Type *> &args,
        bool variableArgs = false
    );

private:
    // LLVM Optimizations
    // llvm::ModuleAnalysisManager MAM;
    // llvm::PassBuilder PB;

    std::map<std::string, llvm::Type *> compiledTypes;

    bool hasGenerated;
};