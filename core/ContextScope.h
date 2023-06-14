#pragma once

#include <string>
#include <map>
#include <memory>
#include <utility>

namespace Statements { struct VariableDecl; }

struct ContextScope {
    std::map<std::string, std::shared_ptr<Statements::VariableDecl>> Variables;
    llvm::BasicBlock *LoopTailLabel = nullptr;
    llvm::BasicBlock *LoopExitLabel = nullptr;
    std::shared_ptr<ContextScope> ParentScope;

    bool AddVar(const std::string &name, std::shared_ptr<Statements::VariableDecl> var) {
        if (HasVar(name))
            return false;

        Variables[name] = std::move(var);
        return true;
    }

    std::shared_ptr<Statements::VariableDecl> GetVar(const std::string &name) const {
        auto var = Variables.find(name);
        if (var == Variables.end()) {
            if (ParentScope)
                return ParentScope->GetVar(name);

            return nullptr;
        }

        return var->second;
    }

    bool HasVar(const std::string &name) const {
        return GetVar(name) != nullptr;
    }

    llvm::BasicBlock *GetLoopTailLabel() const {
        if (LoopTailLabel)
            return LoopTailLabel;

        if (ParentScope)
            return ParentScope->GetLoopTailLabel();

        return nullptr;
    }

    llvm::BasicBlock *GetLoopExitLabel() const {
        if (LoopExitLabel)
            return LoopExitLabel;

        if (ParentScope)
            return ParentScope->GetLoopExitLabel();

        return nullptr;
    }
};
