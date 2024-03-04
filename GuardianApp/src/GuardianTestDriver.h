// PATRICK TODO: This is used only for testing purposes
#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
 
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>

#define TEST_TRIGGER_STRING "TEST_TRG"
#define DEVICE_VALUE_STRING "DEVICE_TEST"

 
class GuardianTestDriver : public asynPortDriver {
  public:
    GuardianTestDriver(const char *portName);
    void GuardianTestThread(void);
    void runTestCases();
    
  protected:
    int TestTriggerIndex;
    int DeviceTestValueIndex;
};