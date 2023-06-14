#include "./VariableDecl.h"
#include "../TypeDef.h"
#include "../../generators/Constructor.h"
#include "../expressions/Match.h"
#include "../types/ArrayTypeDef.h"
#include "../operations/BinaryOp.h"
#include "references/VariableRef.h"

namespace Statements {
    VariableDecl::VariableDecl(
        std::shared_ptr<TypeDef> type,
        std::string name,
        std::vector<std::shared_ptr<Expr>> constructorArgs,
        bool callConstruct
    ) :
        Type(std::move(type)),
        Name(std::move(name)),
        ConstructorArgs(std::move(constructorArgs)),
        CallConstruct(callConstruct) {}

    std::string VariableDecl::print(unsigned indent) const {
        std::string out = Type->print(0, true) + " %w" + Name + "%/";

        for (const auto &arg: ConstructorArgs)
            out += "%c(%/" + arg->print(0) + "%c)%/";

        return Expr::print(indent, out);
    }

    llvm::Value *VariableDecl::generate(Generator *generator, bool loadAddress) {
        if (!Type->IsDeclared)
            return nullptr;

        // Declare an array
        if (auto arrayType = std::dynamic_pointer_cast<ArrayTypeDef>(Type)) {
            auto arraySize = ConstructorArgs[0]->generate(generator);
            // auto arraySize = std::dynamic_pointer_cast<Constants::IntValue>(ConstructorArgs[0]);
            AllocInst = generator->Builder->CreateAlloca(
                /*llvm::ArrayType::get(
                    arrayType->InnerType->generate(generator),
                    arraySize
                ),
                nullptr,*/
                arrayType->InnerType->generate(generator),
                arraySize,
                Name
            );

            generator->CurrentScope->AddVar(Name, shared_from_this());

            return AllocInst;
        }

        auto type = Type->generate(generator);

        if (Type->IsComplexType) {
            if (CallConstruct) {
                auto constructor = Generators::Constructor::Generate(generator, Type);
                std::vector<std::shared_ptr<Operations::BinaryOp>> assignments;
                std::vector<llvm::Value *> args;
                args.reserve(ConstructorArgs.size());

                for (const auto &arg: ConstructorArgs) {
                    if (auto op = std::dynamic_pointer_cast<Operations::BinaryOp>(arg)) {
                        // Invalid token
                        if (op->OpCode != tok_colon) return nullptr;

                        assignments.push_back(op);
                        continue;
                    }

                    args.push_back(arg->generate(generator));
                }

                AllocInst = generator->Builder->CreateCall(constructor, args);

                for (const auto &assign: assignments) {
                    auto varRef = std::dynamic_pointer_cast<References::VariableRef>(assign->LHS);
                    auto fieldIndex = Type->GetFieldIndex(varRef->Name);
                    if (fieldIndex == -1) {
                        // Field doesn't exist on the Type.
                        return nullptr;
                    }

                    auto field = generator->Builder->CreateStructGEP(
                        AllocInst->getType()->getPointerElementType(),
                        AllocInst,
                        fieldIndex,
                        varRef->Name
                    );

                    generator->Builder->CreateStore(
                        assign->RHS->generate(generator),
                        field
                    );
                }
            }
        } else {
            AllocInst = generator->Builder->CreateAlloca(type, nullptr, Name);

            // Simple types (bool/int) can only have a single argument passed to their constructor (for now ;D)
            if (!ConstructorArgs.empty()) {
                if (auto match = std::dynamic_pointer_cast<Expressions::Match>(ConstructorArgs[0]))
                    match->generate(generator, shared_from_this());
                else
                    generator->Builder->CreateStore(ConstructorArgs[0]->generate(generator), AllocInst);
            }
        }

        generator->CurrentScope->AddVar(Name, shared_from_this());

        return AllocInst;
    }
}