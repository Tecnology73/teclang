#pragma once

#include "BinaryOp.h"

namespace Operations {
    struct AssignOp : public BinaryOp {
        AssignOp(std::shared_ptr<Statements::Statement> lhs, std::shared_ptr<Statements::Statement> rhs) :
            BinaryOp(tok_assign, std::move(lhs), std::move(rhs)) {}
    };
}