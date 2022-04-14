#include <argos3/plugins/robots/kheperaiv/control_interface/buzz_controller_kheperaiv.h>

class CForagingBuzzControllerKheperaIV : public CBuzzControllerKheperaIV {
public:
  CVector3 GetPos();
private:
  virtual buzzvm_state RegisterFunctions();
};
