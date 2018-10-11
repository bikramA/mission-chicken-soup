
#ifndef   _CONTAINERS_MISSIONCHICKENSOUP_H_
#define   _CONTAINERS_MISSIONCHICKENSOUP_H_

#include <string>
#include <vector>

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/knowledge/containers/Double.h"
#include "madara/knowledge/containers/NativeIntegerVector.h"
#include "madara/knowledge/containers/NativeDoubleVector.h"
#include "madara/knowledge/containers/String.h"

namespace containers
{
  /**
  * Houses variables modified and read by threads
  **/
  class MissionChickenSoup
  {
  public:
    /**
     * Default constructor
     **/
    MissionChickenSoup ();

    /**
     * Constructor
     * @param  knowledge    the context containing variables and values
     **/
    MissionChickenSoup (
      madara::knowledge::KnowledgeBase & knowledge);

    /**
     * Initializes all containers and variable values
     * @param  knowledge    the context containing variables and values
     **/
    void init (madara::knowledge::KnowledgeBase & knowledge);

    /**
     * Modifies all containers to indicate they should be resent on next send
     **/
    void modify (void);

    /**
     * Reads all MADARA containers into the user-facing C++ variables.
     **/
    void read (void);

    /**
     * Writes user-facing C++ variables to the MADARA containers. Note
     * that this does not check if values are different. It always overwrites
     * the values in the knowledge base with what is currently in local vars
     **/
    void write (void);

    /// imu acceleration x, y, z
    std::vector<double> imu_sigma_accel;

    /// orientation x, y, z (roll, pitch, yaw)
    std::vector<double> orientation;

    /// the position of the agent (x, y, z)
    std::vector<double> position;

  private:

    /// imu_sigma_accel that directly interfaces to the KnowledgeBase
    madara::knowledge::containers::NativeDoubleArray imu_sigma_accel_;

    /// orientation that directly interfaces to the KnowledgeBase
    madara::knowledge::containers::NativeDoubleArray orientation_;

    /// imu_sigma_accel that directly interfaces to the KnowledgeBase
    madara::knowledge::containers::NativeDoubleArray position_;


    /// unmanaged context for locking. The knowledge base should stay in scope
    madara::knowledge::ThreadSafeContext * context_;
  };

} // end containers namespace

#endif // _CONTAINERS_MISSIONCHICKENSOUP_H_
