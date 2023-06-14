#include "../bak/Context.h"
#include "../core/funcs.cpp"

using namespace llvm;

llvm::Value *createString(const std::string &val) {
    llvm::Constant *strConstant = llvm::ConstantDataArray::getString(*Context::getInstance()->LLVMContext, val);
    llvm::GlobalVariable *GVStr = new llvm::GlobalVariable(
            *Context::getInstance()->MainModule,
            strConstant->getType(),
            true,
            llvm::GlobalValue::PrivateLinkage,
            strConstant
    );
    llvm::Constant *zero = llvm::Constant::getNullValue(Context::getCompiledType("int"));
    llvm::Constant *indices[] = {zero, zero};
    llvm::Constant *strVal = llvm::ConstantExpr::getGetElementPtr(
            strConstant->getType(),
            GVStr,
            indices,
            true
    );

    return strVal;
}

void put(const std::string &format) {
    Context::getBuilder()->CreateCall(
            Context::getInstance()->MainModule->getFunction("printf"),
            {
                    createString(format),
            }
    );
}

void put(const std::string &format, std::vector<llvm::Value *> val) {
    val.insert(val.begin(), 1, createString(format));
    Context::getBuilder()->CreateCall(
            Context::getInstance()->MainModule->getFunction("printf"),
            val
    );
}

int main() {
    Context::getInstance();

    Funcs::declareMemFuncs();
    Funcs::declarePrintfFunc();

    // Setup main function
    {
        FunctionType *mainType = FunctionType::get(Context::getCompiledType("int"), false);
        Function *main = Function::Create(
                mainType,
                llvm::GlobalValue::ExternalLinkage,
                "main",
                *Context::getInstance()->MainModule
        );
        BasicBlock *mainBlock = BasicBlock::Create(*Context::getInstance()->LLVMContext, "", main);
        Context::getBuilder()->SetInsertPoint(mainBlock);
    }

    auto int64Size = Context::getInstance()->MainModule->getDataLayout().getTypeAllocSizeInBits(
            Context::getCompiledType("int")
    );
    auto numItems = 2;
    auto numAlloc = Context::getBuilder()->CreateCall(
            Context::getInstance()->MainModule->getFunction("malloc"),
            {
                    llvm::ConstantInt::get(Context::getCompiledType("int"), int64Size * numItems)
            },
            "arr.malloc"
    );
    auto num = Context::getBuilder()->CreateBitCast(
            numAlloc,
            Context::getCompiledType("int")->getPointerTo(0),
            "arr"
    );

    put("\n--- Write ---\n");

    for (int i = 0; i < numItems; i++) {
        auto gep = Context::getBuilder()->CreateInBoundsGEP(
                num->getType()->getPointerElementType(),
                num,
                llvm::ConstantInt::get(
                        Context::getCompiledType("int"),
                        i
                ),
                "write"
        );
        Context::getBuilder()->CreateStore(
                llvm::ConstantInt::get(
                        Context::getCompiledType("int"),
                        i * i
                ),
                gep
        );
        put(
                "#%d: %d\n", {
                        llvm::ConstantInt::get(Context::getCompiledType("int"), i),
                        llvm::ConstantInt::get(Context::getCompiledType("int"), i * i),
                }
        );
    }

    put("\n--- Read ---\n");

    for (int i = 0; i < numItems; i++) {
        auto gep = Context::getBuilder()->CreateInBoundsGEP(
                num->getType()->getPointerElementType(),
                num,
                llvm::ConstantInt::get(
                        Context::getCompiledType("int"),
                        i
                ),
                "read"
        );
        put(
                "#%d: %d\n",
                {
                        llvm::ConstantInt::get(Context::getCompiledType("int"), i),
                        Context::getBuilder()->CreateLoad(gep->getType()->getPointerElementType(), gep)
                }
        );
    }

    put("\n--- Free ---\n");

    Context::getBuilder()->CreateCall(
            Context::getInstance()->MainModule->getFunction("free"),
            Context::getBuilder()->CreateBitCast(num, Type::getInt8PtrTy(*Context::getInstance()->LLVMContext))
    );

    put("Done\n");

    put("\n--- Read After Free ---\n");

    for (int i = 0; i < numItems; i++) {
        auto gep = Context::getBuilder()->CreateInBoundsGEP(
                num->getType()->getPointerElementType(),
                num,
                llvm::ConstantInt::get(
                        Context::getCompiledType("int"),
                        i
                ),
                "read"
        );
        put(
                "#%d: %d\n",
                {
                        llvm::ConstantInt::get(Context::getCompiledType("int"), i),
                        Context::getBuilder()->CreateLoad(gep->getType()->getPointerElementType(), gep)
                }
        );
    }

    // return 0;
    Context::getBuilder()->CreateRet(Context::getDefaultValue("int"));

    std::error_code eCode;
    Context::getInstance()->dumpModule(R"(P:\Cpp\teclang\out\out.ll)", eCode);
    return 0;
}