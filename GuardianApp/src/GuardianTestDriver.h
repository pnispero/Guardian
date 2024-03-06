// PATRICK TODO: This is used only for testing purposes
#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <sstream>
 
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>

#define TEST_TRIGGER_STRING "TEST_TRG"
#define DEVICE_VALUE_STRING "DEVICE_TEST"
#define DEVICE_UPDATE_STRING "DEVICE_TEST_UPDATE"
#define LOGIC_TYPE_VALUE_STRING "LOGIC_TYPE"
#define STORED_VALUE_STRING "STORED"
#define TOLERANCE_ID_STRING "TOLERANCE_ID"
#define TOLERANCE_VALUE_STRING "TOLERANCE"
#define CONDITION_ID_STRING "CONDITION_ID"
#define CONDITION_VALUE_STRING "CONDITION_TEST"
#define CONDITION_UPDATE_STRING "CONDITION_TEST_UPDATE"
#define MPS_PERMIT_STRING "MPS_PERMIT"
#define DISPLAY_MSG_STRING "DISPLAY_MSG"
#define DEVICE_ID_STRING "DEVICE_ID"
#define DEVICE_PARAMS_SIZE 67
#define CONDITION_PARAMS_SIZE 7
#define MONITOR_CYCLE_TIME 4000 // This is the guardian cycle time + 1ms

 
class GuardianTestDriver : public asynPortDriver {
  public:
    GuardianTestDriver(const char *portName);
    void GuardianTestThread(void);
    void getParameterValues(int deviceIndex, int &logicType, 
                        double &desiredVal, double &tolVal);
    void getParameterValues(int deviceIndex, double &desiredVal, double &tolVal);
    void testUpperTolerance(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex=-1);
    void testLowerTolerance(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex=-1);
    void testTolerances(int deviceIndex, int logicType, double desiredVal, double tolVal, int specialIndex=-1);
    void testSpecialCase(int deviceIndex, double desiredVal, double tolVal);
    void runTestCases();

    void setConditionValue(int deviceIndex, int value);
    
  protected:
    int TestTriggerIndex;
    int DeviceTestValueIndex;
    int LogicTypeValueIndex;
    int StoredValueIndex;
    int ToleranceValueIndex;
    int ToleranceIdIndex;
    int MpsPermitIndex;
    int DeviceTestUpdateIndex;
    int DisplayMsgIndex;
    int DeviceIdIndex;

    int ConditionTestIdIndex;
    int ConditionTestValueIndex;
    int ConditionTestUpdateIndex;
};

enum logicType {
    outsidePercentageTolerance = 1,
    outsideAbsPercentageTolerance,
    outsideAbsValueTolerance,
    outsideAbsDifferenceTolerance,
    outsideCollimatorTolerance
};