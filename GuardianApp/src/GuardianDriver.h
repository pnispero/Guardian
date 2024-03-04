#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
#include <utility>
#include <chrono>
#include <condition_variable>
#include <tuple>
 
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>
 
#define MONITOR_CYCLE_STRING "MONITOR_CYCLE_TIME"
#define SNAPSHOT_TRIGGER_STRING "SNAPSHOT_TRG"
#define DEVICE_PARAM_SIZE_STRING "DEVICE_PARAM_SIZE"
#define CONDITION_PARAM_SIZE_STRING "CONDITION_PARAM_SIZE"
#define TOLERANCE_PARAM_SIZE_STRING "TOLERANCE_PARAM_SIZE"
#define CURRENT_VALUE_STRING "CURRENT"
#define STORED_VALUE_STRING "STORED"
#define CONDITION_VALUE_STRING "CONDITION"
#define TOLERANCE_VALUE_STRING "TOLERANCE"
#define LOGIC_TYPE_VALUE_STRING "LOGIC_TYPE"
#define TOLERANCE_ID_STRING "TOLERANCE_ID"
#define CONDITION_ID_STRING "CONDITION_ID"
#define TRIP_MSG_STRING "MSG"
#define MPS_TRIP_STRING "MPS_TRIP"
#define MPS_PERMIT_STRING "MPS_PERMIT"
#define HEARTBEAT_VALUE_STRING "HEARTBEAT"
#define ARM_VALUE_STRING "ARM"

 
class GuardianDriver : public asynPortDriver {
  public:
    GuardianDriver(const char *portName);
    void FELpulseEnergyMonitor(void);
    void takeSnapshot();
    void tripLogic();

    bool outsidePercentageTolerance(int deviceIndex);
    bool outsidePercentageTolerance(int curValIndex, int desiredValIndex);
    bool outsideAbsPercentageTolerance(int deviceIndex);
    bool outsideAbsValueTolerance(int deviceIndex);
    bool outsideAbsDifferenceTolerance(int deviceIndex);
    bool outsideCollimatorTolerance();
    std::tuple<bool, std::string> tripSpecialCase(int deviceIndex);
    void initGuardian();

    // virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    // virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);

  private:
    int MonitorCycleIndex;
    int SnapshotTriggerIndex;
    int DeviceParamSizeIndex;
    int ConditionParamSizeIndex;
    int ToleranceParamSizeIndex;
    int CurrentValueIndex;
    int StoredValueIndex;
    int ConditionValueIndex;
    int ToleranceValueIndex;
    int LogicTypeValueIndex;
    int ToleranceIdIndex;
    int ConditionIdIndex;
    int TripMsgIndex;
    int MpsTripIndex;
    int MpsPermitIndex;
    int HeartbeatValueIndex;
    int ArmValueIndex;

    uint32_t heartbeatCnt; // Heartbeat of the guardian
    int DEVICE_PARAMS_SIZE; // number of device data pv asyn parameters - This will be seperated by mode soon
    int TOL_PARAMS_SIZE; // number of tolerance 'control' pv asyn parameters
    int CONDITION_PARAMS_SIZE; // number of device condition pv asyn parameters
    double MONITOR_CYCLE_TIME; // Used to control the sleep() in the FELpulseMonitor thread
};