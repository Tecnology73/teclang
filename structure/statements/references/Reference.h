#pragma once

#include <string>
#include <utility>
#include <memory>
#include "../Statement.h"

namespace Statements {
    namespace References {
        /**
         * Describes a variable that references a declaration.
         * For example, if you have:
         * ```
         * int number(10);
         * print(number);
         * ```
         * The `number` argument on line 2 will be a *reference* to the `number` variable
         * declared on line 1.
         */
        struct Reference : Statement {
            std::shared_ptr<Reference> NextKey;
            std::shared_ptr<const VariableDecl> Declaration;

            explicit Reference(std::shared_ptr<Reference> nextKey) :
                NextKey(std::move(nextKey)) {}

            std::string print(unsigned indent) const override {
                if (!NextKey)
                    return "";

                return NextKey->print(indent);
            }
        };
    }
}

