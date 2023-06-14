#pragma once

#include <string>
#include <utility>
#include "Reference.h"

namespace Statements {
    namespace References {
        struct ArrayRef : Reference {
            const std::shared_ptr<Expr> Value;

            explicit ArrayRef(std::shared_ptr<Expr> value, std::shared_ptr<Reference> nextKey = nullptr) :
                Reference(std::move(nextKey)),
                Value(std::move(value)) {}

            std::string print(unsigned indent) const override {
                auto out = std::string(indent, '\t');
                out += "%c[%/" + Value->print(0) + "%c]%/";

                auto nextOut = Reference::print(indent);
                if (!nextOut.empty())
                    out += "." + nextOut;

                return Expr::print(indent, out);
            }
        };
    }
}

