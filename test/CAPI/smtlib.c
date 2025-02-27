
/*===- smtlib.c - Simple test of SMTLIB C API -----------------------------===*\
|*                                                                            *|
|* Part of the LLVM Project, under the Apache License v2.0 with LLVM          *|
|* Exceptions.                                                                *|
|* See https://llvm.org/LICENSE.txt for license information.                  *|
|* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception                    *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

/* RUN: circt-capi-smtlib-test 2>&1 | FileCheck %s
 */

#include "circt-c/Dialect/SMT.h"
#include "circt-c/ExportSMTLIB.h"
#include "mlir-c/Dialect/Func.h"
#include "mlir-c/IR.h"
#include "mlir-c/Support.h"
#include <assert.h>
#include <stdio.h>

void dumpCallback(MlirStringRef message, void *userData) {
  fprintf(stderr, "%.*s", (int)message.length, message.data);
}

void testExportSMTLIB(MlirContext ctx) {
  // clang-format off
  const char *testSMT = 
    "func.func @test() {\n"
    "  smt.solver() : () -> () { }\n"
    "  return\n"
    "}\n";
  // clang-format on

  MlirModule module =
      mlirModuleCreateParse(ctx, mlirStringRefCreateFromCString(testSMT));

  MlirLogicalResult result = mlirExportSMTLIB(module, dumpCallback, NULL);
  (void)result;
  assert(mlirLogicalResultIsSuccess(result));

  // CHECK: ; solver scope 0
  // CHECK-NEXT: (reset)
}

void testSMTType(MlirContext ctx) {
  MlirType boolType = smtTypeGetBool(ctx);
  MlirType intType = smtTypeGetInt(ctx);
  MlirType arrayType = smtTypeGetArray(ctx, intType, boolType);
  MlirType bvType = smtTypeGetBitVector(ctx, 32);
  MlirType funcType =
      smtTypeGetSMTFunc(ctx, 2, (MlirType[]){intType, boolType}, boolType);
  MlirType sortType = smtTypeGetSort(
      ctx, mlirIdentifierGet(ctx, mlirStringRefCreateFromCString("sort")), 0,
      NULL);

  // CHECK: !smt.bool
  mlirTypeDump(boolType);
  // CHECK: !smt.int
  mlirTypeDump(intType);
  // CHECK: !smt.array<[!smt.int -> !smt.bool]>
  mlirTypeDump(arrayType);
  // CHECK: !smt.bv<32>
  mlirTypeDump(bvType);
  // CHECK: !smt.func<(!smt.int, !smt.bool) !smt.bool>
  mlirTypeDump(funcType);
  // CHECK: !smt.sort<"sort">
  mlirTypeDump(sortType);

  // CHECK: bool_is_any_non_func_smt_value_type
  fprintf(stderr, smtTypeIsAnyNonFuncSMTValueType(boolType)
                      ? "bool_is_any_non_func_smt_value_type\n"
                      : "bool_is_func_smt_value_type\n");
  // CHECK: int_is_any_non_func_smt_value_type
  fprintf(stderr, smtTypeIsAnyNonFuncSMTValueType(intType)
                      ? "int_is_any_non_func_smt_value_type\n"
                      : "int_is_func_smt_value_type\n");
  // CHECK: array_is_any_non_func_smt_value_type
  fprintf(stderr, smtTypeIsAnyNonFuncSMTValueType(arrayType)
                      ? "array_is_any_non_func_smt_value_type\n"
                      : "array_is_func_smt_value_type\n");
  // CHECK: bit_vector_is_any_non_func_smt_value_type
  fprintf(stderr, smtTypeIsAnyNonFuncSMTValueType(bvType)
                      ? "bit_vector_is_any_non_func_smt_value_type\n"
                      : "bit_vector_is_func_smt_value_type\n");
  // CHECK: sort_is_any_non_func_smt_value_type
  fprintf(stderr, smtTypeIsAnyNonFuncSMTValueType(sortType)
                      ? "sort_is_any_non_func_smt_value_type\n"
                      : "sort_is_func_smt_value_type\n");
  // CHECK: smt_func_is_func_smt_value_type
  fprintf(stderr, smtTypeIsAnyNonFuncSMTValueType(funcType)
                      ? "smt_func_is_any_non_func_smt_value_type\n"
                      : "smt_func_is_func_smt_value_type\n");

  // CHECK: bool_is_any_smt_value_type
  fprintf(stderr, smtTypeIsAnySMTValueType(boolType)
                      ? "bool_is_any_smt_value_type\n"
                      : "bool_is_not_any_smt_value_type\n");
  // CHECK: int_is_any_smt_value_type
  fprintf(stderr, smtTypeIsAnySMTValueType(intType)
                      ? "int_is_any_smt_value_type\n"
                      : "int_is_not_any_smt_value_type\n");
  // CHECK: array_is_any_smt_value_type
  fprintf(stderr, smtTypeIsAnySMTValueType(arrayType)
                      ? "array_is_any_smt_value_type\n"
                      : "array_is_not_any_smt_value_type\n");
  // CHECK: array_is_any_smt_value_type
  fprintf(stderr, smtTypeIsAnySMTValueType(bvType)
                      ? "array_is_any_smt_value_type\n"
                      : "array_is_not_any_smt_value_type\n");
  // CHECK: smt_func_is_any_smt_value_type
  fprintf(stderr, smtTypeIsAnySMTValueType(funcType)
                      ? "smt_func_is_any_smt_value_type\n"
                      : "smt_func_is_not_any_smt_value_type\n");
  // CHECK: sort_is_any_smt_value_type
  fprintf(stderr, smtTypeIsAnySMTValueType(sortType)
                      ? "sort_is_any_smt_value_type\n"
                      : "sort_is_not_any_smt_value_type\n");

  // CHECK: int_type_is_not_a_bool
  fprintf(stderr, smtTypeIsABool(intType) ? "int_type_is_a_bool\n"
                                          : "int_type_is_not_a_bool\n");
  // CHECK: bool_type_is_not_a_int
  fprintf(stderr, smtTypeIsAInt(boolType) ? "bool_type_is_a_int\n"
                                          : "bool_type_is_not_a_int\n");
  // CHECK: bv_type_is_not_a_array
  fprintf(stderr, smtTypeIsAArray(bvType) ? "bv_type_is_a_array\n"
                                          : "bv_type_is_not_a_array\n");
  // CHECK: array_type_is_not_a_bit_vector
  fprintf(stderr, smtTypeIsABitVector(arrayType)
                      ? "array_type_is_a_bit_vector\n"
                      : "array_type_is_not_a_bit_vector\n");
  // CHECK: sort_type_is_not_a_smt_func
  fprintf(stderr, smtTypeIsASMTFunc(sortType)
                      ? "sort_type_is_a_smt_func\n"
                      : "sort_type_is_not_a_smt_func\n");
  // CHECK: func_type_is_not_a_sort
  fprintf(stderr, smtTypeIsASort(funcType) ? "func_type_is_a_sort\n"
                                           : "func_type_is_not_a_sort\n");
}

void testSMTAttribute(MlirContext ctx) {
  // CHECK: slt_is_BVCmpPredicate
  fprintf(stderr,
          smtAttrCheckBVCmpPredicate(ctx, mlirStringRefCreateFromCString("slt"))
              ? "slt_is_BVCmpPredicate\n"
              : "slt_is_not_BVCmpPredicate\n");
  // CHECK: lt_is_not_BVCmpPredicate
  fprintf(stderr,
          smtAttrCheckBVCmpPredicate(ctx, mlirStringRefCreateFromCString("lt"))
              ? "lt_is_BVCmpPredicate\n"
              : "lt_is_not_BVCmpPredicate\n");
  // CHECK: slt_is_not_IntPredicate
  fprintf(stderr,
          smtAttrCheckIntPredicate(ctx, mlirStringRefCreateFromCString("slt"))
              ? "slt_is_IntPredicate\n"
              : "slt_is_not_IntPredicate\n");
  // CHECK: lt_is_IntPredicate
  fprintf(stderr,
          smtAttrCheckIntPredicate(ctx, mlirStringRefCreateFromCString("lt"))
              ? "lt_is_IntPredicate\n"
              : "lt_is_not_IntPredicate\n");

  // CHECK: #smt.bv<5> : !smt.bv<32>
  mlirAttributeDump(smtAttrGetBitVector(ctx, 5, 32));
  // CHECK: 0 : i64
  mlirAttributeDump(
      smtAttrGetBVCmpPredicate(ctx, mlirStringRefCreateFromCString("slt")));
  // CHECK: 0 : i64
  mlirAttributeDump(
      smtAttrGetIntPredicate(ctx, mlirStringRefCreateFromCString("lt")));
}

int main(void) {
  MlirContext ctx = mlirContextCreate();
  mlirDialectHandleLoadDialect(mlirGetDialectHandle__smt__(), ctx);
  mlirDialectHandleLoadDialect(mlirGetDialectHandle__func__(), ctx);
  testExportSMTLIB(ctx);
  testSMTType(ctx);
  testSMTAttribute(ctx);
  return 0;
}
