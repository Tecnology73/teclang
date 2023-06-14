#include <utility>
#include "ArrayTypeDef.h"
#include "../Function.h"
#include "../../core/debug/Console.h"
#include "../../generators/TypeDef.h"

ArrayTypeDef::ArrayTypeDef(std::shared_ptr<TypeDef> innerType, std::shared_ptr<Expr> length) :
    TypeDef("Array"),
    InnerType(std::move(innerType)),
    Length(std::move(length)) {}

std::string ArrayTypeDef::print(unsigned int indent, bool onlyType) const {
    if (!onlyType)
        return TypeDef::print(indent, onlyType);

    std::string out = std::string(indent, '\t');
    out += Console::PrintColoredType(Name);
    out += "%c<%/" + InnerType->print(indent, true) + "%c>%/";

    return out;
}

llvm::Type *ArrayTypeDef::generate(Generator *generator) const {
    return TypeDef::generate(generator);
}
