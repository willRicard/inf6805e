#ifndef FORAGING_LOOP_FUNCTIONS_H
#define FORAGING_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>

#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>

using namespace argos;

/*
 * This structure holds data about food collecting by the robots
 */
struct SFoodData {
  bool HasFoodItem;      // true when the robot is carrying a food item
  size_t FoodItemIdx;    // the index of the current food item in the array of
                         // available food items
  size_t TotalFoodItems; // the total number of food items carried by this robot
                         // during the experiment

  SFoodData();
  void Reset();
};

class CForagingLoopFunctions : public CLoopFunctions {

public:
  CForagingLoopFunctions();
  virtual ~CForagingLoopFunctions() {}

  virtual void Init(TConfigurationNode &t_tree);
  virtual void Reset();
  virtual void Destroy();
  virtual CColor GetFloorColor(const CVector2 &c_position_on_plane);
  virtual void PreStep();

  static int GetZone(int id);

private:
  Real m_fFoodSquareRadius;
  Real m_fDangerZoneRadius;

  CRange<Real> m_cForagingArenaSideX, m_cForagingArenaSideY;
  CRange<Real> m_cDangerZoneSideX, m_cDangerZoneSideY;

  std::vector<CVector2> m_cFoodPos;
  std::vector<CVector2> m_cDangerPos;
  CFloorEntity *m_pcFloor;
  CRandom::CRNG *m_pcRNG;

  std::string m_strOutput;
  std::ofstream m_cOutput;

  UInt32 m_unCollectedFood;
  SInt64 m_nEnergy;
  UInt32 m_unEnergyPerFoodItem;
  UInt32 m_unEnergyPerWalkingRobot;
};

#endif
