// RUN: circt-opt --arc-lower-verif-simulations %s | FileCheck %s

// CHECK: func.func private @exit(i32)

// CHECK-LABEL: hw.module @verif.simulation.impl.Foo(
// CHECK-SAME: in %clock : !seq.clock
// CHECK-SAME: in %init : i1
// CHECK-SAME: out done : i1
// CHECK-SAME: out success : i1
verif.simulation @Foo {} {
^bb0(%clock: !seq.clock, %init: i1):
  // CHECK: [[TMP1:%.+]] = func.call @generateDone()
  // CHECK: [[TMP2:%.+]] = func.call @generateSuccess()
  // CHECK: hw.output [[TMP1]], [[TMP2]] : i1, i1
  %done = func.call @generateDone() : () -> (i1)
  %success = func.call @generateSuccess() : () -> (i1)
  verif.yield %done, %success : i1, i1
}

func.func private @generateDone() -> i1
func.func private @generateSuccess() -> i1

// CHECK-LABEL: func.func @Foo()
// CHECK: [[I0:%.+]] = hw.constant false
// CHECK: [[I1:%.+]] = hw.constant true
// CHECK: [[C0:%.+]] = seq.to_clock [[I0]]
// CHECK: [[C1:%.+]] = seq.to_clock [[I1]]
// CHECK: arc.sim.instantiate @verif.simulation.impl.Foo as [[A:%.+]] {
// CHECK:   scf.execute_region {
// CHECK:     arc.sim.set_input [[A]], "clock" = [[C0]]
// CHECK:     arc.sim.set_input [[A]], "init" = [[I1]]
// CHECK:     arc.sim.step [[A]]
// CHECK:     arc.sim.set_input [[A]], "clock" = [[C1]]
// CHECK:     arc.sim.step [[A]]
// CHECK:     arc.sim.set_input [[A]], "clock" = [[C0]]
// CHECK:     arc.sim.set_input [[A]], "init" = [[I0]]
// CHECK:     arc.sim.step [[A]]
// CHECK:     cf.br [[LOOP:\^.+]]
// CHECK:   [[LOOP]]:
// CHECK:     [[DONE:%.+]] = arc.sim.get_port [[A]], "done" : i1
// CHECK:     [[SUCCESS:%.+]] = arc.sim.get_port [[A]], "success" : i1
// CHECK:     arc.sim.set_input [[A]], "clock" = [[C1]]
// CHECK:     arc.sim.step [[A]]
// CHECK:     arc.sim.set_input [[A]], "clock" = [[C0]]
// CHECK:     arc.sim.step [[A]]
// CHECK:     cf.cond_br [[DONE]], [[EXIT:\^.+]], [[LOOP]]
// CHECK:   [[EXIT]]:
// CHECK:     [[TMP1:%.+]] = hw.constant true
// CHECK:     [[TMP2:%.+]] = arith.xori [[SUCCESS]], [[TMP1]] : i1
// CHECK:     [[TMP3:%.+]] = arith.extui [[TMP2]] : i1 to i32
// CHECK:     func.call @exit([[TMP3]]) : (i32) -> ()
// CHECK:     scf.yield
// CHECK:   }
// CHECK: }
// CHECK: return
