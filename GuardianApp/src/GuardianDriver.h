#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
#include <utility>
 
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>
#include "TripParam.h"
 
#define FEL_PARAMS_SIZE 70 // TODO: This will be seperated by mode soon
#define TOL_PARAMS_SIZE 16
#define SNAPSHOT_TRIGGER_STRING "SNAPSHOT_TRG"
#define STORED_VALUE_STRING "STORED"
#define SNAPSHOT_VALUE_STRING "SNAPSHOT"
#define TOLERANCE_VALUE_STRING "TOLERANCE"

 
class GuardianDriver : public asynPortDriver {
  public:
    GuardianDriver(const char *portName);
    void FELpulseEnergyMonitor(void);
    void takeSnapshot();
    void tripLogic();

    bool outsideTolerancePercentage(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId);
    bool outsideAbsTolerancePercentage(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId);
    bool outsideAbsTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId);
    bool outsideDegreeTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId);
    bool outsideQuadsTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId);
    bool dataUnchanged(uint32_t deviceDataId, uint32_t snapshotId);

    void initializeNCTripParamMap();
    void initializeSCTripParamMap();

    // virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    // virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);

    TripParamMap NCTripParamMap; // Normal Conducting (LCLS-I) trip logic parameters
    TripParamMap SCTripParamMap; // Super Conducting (LCLS-II) trip logic parameters

  protected:
    int SnapshotTriggerIndex;
    int StoredValueIndex;
    int SnapshotValueIndex;
    int ToleranceValueIndex;

};