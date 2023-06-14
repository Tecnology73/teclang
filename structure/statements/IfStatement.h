#pragma once

#include <vector>
#include <memory>
#include "Statement.h"

namespace Statements {
    struct IfStatement : Statement {
        std::shared_ptr<Expr> Condition;
        std::vector<std::shared_ptr<Expr>> Body;
        std::shared_ptr<IfStatement> ElseStatement;

        explicit IfStatement(
                std::shared_ptr<Expr> condition,
                std::vector<std::shared_ptr<Expr>> body,
                std::shared_ptr<IfStatement> elseStatement
        ) :
                Condition(std::move(condition)),
                Body(std::move(body)),
                ElseStatement(std::move(elseStatement)) {}

        std::string print(unsigned indent) const override {
            std::string out;

            if (Condition) {
                out += "%bif%/ %c(%/";
                out += Condition->print(0);
                out += "%c)%/ ";
            }

            out += "%c{%/\n";

            for (const auto &item: Body) {
                if (std::dynamic_pointer_cast<IfStatement>(item))
                    out += std::string(indent + 1, '\t');

                out += item->print(indent + 1) + '\n';
            }

            out += std::string(indent, '\t') + "%c}%/";

            if (ElseStatement)
                out += " %belse%/ " + ElseStatement->print(indent);

            return out;
        }

        llvm::Value *generate(Generator *generator, bool loadAddress) override {
            auto func = generator->Builder->GetInsertBlock()->getParent();
            auto exitBlock = llvm::BasicBlock::Create(*generator->Context, "if.exit");
            auto lastInstruction = generate(generator, func, exitBlock, false);

            func->insert(func->end(), exitBlock);
            generator->Builder->SetInsertPoint(exitBlock);

            return lastInstruction;
        }

        llvm::Value *generate(Generator *generator, llvm::Function *func, llvm::BasicBlock *exitBlock, bool fromChild) {
            auto thenBlock = llvm::BasicBlock::Create(*generator->Context, "if.then");
            auto elseBlock = llvm::BasicBlock::Create(*generator->Context, "if.else");

            // Condition logic
            llvm::Value *conditionValue = nullptr;
            if (Condition) {
                conditionValue = Condition->generate(generator);
                conditionValue = generator->Builder->CreateICmpEQ(
                        conditionValue,
                        llvm::ConstantInt::get(generator->GetCompiledType("bool"), 1)
                );

                generator->Builder->CreateCondBr(conditionValue, thenBlock, !ElseStatement ? exitBlock : elseBlock);
            }

            // Create the `then` body
            if (ElseStatement || !fromChild) {
                func->insert(func->end(), thenBlock);
                generator->Builder->SetInsertPoint(thenBlock);
            }

            llvm::Value *lastValue = nullptr;
            for (const auto &item: Body) {
                lastValue = item->generate(generator);

                if (
                        lastValue &&
                        (llvm::isa<llvm::ReturnInst>(lastValue) || llvm::isa<llvm::BranchInst>(lastValue))
                        )
                    break;
            }

            if (!lastValue || !(llvm::isa<llvm::ReturnInst>(lastValue) || llvm::isa<llvm::BranchInst>(lastValue)))
                generator->Builder->CreateBr(exitBlock);

            // Create the `else` block
            if (ElseStatement) {
                func->insert(func->end(), elseBlock);
                generator->Builder->SetInsertPoint(elseBlock);

                return ElseStatement->generate(generator, func, exitBlock, true);
            }

            return conditionValue;
        }
    };
}