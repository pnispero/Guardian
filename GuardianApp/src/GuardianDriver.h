#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
#include <utility>
#include <chrono>
#include <condition_variable>
#include <tuple>
#include <string.h>
#include <cmath>
 
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>
 
// Normal Conducting
#define CURRENT_VALUE_NC_STRING "CURRENT_NC"
#define STORED_VALUE_NC_STRING "STORED_NC"
#define CONDITION_VALUE_NC_STRING "CONDITION_NC"
#define TOLERANCE_VALUE_NC_STRING "TOLERANCE_NC"
#define LOGIC_TYPE_VALUE_NC_STRING "LOGIC_TYPE_NC"
#define TOLERANCE_ID_NC_STRING "TOLERANCE_ID_NC"
#define CONDITION_ID_NC_STRING "CONDITION_ID_NC"
#define TRIP_MSG_NC_STRING "MSG_NC"
#define DEVICE_PARAM_SIZE_NC_STRING "DEVICE_PARAM_SIZE_NC"

// Super Conducting
#define CURRENT_VALUE_SC_STRING "CURRENT_SC"
#define STORED_VALUE_SC_STRING "STORED_SC"
#define CONDITION_VALUE_SC_STRING "CONDITION_SC"
#define TOLERANCE_VALUE_SC_STRING "TOLERANCE_SC"
#define LOGIC_TYPE_VALUE_SC_STRING "LOGIC_TYPE_SC"
#define TOLERANCE_ID_SC_STRING "TOLERANCE_ID_SC"
#define CONDITION_ID_SC_STRING "CONDITION_ID_SC"
#define TRIP_MSG_SC_STRING "MSG_SC"
#define DEVICE_PARAM_SIZE_SC_STRING "DEVICE_PARAM_SIZE_SC"

// Universal
#define MONITOR_CYCLE_STRING "MONITOR_CYCLE_TIME"
#define SNAPSHOT_TRIGGER_STRING "SNAPSHOT_TRG"
#define SNAPSHOT_RESET_TRIGGER_STRING "SNAPSHOT_RESET"
#define DISPLAY_MSG_STRING "DISPLAY_MSG"
#define MPS_PERMIT_STRING "MPS_PERMIT"
#define GUARDIAN_ON_STRING "GUARD_ON"
#define ARM_VALUE_STRING "ARM"
#define SS_STRING "SS_UPDATE"
#define TRIP_ID_STRING "TRIP_ID"
#define GUARDIAN_MODE_STRING "GUARDIAN_MODE"
 
class GuardianDriver : public asynPortDriver {
  public:
    GuardianDriver(const char *portName);
    void FELpulseEnergyMonitor(void);
    void takeSnapshot();
    void resetSnapshot();
    void tripLogic();

    void getDeviceParameterValues(int deviceIndex, double &tolVal, double &curDeviceVal, double &desiredDeviceVal);
    void setDeviceIndexesBasedOffMode();
    bool outsidePercentageTolerance(int deviceIndex);
    bool outsidePercentageTolerance(int curValIndex, int desiredValIndex); // overload
    bool outsideAbsPercentageTolerance(int deviceIndex);
    bool outsideAbsValueTolerance(int deviceIndex);
    bool outsideAbsDifferencePercentageTolerance(int deviceIndex);
    bool outsideCollimatorTolerance();
    std::tuple<bool, std::string> tripSpecialCase(int deviceIndex);
    void initGuardian();

  private:
    // Normal conducting
    int CurrentValueNCIndex;
    int StoredValueNCIndex;
    int ConditionValueNCIndex;
    int ToleranceValueNCIndex;
    int LogicTypeValueNCIndex;
    int ToleranceIdNCIndex;
    int ConditionIdNCIndex;
    int TripMsgNCIndex;
    int DeviceParamSizeNCIndex;

    // Super conducting
    int CurrentValueSCIndex;
    int StoredValueSCIndex;
    int ConditionValueSCIndex;
    int ToleranceValueSCIndex;
    int LogicTypeValueSCIndex;
    int ToleranceIdSCIndex;
    int ConditionIdSCIndex;
    int TripMsgSCIndex;
    int DeviceParamSizeSCIndex;
    
    // Index set at initialization based off mode
    int CurrentValueIndex;
    int StoredValueIndex;
    int ConditionValueIndex;
    int ToleranceValueIndex;
    int LogicTypeValueIndex;
    int ToleranceIdIndex;
    int ConditionIdIndex;
    int TripMsgIndex;
    int DeviceParamSizeIndex;

    // Universal
    int MonitorCycleIndex;
    int SnapshotTriggerIndex;
    int SnapshotResetTriggerIndex;
    int DisplayMsgIndex;
    int MpsPermitIndex;
    int GuardianOnIndex;
    int ArmValueIndex;
    int SSIndex;
    int tripIdIndex;
    int GuardianModeIndex;

    uint32_t heartbeatCnt; // Heartbeat of the guardian
    int DEVICE_PARAMS_SIZE; // number of device data pv asyn parameters - This will be seperated by mode soon
    double MONITOR_CYCLE_TIME; // Used to control the sleep() in the FELpulseMonitor thread
    uint32_t guardianMode; // 0 - Normal conducting, 1 - Super conducting
};

enum logicType {
    outsidePercentageToleranceEnum = 1,
    outsideAbsPercentageToleranceEnum,
    outsideAbsValueToleranceEnum,
    outsideAbsDifferencePercentageToleranceEnum
};

enum mode {
  NC = 0,
  SC
};