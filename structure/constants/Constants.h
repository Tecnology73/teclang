#pragma once

#include <string>
#include <memory>
#include <cassert>
#include "../TypeDef.h"
#include "../statements/Statement.h"

namespace Constants {
    struct ConstantValue : std::enable_shared_from_this<ConstantValue>, Statements::Statement {
        const std::shared_ptr<TypeDef> Type;

        explicit ConstantValue(std::shared_ptr<TypeDef> typeDef) :
            Type(std::move(typeDef)) {}

        virtual ~ConstantValue() = default;

        template<typename T>
        std::shared_ptr<T> castTo() {
            auto ptr = std::dynamic_pointer_cast<T>(shared_from_this());
            assert(ptr != nullptr && "Invalid cast!");
            return ptr;
        }
    };
}
