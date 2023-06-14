#pragma once

#include <utility>
#include <vector>
#include <memory>
#include "VariableRef.h"

namespace Statements {
    namespace References {
        struct FunctionCall : Reference {
            std::string FunctionName;
            std::vector<std::shared_ptr<Expr>> Arguments;

            explicit FunctionCall(
                std::string functionName,
                std::vector<std::shared_ptr<Expr>> arguments,
                std::shared_ptr<Reference> nextKey = nullptr
            ) :
                Reference(std::move(nextKey)),
                FunctionName(std::move(functionName)),
                Arguments(std::move(arguments)) {}

            std::string print(unsigned indent) const override {
                std::string out = FunctionName + "%c(%/";

                unsigned i = 0;
                for (const auto &item: Arguments)
                    out += (i++ == 0 ? "" : ", ") + item->print(0);

                out += "%c)%/";

                return Expr::print(indent, out);
            }

            llvm::Value *generate(Generator *generator, bool loadAddress = true) override {
                auto func = generator->Module->getFunction(FunctionName);
                std::vector<llvm::Value *> args;
                for (const auto &arg: Arguments) {
                    if (auto ref = std::dynamic_pointer_cast<VariableRef>(arg)) {
                        auto value = ref->generate(generator, true);
                        auto type = value->getType();
                        // A disgusting hack to allow for types that wrap simple types (for ex. a string).
                        if (type->getNumContainedTypes() > 0 && type->getContainedType(0)->isPointerTy()) {
                            value = generator->Builder->CreateLoad(
                                value->getType()->getPointerElementType(),
                                value
                            );
                        }

                        args.push_back(value);
                        continue;
                    }

                    args.push_back(arg->generate(generator));
                }

                return generator->Builder->CreateCall(
                    func,
                    args,
                    FunctionName
                );
            }
        };
    }
}