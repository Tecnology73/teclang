#pragma once

#include "../Expr.h"

namespace Expressions {
    struct MatchCase : Expr {
        std::vector<std::shared_ptr<Expr>> Values;
        std::vector<std::shared_ptr<Expr>> Body;
        bool IsDefault;

        explicit MatchCase(
            std::vector<std::shared_ptr<Expr>> values,
            std::vector<std::shared_ptr<Expr>> body,
            bool isDefault = false
        ) :
            Values(std::move(values)),
            Body(std::move(body)),
            IsDefault(isDefault) {}

        std::string print(unsigned indent) const override {
            std::string out = std::string(indent, '\t');
            if (!Values.empty() && Values[0] != nullptr) {
                out += std::string(indent, '\t') + "%bcase%/ %c(%/";
                for (unsigned i = 0; i < Values.size(); i++) {
                    out += Values[i]->print(0);
                    if (i != Values.size() - 1)
                        out += "%c, %/";
                }

                out += "%c) => {%/\n";
            } else
                out += std::string(indent, '\t') + "%bdefault%/%c => {%/\n";

            for (const auto &item: Body)
                out += item->print(indent + 2) + '\n';

            out += std::string(indent + 1, '\t') + "%c}%/";
            return out;
        }

        llvm::BasicBlock *generate(
            Generator *generator,
            llvm::Function *func,
            llvm::SwitchInst *switchInst,
            const std::shared_ptr<Statements::VariableDecl> &var
        );
    };
}