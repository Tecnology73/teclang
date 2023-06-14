#include "EnumTypeDef.h"
#include "../../core/debug/Console.h"

std::string EnumTypeDef::print(unsigned int indent, bool onlyType) const {
    if (onlyType)
        return std::string(indent, '\t') + "%b" + Name + "%/";

    std::string out = std::string(indent, '\t') + "%benum%/ ";
    out += Console::PrintColoredType(Name) + " %c{%/\n";

    for (const auto &item: Fields)
        out += item->print(indent + 1) + '\n';

    out += std::string(indent, '\t') + "%c}%/";

    return out;
}