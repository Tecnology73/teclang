#pragma once

#include <string>
#include <map>
#include <memory>
#include <iostream>
#include "../structure/TypeDef.h"
#include "ContextScope.h"
#include "../structure/types/IntTypeDef.h"

struct Context {
public:
    static const std::shared_ptr<Context> Instance;

    // Includes all built-in + user-defined types
    std::map<std::string, std::shared_ptr<TypeDef>> DeclaredTypes = {
        // Built-in types
        {"int8",    std::make_shared<IntTypeDef>(8)},
        {"int16",   std::make_shared<IntTypeDef>(16)},
        {"int32",   std::make_shared<IntTypeDef>(32)},
        {"int64",   std::make_shared<IntTypeDef>(64)},
        {"int",     std::make_shared<IntTypeDef>(64)},
        {"uint8",   std::make_shared<IntTypeDef>(8, true)},
        {"uint16",  std::make_shared<IntTypeDef>(16, true)},
        {"uint32",  std::make_shared<IntTypeDef>(32, true)},
        {"uint64",  std::make_shared<IntTypeDef>(64, true)},
        {"uint",    std::make_shared<IntTypeDef>(64, true)},
        {"double",  std::make_shared<TypeDef>("double")},
        {"bool",    std::make_shared<TypeDef>("bool")},
        {"void",    std::make_shared<TypeDef>("void")},
        // Pointer variants
        {"int8*",   std::make_shared<IntTypeDef>(8, false, true)},
        {"int16*",  std::make_shared<IntTypeDef>(16, false, true)},
        {"int32*",  std::make_shared<IntTypeDef>(32, false, true)},
        {"int64*",  std::make_shared<IntTypeDef>(64, false, true)},
        {"int*",    std::make_shared<IntTypeDef>(64, false, true)},
        {"uint8*",  std::make_shared<IntTypeDef>(8, true, true)},
        {"uint16*", std::make_shared<IntTypeDef>(16, true, true)},
        {"uint32*", std::make_shared<IntTypeDef>(32, true, true)},
        {"uint64*", std::make_shared<IntTypeDef>(64, true, true)},
        {"uint*",   std::make_shared<IntTypeDef>(64, true, true)},
        {"void*",   std::make_shared<TypeDef>("void*", true)},
    };
    // Includes all functions that do not belong to a type (aka, all non-member functions)
    std::map<std::string, std::shared_ptr<Function>> DeclaredFunctions;

public:
    bool AddDeclaredType(const std::shared_ptr<TypeDef> &typeDef, bool isDeclaration = false);

    std::shared_ptr<TypeDef> GetDeclaredType(const std::string &name) const;

    bool HasDeclaredType(const std::string &name) const;

    bool AddFunction(const std::shared_ptr<Function> &function);

    std::shared_ptr<Function> GetFunction(const std::string &name) const;
};