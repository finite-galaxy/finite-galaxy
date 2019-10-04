// Personality.h

#ifndef PERSONALITY_H_
#define PERSONALITY_H_

#include "Angle.h"
#include "Point.h"

class DataNode;
class DataWriter;



// Class defining an AI "personality": what actions it takes, and how skilled
// and aggressive it is in combat. This also includes some more specialized
// behaviours, like plundering ships or launching surveillance drones, that are
// used to make some fleets noticeably different from others.
class Personality {
public:
  Personality();

  void Load(const DataNode &node);
  void Save(DataWriter &out) const;

  // Who a ship decides to attack:
  bool IsPacifist() const;
  bool IsForbearing() const;
  bool IsTimid() const;
  bool IsHeroic() const;
  bool IsNemesis() const;

  // How they fight:
  bool IsFrugal() const;
  bool Disables() const;
  bool Plunders() const;
  bool IsVindictive() const;
  bool IsUnconstrained() const;
  bool IsCoward() const;
  bool IsAppeasing() const;
  bool IsOpportunistic() const;

  // Mission NPC states:
  bool IsStaying() const;
  bool IsEntering() const;
  bool IsWaiting() const;
  bool IsLaunching() const;
  bool IsFleeing() const;
  bool IsDerelict() const;
  bool IsUninterested() const;

  // Non-combat goals:
  bool IsSurveillance() const;
  bool IsMining() const;
  bool Harvests() const;
  bool IsSwarming() const;

  // Special flags:
  bool IsEscort() const;
  bool IsTarget() const;
  bool IsMarked() const;
  bool IsMute() const;

  // Current inaccuracy in this ship's targeting:
  const Point &Confusion() const;
  void UpdateConfusion(bool isFiring);

  // Personality to use for ships defending a planet from domination:
  static Personality Defender();


private:
  void Parse(const DataNode &node, int index, bool remove);


private:
  int flags;
  double confusionMultiplier;
  double aimMultiplier;
  Point confusion;
  Point confusionVelocity;
};



#endif
