#include "TypeDef.h"
#include "Function.h"
#include "../generators/Function.h"
#include "../core/debug/Console.h"


TypeDef::TypeDef(std::string name) :
    Name(std::move(name)) {}

TypeDef::TypeDef(std::string name, bool isPointer) :
    Name(std::move(name)),
    IsPointer(isPointer) {}

TypeDef::TypeDef(
    std::string name,
    std::vector<std::shared_ptr<Statements::VariableDecl>> fields,
    std::vector<std::shared_ptr<TypeDef>> derivedTypes
) : Name(std::move(name)),
    Fields(std::move(fields)),
    DerivedTypes(std::move(derivedTypes)),
    IsDeclared(true) {
    unsigned i = 0;
    for (const auto &item: Fields)
        FieldIndices[item->Name] = i++;
}

std::string TypeDef::print(unsigned int indent, bool onlyType) const {
    if (onlyType)
        return std::string(indent, '\t') + Console::PrintColoredType(Name);

    std::string out = std::string(indent, '\t') + "%btype%/ ";
    out += Console::PrintColoredType(Name) + " %c{%/\n";

    for (const auto &item: Fields)
        out += item->print(indent + 1) + '\n';

    out += std::string(indent, '\t') + "%c}%/";

    return out;
}

llvm::Type *TypeDef::generate(Generator *generator) const {
    if (auto compiledType = generator->GetCompiledType(Name))
        return compiledType;

    auto type = Generators::TypeDef::Generate(generator, this);

    generateFunctions(generator);

    return type;
}

void TypeDef::generateFunctions(Generator *generator) const {
    for (const auto &function: Functions)
        Generators::Function::Generate(generator, function.second);
}

bool TypeDef::AddFunction(const std::shared_ptr<Function> &function) {
    if (Functions.count(function->Name) != 0)
        return false;

    Functions[function->Name] = function;
    return true;
}

std::shared_ptr<Function> TypeDef::GetFunction(const std::string &name) const {
    auto func = Functions.find(name);
    if (func == Functions.end())
        return nullptr;

    return func->second;
}

llvm::Function *TypeDef::GetCompiledFunction(const std::string &name) const {
    auto func = CompiledFunctions.find(name);
    if (func == CompiledFunctions.end())
        return nullptr;

    return func->second;
}

std::shared_ptr<TypeDef> TypeDef::Clone() const {
    auto cloned = std::make_shared<TypeDef>(
        Name,
        Fields,
        DerivedTypes
    );
    cloned->IsPointer = IsPointer;

    return cloned;
}

std::shared_ptr<TypeDef> TypeDef::CloneOnto(std::shared_ptr<TypeDef> typeDef) const {
    typeDef->Fields = Fields;
    typeDef->FieldIndices = FieldIndices;
    typeDef->DerivedTypes = DerivedTypes;
    typeDef->IsPointer = IsPointer;
    typeDef->IsDeclared = IsDeclared;
    typeDef->IsUserDefined = IsUserDefined;
    typeDef->IsComplexType = IsComplexType;

    return typeDef;
}

bool TypeDef::AddField(const std::shared_ptr<Statements::VariableDecl> &field) {
    if (FieldIndices.count(field->Name) != 0)
        return false;

    Fields.push_back(field);
    FieldIndices[field->Name] = FieldIndices.size();
    return true;
}

std::shared_ptr<Statements::VariableDecl> TypeDef::GetField(const std::string &name) const {
    auto field = FieldIndices.find(name);
    if (field == FieldIndices.end())
        return nullptr;

    return Fields[field->second];
}

std::shared_ptr<Statements::VariableDecl> TypeDef::GetField(unsigned int index) const {
    if (index > Fields.size() - 1)
        return nullptr;

    return Fields[index];
}

int TypeDef::GetFieldIndex(const std::string &name) const {
    auto field = FieldIndices.find(name);
    if (field == FieldIndices.end())
        return -1;

    return field->second;
}