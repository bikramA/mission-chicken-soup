

#include "MissionChickenSoup.h"

#include "madara/knowledge/ContextGuard.h"

containers::MissionChickenSoup::MissionChickenSoup ()
: context_ (0)
{
}

containers::MissionChickenSoup::MissionChickenSoup (
  madara::knowledge::KnowledgeBase & knowledge)
{
  init (knowledge);
}

void
containers::MissionChickenSoup::init (
  madara::knowledge::KnowledgeBase & knowledge)
{
  // hold context for use in guards later
  context_ = &knowledge.get_context ();

  // init knowledge variables. DO NOT REMOVE COMMENT
  imu_sigma_accel_.set_name (".imu.sigma.accel", knowledge);
  orientation_.set_name (".orientation", knowledge);
  position_.set_name (".position", knowledge);
}

void
containers::MissionChickenSoup::read (void)
{
  if (context_)
  {
    // lock the context for consistency
    madara::knowledge::ContextGuard guard (*context_);

    // update user-facing variables. DO NOT REMOVE COMMENT
    imu_sigma_accel = imu_sigma_accel_.to_record ().to_doubles ();
    orientation = orientation_.to_record ().to_doubles ();
    position = position_.to_record ().to_doubles ();
  }
}

void
containers::MissionChickenSoup::write (void)
{
  if (context_)
  {
    // lock the context for consistency
    madara::knowledge::ContextGuard guard (*context_);

    // update knowledge base. DO NOT REMOVE COMMENT
    imu_sigma_accel_.set (imu_sigma_accel);
    orientation_.set (orientation);
    position_.set (position);
  }
}

void
containers::MissionChickenSoup::modify (void)
{
  // mark containers as modified so the values are resent. DO NOT REMOVE COMMENT
  imu_sigma_accel_.modify ();
  orientation_.modify ();
  position_.modify ();
}
