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
 
using namespace std;
 
#define CounterString "COUNTER"
#define StepString "STEP"
 
class GuardianDriver : public asynPortDriver {
  public:
    GuardianDriver(const char *portName);
    void counterTask(void);
   
  protected:
    int CounterIndex;
    int StepIndex;
};