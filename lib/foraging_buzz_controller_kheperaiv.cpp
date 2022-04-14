#include "foraging_buzz_controller_kheperaiv.h"

#include <iostream>

#include "foraging_loop_functions.h"

CVector3 CForagingBuzzControllerKheperaIV::GetPos() {
  return m_pcPos->GetReading().Position;
}

int BuzzGetZone(buzzvm_t vm) {
  buzzvm_lload(vm, 1);
  buzzobj_t buzz_id = buzzvm_stack_at(vm, 1);
  int id = 0;
  if (buzz_id->o.type == BUZZTYPE_INT) {
    id = buzz_id->i.value;
  } else {
    buzzvm_seterror(vm,
                    BUZZVM_ERROR_TYPE,
                    "get_zone(id): expected %s, got %s in first argument",
                    buzztype_desc[BUZZTYPE_INT],
                    buzztype_desc[buzz_id->o.type]);
    return vm->state;
  }
  buzzvm_pushi(vm, CForagingLoopFunctions::GetZone(id));
  return buzzvm_ret1(vm);
}

buzzvm_state CForagingBuzzControllerKheperaIV::RegisterFunctions() {
  CBuzzControllerKheperaIV::RegisterFunctions();

  buzzvm_pushs(m_tBuzzVM, buzzvm_string_register(m_tBuzzVM, "get_zone", 1));
  buzzvm_pushcc(m_tBuzzVM, buzzvm_function_register(m_tBuzzVM, BuzzGetZone));
  buzzvm_gstore(m_tBuzzVM);

  return m_tBuzzVM->state;
}

REGISTER_CONTROLLER(CForagingBuzzControllerKheperaIV, "foraging_buzz_controller_kheperaiv");
