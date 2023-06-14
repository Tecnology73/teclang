#pragma once

#include <vector>
#include <memory>
#include "Statement.h"
#include "../operations/BinaryOp.h"
#include "../../core/Debug.h"
#include "VariableDecl.h"
#include "Break.h"

namespace Statements {
    struct ForStatement : Statement {
        std::shared_ptr<Statements::VariableDecl> Head;
        std::shared_ptr<Operations::BinaryOp> Condition;
        std::shared_ptr<Expr> Tail;
        std::vector<std::shared_ptr<Expr>> Body;

        ForStatement(
            std::shared_ptr<Statements::VariableDecl> head,
            std::shared_ptr<Operations::BinaryOp> condition,
            std::shared_ptr<Expr> tail,
            std::vector<std::shared_ptr<Expr>> body
        ) :
            Head(std::move(head)),
            Condition(std::move(condition)),
            Tail(std::move(tail)),
            Body(std::move(body)) {}

        std::string print(unsigned indent) const override {
            std::string out = "%bfor%/ %c(%/";
            out += Head->print(0);

            if (Condition->OpCode == tok_spread) {
                // Initial value
                out += " %c=%/ " + Condition->LHS->print(0) + "%c;%/ ";

                // Condition
                out += "%w" + Head->Name + "%/";
                out += " %c" + Debug::PrintToken(tok_less_than) + "%/ ";
                out += Condition->RHS->print(0) + "%c;%/ ";

                // Tail
                if (Tail)
                    out += Tail->print(0);
                else
                    out += Head->Name + "%c++%/";
            } else
                return nullptr;

            out += "%c) {%/\n";

            for (const auto &item: Body)
                out += item->print(indent + 1) + '\n';

            out += std::string(indent, '\t') + "%c}%/";

            return Expr::print(indent, out);
        }

        llvm::Value *generate(Generator *generator, bool loadAddress) override {
            generator->OpenScope();
            auto func = generator->Builder->GetInsertBlock()->getParent();

            auto conditionBB = llvm::BasicBlock::Create(*generator->Context, "for.cond", func);
            auto bodyBB = llvm::BasicBlock::Create(*generator->Context, "for.body", func);
            auto tailBB = llvm::BasicBlock::Create(*generator->Context, "for.tail", func);
            auto exitBB = llvm::BasicBlock::Create(*generator->Context, "for.exit", func);

            generator->CurrentScope->LoopTailLabel = tailBB;

            // Generate the head
            auto var = Head->generate(generator);
            generator->Builder->CreateStore(Condition->LHS->generate(generator), var);

            // Generate the condition
            generator->Builder->CreateBr(conditionBB);
            generator->Builder->SetInsertPoint(conditionBB);
            auto cond = generator->Builder->CreateICmpSLT(
                generator->Builder->CreateLoad(
                    var->getType()->getPointerElementType(),
                    var
                ),
                Condition->RHS->generate(generator)
            );
            generator->Builder->CreateCondBr(cond, bodyBB, exitBB);

            // Generate the body
            generator->Builder->SetInsertPoint(bodyBB);
            generator->CurrentScope->LoopExitLabel = exitBB;

            llvm::Value *lastValue = nullptr;
            for (const auto &item: Body) {
                lastValue = item->generate(generator);
                if (lastValue && (llvm::isa<llvm::ReturnInst>(lastValue) || llvm::isa<llvm::BranchInst>(lastValue)))
                    break;
            }

            generator->CurrentScope->LoopExitLabel = nullptr;

            // Generate the tail
            if (!lastValue || !(llvm::isa<llvm::ReturnInst>(lastValue) || llvm::isa<llvm::BranchInst>(lastValue)))
                generator->Builder->CreateBr(tailBB);

            generator->Builder->SetInsertPoint(tailBB);
            if (Tail)
                Tail->generate(generator);
            else
                generator->Builder->CreateStore(
                    generator->Builder->CreateAdd(
                        generator->Builder->CreateLoad(
                            var->getType()->getPointerElementType(),
                            var
                        ),
                        generator->Builder->getInt64(1)
                    ),
                    var
                );

            generator->CurrentScope->LoopTailLabel = nullptr;
            generator->Builder->CreateBr(conditionBB);
            generator->Builder->SetInsertPoint(exitBB);
            generator->CloseScope();

            return nullptr;
        }
    };
}