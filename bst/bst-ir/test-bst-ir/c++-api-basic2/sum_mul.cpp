#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <map>

using namespace llvm;

class SumMulAttr {
public:
    LLVMModuleRef m;
    std::vector<LLVMTypeRef> p;
    LLVMTypeRef rsum;
    LLVMTypeRef rmul;
    LLVMValueRef sum;
    LLVMValueRef mul;

    SumMulAttr::SumMulAttr(
        LLVMModuleRef m,
        std::vector<LLVMTypeRef> p,
        LLVMTypeRef rsum,
        LLVMTypeRef rmul,
        LLVMValueRef sum,
        LLVMValueRef mul
    ) : m(m), p(p), rsum(rsum), rmul(rmul), sum(sum), mul(mul) {}
};

class CodeGen {
private:
    std::unique_ptr<LLVMContext> TheContext;
    std::unique_ptr<Module> TheModule;
    std::unique_ptr<IRBuilder<>> Builder;
    std::map<std::string, Value *> NamedValues;
    std::unique_ptr<JIT> TheJIT;

public:
    SumMulAttr create_module(void);
    void build_function(SumMulAttr& sma, LLVMBuilderRef *builder);
    void verify_module(SumMulAttr& sma);
    // LLVMExecutionEngineRef create_engine(SumMulAttr& sa);
    // void run_module(SumMulAttr& sa, int32_t x, int32_t y,  LLVMExecutionEngineRef *engine);
    // void output_bc(SumMulAttr& sa);
};

SumMulAttr CodeGen::create_module(void) {
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("KaleidoscopeJIT", *TheContext);
    TheModule->setDataLayout(TheJIT->getDataLayout());
}