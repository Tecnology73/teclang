#include "BinaryOp.h"
#include "../statements/references/VariableRef.h"
#include "../../core/Debug.h"
#include "../expressions/Match.h"

namespace Operations {
    BinaryOp::BinaryOp(
        Token opCode,
        std::shared_ptr<Expr> lhs,
        std::shared_ptr<Expr> rhs
    ) :
        OpCode(opCode),
        LHS(std::move(lhs)),
        RHS(std::move(rhs)) {}

    std::string BinaryOp::print(unsigned int indent) const {
        return Expr::print(
            indent,
            "%c(%/" + LHS->print(0) +
            " %c" + Debug::PrintToken(OpCode) + "%/ " +
            RHS->print(0) + "%c)%/"
        );
    }

    llvm::Value *BinaryOp::generate(Generator *generator, bool loadAddress) {
        switch (OpCode) {
            case tok_assign:
            case tok_colon: {
                // TODO: Clean this fucking shit up...
                auto destAddress = LHS->generate(generator, false);
                if (auto ref = std::dynamic_pointer_cast<Statements::References::VariableRef>(LHS)) {
                    if (auto var = generator->CurrentScope->GetVar(ref->Name)) {
                        if (var->Type->Name == "string") {
                            auto value = RHS->generate(generator);
                            return generator->Builder->CreateCall(
                                generator->Module->getFunction("string_Assign"),
                                {
                                    var->AllocInst,
                                    value
                                }
                            );
                        }

                        if (auto match = std::dynamic_pointer_cast<Expressions::Match>(RHS))
                            return match->generate(generator, var);
                    }
                }

                return generator->Builder->CreateStore(
                    RHS->generate(generator),
                    destAddress
                );
            }
                // case tok_plusplus: // The parser converts this to `+= 1`
            case tok_plus_equal: {
                return LHS->add(generator, RHS);
            }
                // case tok_minusminus: // The parser converts this to `-= 1`
            case tok_minus_equal: {
                auto destAddress = LHS->generate(generator, false);
                auto result = generator->Builder->CreateSub(
                    generator->Builder->CreateLoad(
                        destAddress->getType()->getPointerElementType(),
                        destAddress
                    ),
                    RHS->generate(generator)
                );

                return generator->Builder->CreateStore(result, destAddress);
            }
            case tok_multi_equal: {
                auto destAddress = LHS->generate(generator, false);
                auto result = generator->Builder->CreateMul(
                    generator->Builder->CreateLoad(
                        destAddress->getType()->getPointerElementType(),
                        destAddress
                    ),
                    RHS->generate(generator)
                );

                return generator->Builder->CreateStore(result, destAddress);
            }
            case tok_divide_equal: {
                auto destAddress = LHS->generate(generator, false);
                // TODO: CreateSDiv/CreateUDiv???
                auto result = generator->Builder->CreateFDiv(
                    generator->Builder->CreateLoad(
                        destAddress->getType()->getPointerElementType(),
                        destAddress
                    ),
                    RHS->generate(generator)
                );

                return generator->Builder->CreateStore(result, destAddress);
            }
            case tok_plus: {
                return generator->Builder->CreateAdd(
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_minus: {
                return generator->Builder->CreateSub(
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_multiply: {
                return generator->Builder->CreateMul(
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_divide: {
                // TODO: CreateSDiv/CreateUDiv???
                return generator->Builder->CreateFDiv(
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_less_than: {
                return generator->Builder->CreateCmp(
                    llvm::CmpInst::Predicate::ICMP_SLT,
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_lt_equal: {
                return generator->Builder->CreateCmp(
                    llvm::CmpInst::Predicate::ICMP_SLE,
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_greater_than: {
                return generator->Builder->CreateCmp(
                    llvm::CmpInst::Predicate::ICMP_SGT,
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_gt_equal: {
                return generator->Builder->CreateCmp(
                    llvm::CmpInst::Predicate::ICMP_SGE,
                    LHS->generate(generator),
                    RHS->generate(generator)
                );
            }
            case tok_question_mark: {
                auto ternary = std::dynamic_pointer_cast<BinaryOp>(RHS);
                if (!ternary || ternary->OpCode != tok_colon) {
                    generator->PrintError("Expected ternary operator");
                    return nullptr;
                }

                auto cond = LHS->generate(generator);

                return nullptr;
            }
        }

        return nullptr;
    }

    llvm::Value *BinaryOp::generate(
        Generator *generator,
        const std::shared_ptr<Statements::VariableDecl> &prevRef,
        llvm::Value *prevValue
    ) const {
        if (prevRef->Type->IsUserDefined) {
            auto lhsVar = std::static_pointer_cast<Statements::References::VariableRef>(LHS);
            auto i = prevRef->Type->GetFieldIndex(lhsVar->Name);
            auto lhsAddr = generator->Builder->CreateInBoundsGEP(
                prevValue->getType()->getPointerElementType(),
                prevValue,
                {
                    llvm::ConstantInt::get(generator->GetCompiledType("int"), 0),
                    llvm::ConstantInt::get(generator->GetCompiledType("int32"), i),
                }
            );

            return generator->Builder->CreateStore(
                RHS->generate(generator),
                lhsAddr
            );
        }

        return generator->Builder->CreateStore(
            RHS->generate(generator),
            prevValue
        );
    }
}