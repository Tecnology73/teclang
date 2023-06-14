#include "StringValue.h"
#include "../../core/Context.h"
#include "../../core/Generator.h"

namespace Constants {
    StringValue::StringValue(std::string value) :
        ConstantValue(Context::Instance->GetDeclaredType("string")),
        Value(std::move(value)) {}

    std::string StringValue::print(unsigned indent) const {
        std::string serializedString(Value);
        size_t i;
        while ((i = serializedString.find('\n')) != std::string::npos)
            serializedString.replace(i, i + 1, "\\n");

        return Expr::print(
            indent,
            "%l\"" + serializedString + "\"%/"
        );
    }

    llvm::Value *StringValue::generate(Generator *generator, bool loadAddress) {
        auto ptr = generator->Builder->CreateGlobalStringPtr(
            Value,
            "",
            0
        );

        // Cast i8* to i64*
        return generator->Builder->CreateBitCast(
            ptr,
            generator->GetCompiledType("int8*"),
            "strAlloc"
        );
    }
}