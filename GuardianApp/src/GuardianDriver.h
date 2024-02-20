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
 
#define MONITOR_CYCLE_STRING "MONITOR_CYCLE_TIME"
#define SNAPSHOT_TRIGGER_STRING "SNAPSHOT_TRG"
#define DEVICE_PARAM_SIZE_STRING "DEVICE_PARAM_SIZE"
#define TOLERANCE_PARAM_SIZE_STRING "TOLERANCE_PARAM_SIZE"
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
    int MonitorCycleIndex;
    int SnapshotTriggerIndex;
    int DeviceParamSizeIndex;
    int ToleranceParamSizeIndex;
    int StoredValueIndex;
    int SnapshotValueIndex;
    int ToleranceValueIndex;
    int DEVICE_PARAMS_SIZE; // This will be seperated by mode soon and shouldn't be hardcoded, but getNumParam() instead
    int TOL_PARAMS_SIZE; // number tolerance 'control' pv asyn parameters
    double MONITOR_CYCLE_TIME; // Used to control the sleep() in the FELpulseMonitor thread
};