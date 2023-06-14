#include "Match.h"
#include "../statements/VariableDecl.h"

namespace Expressions {
    Match::Match(
        std::shared_ptr<Expr> value,
        std::vector<std::shared_ptr<MatchCase>> cases
    ) :
        Value(std::move(value)),
        Cases(std::move(cases)) {}

    std::string Match::print(unsigned indent) const {
        std::string out = std::string(indent, '\t') + "%bmatch%/ %c(%/";
        out += Value->print(0);
        out += "%c) {%/\n";

        for (const auto &item: Cases)
            out += item->print(indent + 1) + '\n';

        out += std::string(indent + 1, '\t') + "%c}%/";
        return out;
    }

    llvm::Value *Match::generate(Generator *generator, const std::shared_ptr<Statements::VariableDecl> &var) {
        auto func = generator->Builder->GetInsertBlock()->getParent();
        auto exitBB = llvm::BasicBlock::Create(*generator->Context, "match.exit", func);
        auto switchInst = generator->Builder->CreateSwitch(
            Value->generate(generator, true),
            exitBB,
            Cases.size()
        );

        // Generate each of the case expressions.
        llvm::BasicBlock *lastBlock;
        for (const auto &item: Cases) {
            lastBlock = item->generate(generator, func, switchInst, var);
            generator->Builder->CreateBr(exitBB);
        }

        // Move the exit block to the end
        exitBB->moveAfter(lastBlock);
        generator->Builder->SetInsertPoint(exitBB);

        return switchInst;
    }
}