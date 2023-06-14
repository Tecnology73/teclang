#include "Context.h"
#include "../structure/Function.h"

std::shared_ptr<Context> const Context::Instance = std::make_shared<Context>();

bool Context::AddDeclaredType(const std::shared_ptr<TypeDef> &typeDef, bool isDeclaration) {
    auto type = GetDeclaredType(typeDef->Name);
    if (type && type->IsDeclared)
        return false;

    if (isDeclaration)
        typeDef->IsDeclared = true;

    // The function is only called for user-defined types.
    // Unless this changes, this is perfectly fine to do (maybe?).
    typeDef->IsUserDefined = true;
    // All User defined types are "complex"
    typeDef->IsComplexType = true;

    DeclaredTypes[typeDef->Name] = typeDef;
    return true;
}

std::shared_ptr<TypeDef> Context::GetDeclaredType(const std::string &name) const {
    auto type = DeclaredTypes.find(name);
    if (type == DeclaredTypes.end())
        return nullptr;

    return type->second;
}

bool Context::HasDeclaredType(const std::string &name) const {
    auto type = GetDeclaredType(name);

    return type == nullptr || !type->IsDeclared;
}

bool Context::AddFunction(const std::shared_ptr<Function> &function) {
    if (DeclaredFunctions.count(function->Name) != 0)
        return false;

    DeclaredFunctions[function->Name] = function;
    return true;
}

std::shared_ptr<Function> Context::GetFunction(const std::string &name) const {
    auto function = DeclaredFunctions.find(name);
    if (function == DeclaredFunctions.end())
        return nullptr;

    return function->second;
}
