#pragma once

#include <llvm/IR/Type.h>
#include "../core/Generator.h"
#include "../structure/TypeDef.h"
#include "../structure/types/IntTypeDef.h"

namespace Generators {
    struct TypeDef {
        static llvm::Type *Generate(Generator *generator, const ::TypeDef *typeDef) {
            if (auto intTypeDef = dynamic_cast<const IntTypeDef *>(typeDef))
                return generateInt(generator, intTypeDef);
            if (typeDef->Name == "bool")
                return generateBool(generator, typeDef);
            if (typeDef->Name == "double")
                return generateDouble(generator, typeDef);
            if (typeDef->Name == "void" || typeDef->Name == "void*")
                return generateVoid(generator, typeDef);

            // We can't have this at the top because IntTypeDef has a dynamic name...
            if (auto compiledType = generator->GetCompiledType(typeDef->Name))
                return compiledType;

            // User-defined type
            std::vector<llvm::Type *> fieldTypes(typeDef->Fields.size());
            unsigned i = 0;
            for (const auto &field: typeDef->Fields) {
                auto type = field->Type->generate(generator);

                if (field->Type->IsComplexType)
                    fieldTypes[i++] = type->getPointerTo(0);
                else
                    fieldTypes[i++] = type;
            }

            auto structType = llvm::StructType::create(
                *generator->Context,
                fieldTypes,
                typeDef->Name
            );
            generator->SetCompiledType(typeDef->Name, structType);

            return structType;
        }

        static llvm::Type *Generate(Generator *generator, const std::shared_ptr<::TypeDef> &typeDef) {
            return Generate(generator, typeDef.get());
        }

    private:
        static llvm::Type *generateInt(Generator *generator, const IntTypeDef *typeDef) {
            auto realName = typeDef->GetRealName();
            if (auto compiledType = generator->GetCompiledType(realName))
                return compiledType;

            auto type = llvm::IntegerType::get(*generator->Context, typeDef->BitLength);
            if (typeDef->IsPointer)
                return generator->SetCompiledType(realName, type->getPointerTo(0));

            return generator->SetCompiledType(realName, type);
        }

        static llvm::Type *generateDouble(Generator *generator, const ::TypeDef *typeDef) {
            if (auto compiledType = generator->GetCompiledType(typeDef->Name))
                return compiledType;

            auto type = generator->Builder->getDoubleTy();
            if (typeDef->IsPointer)
                return generator->SetCompiledType(typeDef->Name, type->getPointerTo(0));

            return generator->SetCompiledType(typeDef->Name, type);
        }

        static llvm::Type *generateBool(Generator *generator, const ::TypeDef *typeDef) {
            if (auto compiledType = generator->GetCompiledType(typeDef->Name))
                return compiledType;

            auto type = generator->Builder->getInt1Ty();
            if (typeDef->IsPointer)
                return generator->SetCompiledType(typeDef->Name, type->getPointerTo(0));

            return generator->SetCompiledType(typeDef->Name, type);
        }

        static llvm::Type *generateVoid(Generator *generator, const ::TypeDef *typeDef) {
            if (auto compiledType = generator->GetCompiledType(typeDef->Name))
                return compiledType;

            if (typeDef->IsPointer)
                return generator->SetCompiledType(typeDef->Name, generator->Builder->getInt8PtrTy(0));

            return generator->SetCompiledType(typeDef->Name, generator->Builder->getVoidTy());
        }
    };
}