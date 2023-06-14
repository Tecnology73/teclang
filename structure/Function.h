#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "TypeDef.h"
#include "FunctionArg.h"

struct Function {
    std::string Name;
    std::vector<std::shared_ptr<FunctionArg>> Args;
    std::map<std::string, unsigned> ArgIndices;
    std::shared_ptr<TypeDef> ReturnType;
    std::vector<std::shared_ptr<Expr>> Body;
    std::shared_ptr<TypeDef> OwnerType;

    llvm::Function *CompiledFunction = nullptr;

    Function(
        std::string name,
        std::vector<std::shared_ptr<FunctionArg>> args,
        std::shared_ptr<TypeDef> returnType,
        std::vector<std::shared_ptr<Expr>> body,
        std::shared_ptr<TypeDef> ownerType = nullptr
    );

    std::shared_ptr<FunctionArg> GetArg(const std::string &name) const;

    std::shared_ptr<FunctionArg> GetArg(unsigned index) const;

    unsigned GetArgIndex(const std::string &name) const;

    std::string print(unsigned indent = 0) const;
};

