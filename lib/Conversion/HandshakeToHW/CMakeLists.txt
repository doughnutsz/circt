add_circt_conversion_library(CIRCTHandshakeToHW
  HandshakeToHW.cpp

  ADDITIONAL_HEADER_DIRS
  ${MLIR_MAIN_INCLUDE_DIR}/mlir/Conversion/HandshakeToHW

  DEPENDS
  CIRCTConversionPassIncGen
  MLIRArithDialect

  LINK_LIBS PUBLIC
  CIRCTHW
  CIRCTESI
  CIRCTHandshake
  MLIRIR
  MLIRPass
  MLIRArithDialect
  MLIRControlFlowDialect
  MLIRFuncDialect
  MLIRSupport
  MLIRTransforms
  )
