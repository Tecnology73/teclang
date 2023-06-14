#include "./MatchCase.h"
#include "../statements/VariableDecl.h"
#include "../statements/Return.h"
#include "../../generators/Function.h"
#include "../../core/Context.h"

namespace Expressions {
    llvm::BasicBlock *MatchCase::generate(
        Generator *generator,
        llvm::Function *func,
        llvm::SwitchInst *switchInst,
        const std::shared_ptr<Statements::VariableDecl> &var
    ) {
        // Just generate a function that returns the value of the case.
        // This is easier than trying to capture each of the return statements ourselves.
        // We can just let LLVM optimize this out if possible.
        auto caseFunc = std::make_shared<Function>(
            "match_case_block",
            std::vector<std::shared_ptr<FunctionArg>>(),
            Context::Instance->GetDeclaredType("int"),
            Body
        );
        auto caseFuncGen = Generators::Function::Generate(generator, caseFunc);
        caseFuncGen->addFnAttr(llvm::Attribute::InlineHint);

        // Generate the case block.
        generator->OpenScope();

        auto body = llvm::BasicBlock::Create(*generator->Context, "case", func);
        generator->Builder->SetInsertPoint(body);

        // Call the match block function and store the result in the variable.
        auto caseCall = generator->Builder->CreateCall(caseFuncGen, {});
        generator->Builder->CreateStore(caseCall, var->AllocInst);

        // Map all the case values to this block.
        for (const auto &item: Values)
            switchInst->addCase(
                static_cast<llvm::ConstantInt *>(item->generate(generator)),
                body
            );

        if (IsDefault) {
            body->setName("default");
            switchInst->setDefaultDest(body);
        }

        generator->CloseScope();
        return body;
    }
}