#pragma once

#include <string>
#include <utility>
#include "Reference.h"
#include "ArrayRef.h"
#include "../VariableDecl.h"
#include "../../operations/BinaryOp.h"
#include "../../constants/StringValue.h"
#include "../../types/ArrayTypeDef.h"

namespace Statements {
    namespace References {
        struct VariableRef : std::enable_shared_from_this<VariableRef>, Reference {
            const std::string Name;

            explicit VariableRef(
                std::string name,
                std::shared_ptr<Reference> nextKey = nullptr
            ) :
                Reference(std::move(nextKey)),
                Name(std::move(name)) {}

            std::string print(unsigned indent) const override {
                auto out = "%w" + Name + "%/";

                auto nextOut = Reference::print(0);
                if (!nextOut.empty()) {
                    if (std::dynamic_pointer_cast<Reference>(NextKey))
                        out += ".";

                    out += nextOut;
                }

                return Expr::print(indent, out);
            }

            llvm::Value *generate(Generator *generator, bool loadAddress) override {
                return generateRead(generator, loadAddress);
            }

            llvm::Value *generateRead(Generator *generator, bool loadAddress) {
                Declaration = generator->CurrentScope->GetVar(Name);
                if (!Declaration) {
                    generator->PrintError("Variable \"" + Name + "\" does not exist in the current scope!");
                    return nullptr;
                }

                std::shared_ptr<VariableRef> ref = shared_from_this();
                llvm::Value *prevValue = ref->Declaration->AllocInst;
                while (ref && ref->NextKey) {
                    // Accessing an item in an array - `array[0]`
                    if (auto arrayRef = std::dynamic_pointer_cast<ArrayRef>(ref->NextKey)) {
                        prevValue = generator->Builder->CreateInBoundsGEP(
                            prevValue->getType()->getPointerElementType(),
                            prevValue,
                            // Index of the array
                            arrayRef->Value->generate(generator)
                        );

                        // We're loading the next key here because ArrayRef doesn't inherit VariableRef.
                        // Because of this, the cast below doesn't work.
                        auto nextKey = std::dynamic_pointer_cast<VariableRef>(arrayRef->NextKey);
                        if (nextKey) {
                            auto innerType = std::dynamic_pointer_cast<ArrayTypeDef>(ref->Declaration->Type)->InnerType;
                            prevValue = generator->Builder->CreateStructGEP(
                                prevValue->getType()->getPointerElementType(),
                                prevValue,
                                innerType->GetFieldIndex(nextKey->Name),
                                nextKey->Name
                            );
                        }

                        ref = nextKey;
                        continue;
                    }

                    auto nextRef = std::dynamic_pointer_cast<VariableRef>(ref->NextKey);
                    // Accessing the length of an array - `array.length`
                    // TODO: This is a hacky way of doing this.
                    //       I should figure out a way to actually implement the Array type.
                    if (nextRef->Name == "length") {
                        if (auto arrayType = std::dynamic_pointer_cast<ArrayTypeDef>(ref->Declaration->Type)) {
                            auto arrayCount = ref->Declaration->ConstructorArgs[0]->generate(generator);
                            if (arrayCount->getType()->isPointerTy()) {
                                prevValue = generator->Builder->CreateLoad(
                                    arrayCount->getType()->getPointerElementType(),
                                    arrayCount
                                );
                            } else {
                                prevValue = arrayCount;
                            }
                        }
                    } else {
                        nextRef->Declaration = ref->Declaration->Type->GetField(nextRef->Name);
                        prevValue = generator->Builder->CreateStructGEP(
                            prevValue->getType()->getPointerElementType(),
                            prevValue,
                            ref->Declaration->Type->GetFieldIndex(nextRef->Name),
                            nextRef->Name
                        );
                    }

                    ref = nextRef;
                }

                if (loadAddress && prevValue->getType()->isPointerTy()) {
                    prevValue = generator->Builder->CreateLoad(
                        prevValue->getType()->getPointerElementType(),
                        prevValue
                    );
                }

                return prevValue;
            }

            /*
             * Operations
             */

            llvm::Value *add(Generator *generator, const std::shared_ptr<Expr> &rhs) override {
                return nullptr;

                /*auto value = generateWithRef(generator);
                auto ref = std::static_pointer_cast<VariableRef>(std::get<1>(value));
                if (auto func = ref->Declaration->Type->GetCompiledFunction("+=")) {
                    return generator->Builder->CreateCall(
                        func,
                        {
                            std::get<0>(value),
                            rhs->generate(generator)
                        }
                    );
                }

                auto v = std::static_pointer_cast<VariableRef>(std::get<1>(value));
                return generator->Builder->CreateStore(
                    generator->Builder->CreateAdd(
                        std::get<0>(value),
                        rhs->generate(generator)
                    ),
                    v->Declaration->AllocInst
                );*/
            }
        };
    }
}

