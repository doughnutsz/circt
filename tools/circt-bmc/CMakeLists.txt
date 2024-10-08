if(MLIR_ENABLE_EXECUTION_ENGINE)
  add_compile_definitions(CIRCT_BMC_ENABLE_JIT)
  set(CIRCT_BMC_JIT_LLVM_COMPONENTS native)
  set(CIRCT_BMC_JIT_DEPS
    MLIRExecutionEngine
    MLIRExecutionEngineUtils
  )
endif()

set(LLVM_LINK_COMPONENTS Support ${CIRCT_BMC_JIT_LLVM_COMPONENTS})

add_circt_tool(circt-bmc circt-bmc.cpp)
target_link_libraries(circt-bmc
  PRIVATE
  CIRCTBMCTransforms
  CIRCTSMTToZ3LLVM
  CIRCTHWToSMT
  CIRCTCombToSMT
  CIRCTVerifToSMT
  CIRCTComb
  CIRCTHW
  CIRCTSeq
  CIRCTSMT
  CIRCTVerif
  CIRCTSupport
  MLIRIR
  MLIRFuncDialect
  MLIRArithDialect
  MLIRLLVMIRTransforms
  MLIRLLVMToLLVMIRTranslation
  MLIRTargetLLVMIRExport
  MLIRFuncInlinerExtension
  MLIRBuiltinToLLVMIRTranslation
  LLVMSupport

  ${CIRCT_BMC_JIT_DEPS}
)

llvm_update_compile_flags(circt-bmc)
mlir_check_all_link_libraries(circt-bmc)
