#include "foraging_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/kheperaiv/simulator/kheperaiv_entity.h>

#include <iostream>
#include <map>

#include "foraging_buzz_controller_kheperaiv.h"

/****************************************/
/****************************************/

static std::map<int, int> gZones;

SFoodData::SFoodData()
    : HasFoodItem(false), FoodItemIdx(0), TotalFoodItems(0) {}

void SFoodData::Reset() {
  HasFoodItem = false;
  FoodItemIdx = 0;
  TotalFoodItems = 0;
}

CForagingLoopFunctions::CForagingLoopFunctions()
    : m_cForagingArenaSideX(-1.6f, 1.7f), m_cForagingArenaSideY(-1.7f, 1.7f),
      m_pcFloor(NULL), m_pcRNG(NULL), m_unCollectedFood(0), m_nEnergy(0),
      m_unEnergyPerFoodItem(1), m_unEnergyPerWalkingRobot(1) {}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Init(TConfigurationNode &t_node) {
  try {
    TConfigurationNode &tForaging = GetNode(t_node, "foraging");
    /* Get a pointer to the floor entity */
    m_pcFloor = &GetSpace().GetFloorEntity();
    /* Get the number of food items we want to be scattered from XML */
    UInt32 unFoodItems;
    GetNodeAttribute(tForaging, "items", unFoodItems);
    /* Get the number of food items we want to be scattered from XML */
    GetNodeAttribute(tForaging, "radius", m_fFoodSquareRadius);
    m_fFoodSquareRadius *= m_fFoodSquareRadius;

    // Get danger zones attributes
    UInt32 unDangerZones;
    GetNodeAttribute(tForaging, "dangerzones", unDangerZones);
    GetNodeAttribute(tForaging, "radiusdanger", m_fDangerZoneRadius);
    m_fDangerZoneRadius *= m_fDangerZoneRadius;

    /* Create a new RNG */
    m_pcRNG = CRandom::CreateRNG("argos");
    /* Distribute uniformly the items in the environment */
    for (UInt32 i = 0; i < unFoodItems; ++i) {
      m_cFoodPos.push_back(CVector2(m_pcRNG->Uniform(m_cForagingArenaSideX),
                                    m_pcRNG->Uniform(m_cForagingArenaSideY)));
    }

    // Distribute the Danger zones uniformly
    for (UInt32 i = 0; i < unDangerZones; ++i) {
      m_cDangerPos.push_back(CVector2(m_pcRNG->Uniform(m_cForagingArenaSideX),
                                    m_pcRNG->Uniform(m_cForagingArenaSideY)));
    }

    /* Get the output file name from XML */
    GetNodeAttribute(tForaging, "output", m_strOutput);
    /* Open the file, erasing its contents */
    m_cOutput.open(m_strOutput.c_str(),
                   std::ios_base::trunc | std::ios_base::out);
    m_cOutput << "# clock\twalking\tresting\tcollected_food\tenergy"
              << std::endl;


    /* Get energy gain per item collected */
    GetNodeAttribute(tForaging, "energy_per_item", m_unEnergyPerFoodItem);
    /* Get energy loss per walking robot */
    GetNodeAttribute(tForaging, "energy_per_walking_robot",
                     m_unEnergyPerWalkingRobot);
  } catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
  }
}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Reset() {
  /* Zero the counters */
  m_unCollectedFood = 0;
  m_nEnergy = 0;
  /* Close the file */
  m_cOutput.close();
  /* Open the file, erasing its contents */
  m_cOutput.open(m_strOutput.c_str(),
                 std::ios_base::trunc | std::ios_base::out);
  m_cOutput << "# clock\twalking\tresting\tcollected_food\tenergy" << std::endl;
  /* Distribute uniformly the items in the environment */
  for (UInt32 i = 0; i < m_cFoodPos.size(); ++i) {
    m_cFoodPos[i].Set(m_pcRNG->Uniform(m_cForagingArenaSideX),
                      m_pcRNG->Uniform(m_cForagingArenaSideY));
  }
}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Destroy() {
  /* Close the file */
  m_cOutput.close();
}

/****************************************/
/****************************************/

CColor
CForagingLoopFunctions::GetFloorColor(const CVector2 &c_position_on_plane) {
  if (c_position_on_plane.GetX() < -1.8f) {
    return CColor::GRAY50;
  }

  for (UInt32 i = 0; i < m_cDangerPos.size(); ++i) {
    if ((c_position_on_plane - m_cDangerPos[i]).SquareLength() <
        m_fDangerZoneRadius) {
      return CColor::RED;
    }
  }

  for (UInt32 i = 0; i < m_cFoodPos.size(); ++i) {
    if ((c_position_on_plane - m_cFoodPos[i]).SquareLength() <
        m_fFoodSquareRadius) {
      return CColor::BLACK;
    }
  }
  return CColor::WHITE;
}

/****************************************/
/****************************************/

int CForagingLoopFunctions::GetZone(int id) {
  return gZones[id];
}

void CForagingLoopFunctions::PreStep() {
  CSpace::TMapPerType &m_cBots = GetSpace().GetEntitiesByType("kheperaiv");
  int i = 0;
  for (CSpace::TMapPerType::iterator it = m_cBots.begin(); it != m_cBots.end();
      ++it) {
    CKheperaIVEntity *cBot = any_cast<CKheperaIVEntity*>(it->second);
    CForagingBuzzControllerKheperaIV &cController = dynamic_cast<CForagingBuzzControllerKheperaIV&>(cBot->GetControllableEntity().GetController());
    CVector3 cPos = cController.GetPos();

    int zone = 0;
    CVector2 c_position_on_plane(cPos.GetX(), cPos.GetY());

    for (UInt32 i = 0; i < m_cFoodPos.size(); ++i) {
      if ((c_position_on_plane - m_cFoodPos[i]).SquareLength() <
          m_fFoodSquareRadius) {
        zone = 1;
        break;
      }
    }

    for (UInt32 i = 0; i < m_cDangerPos.size(); ++i) {
      if ((c_position_on_plane - m_cDangerPos[i]).SquareLength() <
          m_fDangerZoneRadius) {
        zone = 2;
        break;
      }
    }
    gZones[i] = zone;
    std::cout << "(x=" << cPos.GetX() << ",y=" << cPos.GetY() << "): " << zone << std::endl;
    ++i;
  }
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CForagingLoopFunctions, "foraging_loop_functions")
