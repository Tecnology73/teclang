#include "Function.h"
#include "../core/Debug/Console.h"
#include "statements/IfStatement.h"

Function::Function(
    std::string name,
    std::vector<std::shared_ptr<FunctionArg>> args,
    std::shared_ptr<TypeDef> returnType,
    std::vector<std::shared_ptr<Expr>> body,
    std::shared_ptr<TypeDef> ownerType
) : Name(std::move(name)),
    Args(std::move(args)),
    ReturnType(std::move(returnType)),
    Body(std::move(body)),
    OwnerType(std::move(ownerType)) {
    if (OwnerType != nullptr)
        Args.insert(
            Args.begin(),
            std::make_shared<FunctionArg>(OwnerType, "self", nullptr)
        );

    unsigned i = 0;
    for (const auto &item: Args)
        ArgIndices[item->Name] = i++;
}

std::shared_ptr<FunctionArg> Function::GetArg(const std::string &name) const {
    for (const auto &item: Args)
        if (item->Name == name)
            return item;

    return nullptr;
}

std::shared_ptr<FunctionArg> Function::GetArg(unsigned index) const {
    if (index > Args.size() - 1)
        return nullptr;

    return Args[index];
}

unsigned Function::GetArgIndex(const std::string &name) const {
    auto arg = ArgIndices.find(name);
    if (arg == ArgIndices.end())
        return -1;

    return arg->second;
}

std::string Function::print(unsigned indent) const {
    std::string out = std::string(indent, '\t') + "%bfunc%/";

    if (OwnerType)
        out += "%c<%/%y" + OwnerType->Name + "%/%c>%/";

    out += ' ' + Name + "%c(%/";

    unsigned i = 0;
    for (const auto &item: Args)
        out += (i++ == 0 ? "" : ", ") + item->print(0);

    out += "%c)%/";

    if (ReturnType)
        out += " %m" + ReturnType->Name + "%/";

    out += " %c{%/\n";

    for (const auto &item: Body) {
        // Because I want the debug output to be correctly indented...
        if (std::dynamic_pointer_cast<Statements::IfStatement>(item))
            out += std::string(indent + 1, '\t');

        out += item->print(indent + 1) + '\n';
    }

    out += std::string(indent, '\t') + "%c}%/";

    return out;
}
