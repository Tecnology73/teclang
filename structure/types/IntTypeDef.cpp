#include "IntTypeDef.h"
#include "../../generators/TypeDef.h"

std::string IntTypeDef::GetRealName() const {
    std::string realName;
    if (IsUnsigned) realName += "u";
    realName += "int";
    if (BitLength != 64) realName += std::to_string(BitLength);
    if (IsPointer) realName += '*';

    return realName;
}

std::string IntTypeDef::print(unsigned int indent, bool onlyType) const {
    auto out = std::string(indent, '\t');
    out += "%m";
    if (IsUnsigned) out += "u";
    out += "int";
    if (BitLength != 64) out += std::to_string(BitLength);
    out += "%/";

    return out;
}

llvm::Type *IntTypeDef::generate(Generator *generator) const {
    if (auto compiledType = generator->GetCompiledType(GetRealName()))
        return compiledType;

    auto type = Generators::TypeDef::Generate(generator, this);

    generateFunctions(generator);

    return type;
}