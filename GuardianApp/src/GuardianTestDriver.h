// PATRICK TODO: This is used only for testing purposes
#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <cmath>
 
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>

// Normal Conducting
#define DEVICE_VALUE_NC_STRING "DEVICE_TEST_NC"
#define LOGIC_TYPE_VALUE_NC_STRING "LOGIC_TYPE_NC"
#define STORED_VALUE_NC_STRING "STORED_NC"
#define TOLERANCE_ID_NC_STRING "TOLERANCE_ID_NC"
#define TOLERANCE_VALUE_NC_STRING "TOLERANCE_NC"
#define CONDITION_ID_NC_STRING "CONDITION_ID_NC"
#define CONDITION_VALUE_NC_STRING "CONDITION_TEST_NC"
#define DEVICE_PARAMS_NC_SIZE 67

// Super Conducting
#define DEVICE_VALUE_SC_STRING "DEVICE_TEST_SC"
#define LOGIC_TYPE_VALUE_SC_STRING "LOGIC_TYPE_SC"
#define STORED_VALUE_SC_STRING "STORED_SC"
#define TOLERANCE_ID_SC_STRING "TOLERANCE_ID_SC"
#define TOLERANCE_VALUE_SC_STRING "TOLERANCE_SC"
#define CONDITION_ID_SC_STRING "CONDITION_ID_SC"
#define CONDITION_VALUE_SC_STRING "CONDITION_TEST_SC"
#define DEVICE_PARAMS_SC_SIZE 1

// Universal
#define TEST_TRIGGER_STRING "TEST_TRG"
#define DEVICE_UPDATE_STRING "DEVICE_TEST_UPDATE"
#define CONDITION_UPDATE_STRING "CONDITION_TEST_UPDATE"
#define MPS_PERMIT_STRING "MPS_PERMIT"
#define DISPLAY_MSG_STRING "DISPLAY_MSG"
#define DEVICE_ID_STRING "DEVICE_ID"
#define GUARDIAN_MODE_STRING "GUARDIAN_MODE"
#define CONDITION_PARAMS_SIZE 7 // TODO: Temp here delete once done
#define MONITOR_CYCLE_TIME 4000 // This is the guardian cycle time + 1ms

 
class GuardianTestDriver : public asynPortDriver {
  public:
    GuardianTestDriver(const char *portName);
    void GuardianTestThread(void);
    void getParameterValues(int deviceIndex, int &logicType, 
                        double &desiredVal, double &tolVal);
    void getParameterValues(int deviceIndex, double &desiredVal, double &tolVal);
    void setDeviceIndexesBasedOffMode();
    void testUpperTolerance(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex=-1);
    void testLowerTolerance(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex=-1);
    void testTolerances(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex=-1);
    void testSpecialCase(int deviceIndex, double desiredVal, double tolVal);
    void runTestCases();

    void setConditionValue(int deviceIndex, int value);
    
  protected:
    // Normal Conducting
    int DeviceTestValueNCIndex;
    int LogicTypeValueNCIndex;
    int StoredValueNCIndex;
    int ToleranceValueNCIndex;
    int ToleranceIdNCIndex;
    int ConditionTestIdNCIndex;
    int ConditionTestValueNCIndex;

    // Super Conducting
    int DeviceTestValueSCIndex;
    int LogicTypeValueSCIndex;
    int StoredValueSCIndex;
    int ToleranceValueSCIndex;
    int ToleranceIdSCIndex;
    int ConditionTestIdSCIndex;
    int ConditionTestValueSCIndex;

    // Index set at initialization based off mode
    int DeviceTestValueIndex;
    int LogicTypeValueIndex;
    int StoredValueIndex;
    int ToleranceValueIndex;
    int ToleranceIdIndex;
    int ConditionTestIdIndex;
    int ConditionTestValueIndex;

    // Universal
    int TestTriggerIndex;
    int MpsPermitIndex;
    int DeviceTestUpdateIndex;
    int DisplayMsgIndex;
    int DeviceIdIndex;
    int ConditionTestUpdateIndex;
    int GuardianModeIndex;

    int DEVICE_PARAMS_SIZE;
    uint32_t guardianMode; // 0 - Normal conducting, 1 - Super conducting
};

enum logicType {
    outsidePercentageTolerance = 1,
    outsideAbsPercentageTolerance,
    outsideAbsValueTolerance,
    outsideAbsDifferencePercentageTolerance,
    outsideCollimatorTolerance
};

enum mode {
  NC = 0,
  SC
};