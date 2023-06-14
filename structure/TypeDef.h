#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Expr.h"
#include "./statements/VariableDecl.h"

struct Function;

struct TypeDef : std::enable_shared_from_this<TypeDef> {
    const std::string Name;
    std::vector<std::shared_ptr<Statements::VariableDecl>> Fields;
    std::map<std::string, int> FieldIndices;
    std::vector<std::shared_ptr<TypeDef>> DerivedTypes;
    std::map<std::string, std::shared_ptr<Function>> Functions;
    std::map<std::string, llvm::Function *> CompiledFunctions;
    bool IsPointer = false;
    bool IsDeclared = false;
    bool IsUserDefined = false;
    // Used for just String & Array types for now.
    bool IsComplexType = false;

    TypeDef() = default;

    explicit TypeDef(std::string name);

    TypeDef(std::string name, bool isPointer);

    TypeDef(
        std::string name,
        std::vector<std::shared_ptr<Statements::VariableDecl>> fields,
        std::vector<std::shared_ptr<TypeDef>> derivedTypes
    );

    virtual ~TypeDef() = default;

    virtual std::string print(unsigned indent = 0, bool onlyType = false) const;

    virtual llvm::Type *generate(Generator *generator) const;

    void generateFunctions(Generator *generator) const;

    bool AddFunction(const std::shared_ptr<Function> &function);

    std::shared_ptr<Function> GetFunction(const std::string &name) const;

    llvm::Function *GetCompiledFunction(const std::string &name) const;

    std::shared_ptr<TypeDef> Clone() const;

    std::shared_ptr<TypeDef> CloneOnto(std::shared_ptr<TypeDef> typeDef) const;

    bool AddField(const std::shared_ptr<Statements::VariableDecl> &field);

    std::shared_ptr<Statements::VariableDecl> GetField(const std::string &name) const;

    std::shared_ptr<Statements::VariableDecl> GetField(unsigned index) const;

    int GetFieldIndex(const std::string &name) const;
};
