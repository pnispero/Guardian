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
 
#define FEL_PARAMS_SIZE 56
#define SNAPSHOT_TRIGGER_STRING "SNAPSHOT_TRG"
#define STORED_VALUE_STRING "STORED"
#define SNAPSHOT_VALUE_STRING "SNAPSHOT"

 
class GuardianDriver : public asynPortDriver {
  public:
    GuardianDriver(const char *portName);
    void FELpulseEnergyMonitor(void);
    void takeSnapshot();
    
  protected:
    int SnapshotTriggerIndex;
    int StoredValueIndex;
    int SnapshotValueIndex;
};