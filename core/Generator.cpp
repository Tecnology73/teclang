#include "./Generator.h"
#include "Context.h"
#include "../generators/Function.h"
#include "../generators/types/Array.h"
#include "../generators/types/String.h"
#include "../generators/types/Int.h"
#include "../generators/types/System.h"

Generator::Generator() {
    // Setup Context, Builder & Module
    Context = new llvm::LLVMContext();
    Builder = new llvm::IRBuilder<>(*Context);
    Module = std::make_unique<llvm::Module>("TecLang", *Context);

    Context->setOpaquePointers(false);

    Module->setTargetTriple("x86_64-pc-win32-gnu");
    // Module->setDataLayout("e-m:w-i64:64-f80:128-n8:16:32:64-S128");
    Module->setDataLayout("e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128");

    // Setup optimizations
    // llvm::LoopAnalysisManager LAM;
    // llvm::FunctionAnalysisManager FAM;
    // llvm::CGSCCAnalysisManager CGAM;
    //
    // PB.registerModuleAnalyses(MAM);
    // PB.registerCGSCCAnalyses(CGAM);
    // PB.registerFunctionAnalyses(FAM);
    // PB.registerLoopAnalyses(LAM);
    // PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    prepareBuiltInTypes();
    prepareBuiltInFuncs();
}

int Generator::Generate() {
    auto entryPoint = Context::Instance->GetFunction("main");
    auto func = Generators::Function::Generate(this, entryPoint);

    hasGenerated = true;
    return 0;
}

int Generator::DumpModule(const std::string &outFilePath/*, llvm::OptimizationLevel optimizationLevel*/) {
    if (!hasGenerated)
        return 1;

    // if (optimizationLevel != llvm::OptimizationLevel::O0) {
    //     llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(optimizationLevel);
    //     MPM.run(*Module, MAM);
    // }

    std::error_code eCode;
    llvm::raw_fd_ostream outStream(outFilePath, eCode);

    Module->print(outStream, nullptr);
    return 0;
}

bool Generator::HasCompiledType(const std::string &name) const {
    return GetCompiledType(name) != nullptr;
}

llvm::Type *Generator::SetCompiledType(const std::string &name, llvm::Type *type) {
    return compiledTypes[name] = type;
}

llvm::Type *Generator::GetCompiledType(const std::string &name) const {
    auto type = compiledTypes.find(name);
    if (type == compiledTypes.end())
        return nullptr;

    return type->second;
}

llvm::Function *Generator::SetLibraryFunction(const std::string &name, llvm::Function *function) {
    return LibraryFunctions[name] = function;
}

llvm::Function *Generator::GetLibraryFunction(const std::string &name) const {
    auto type = LibraryFunctions.find(name);
    if (type == LibraryFunctions.end())
        return nullptr;

    return type->second;
}

std::shared_ptr<ContextScope> Generator::OpenScope() {
    std::shared_ptr<ContextScope> newScope = std::make_shared<ContextScope>();
    newScope->ParentScope = CurrentScope;

    return CurrentScope = newScope;
}

std::shared_ptr<ContextScope> Generator::CloseScope() {
    // There is no scope if we're not currently generating inside a function.
    if (!CurrentScope)
        return nullptr;

    return CurrentScope = CurrentScope->ParentScope;
}

int Generator::PrintError(const std::string &message, int returnCode) const {
    std::cerr << "Error: " << message << std::endl;

    return returnCode;
}

void Generator::prepareBuiltInTypes() {
    for (const auto &type: Context::Instance->DeclaredTypes) {
        if (type.second->IsUserDefined) continue;

        if (auto intType = std::dynamic_pointer_cast<IntTypeDef>(type.second))
            intType->generate(this);
        else
            type.second->generate(this);

        type.second->IsDeclared = true;
    }
}

void Generator::prepareBuiltInFuncs() {
    declareFunc(
        "printf",
        GetCompiledType("int*"),
        {},
        true
    );
    declareFunc(
        "malloc",
        GetCompiledType("void*"),
        {
            GetCompiledType("int") // size
        }
    );
    declareFunc(
        "realloc",
        GetCompiledType("void*"),
        {
            GetCompiledType("void*"), // ptr
            GetCompiledType("int")    // size
        }
    );
    declareFunc(
        "calloc",
        GetCompiledType("int8*"),
        {
            GetCompiledType("int"), // num
            GetCompiledType("int")  // size
        }
    );
    declareFunc(
        "free",
        GetCompiledType("void"),
        {
            GetCompiledType("int8*") // source
        }
    );
    declareFunc(
        "memcpy",
        GetCompiledType("int8*"),
        {
            GetCompiledType("int8*"), // source
            GetCompiledType("int8*"), // destination
            GetCompiledType("int")    // n
        }
    );
    declareFunc(
        "strlen",
        GetCompiledType("int"),
        {
            GetCompiledType("int8*") // source
        }
    );
    declareFunc(
        "strcat",
        GetCompiledType("int8*"),
        {
            GetCompiledType("int8*"), // destination
            GetCompiledType("int8*")  // source
        }
    );

    // Int::generate(this);
    // String::generate(this);
    // System::generate(this);
    Array::generate(this);
}

void Generator::declareFunc(
    const std::string &name,
    llvm::Type *returnType,
    const std::vector<llvm::Type *> &args,
    bool variableArgs
) {
    auto funcType = llvm::FunctionType::get(returnType, args, variableArgs);
    auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name, *Module);
    func->setCallingConv(llvm::CallingConv::C);

    SetLibraryFunction(name, func);
}