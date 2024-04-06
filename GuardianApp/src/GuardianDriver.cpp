/* G*/

#include "GuardianDriver.h"
#include <unistd.h>
 
void FELpulseEnergyMonitor(void *driverPointer); //Necessary otherwise we get an error about functions being declared
void watchdog(void *driverPointer);


static GuardianDriver *pGDriver = NULL; // pGDriver - pointer to guardian driver. global
 
GuardianDriver::GuardianDriver(const char *portName) : asynPortDriver                                                      (
                                                      portName,
                                                      1000, // number to specify maximum parameter lists (starting from 0)
                                                      asynOctetMask | asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask | asynDrvUserMask, // interfaceMask
                                                      asynOctetMask | asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask, // interruptMask
                                                      ASYN_MULTIDEVICE, // asynFlags
                                                      1, // autoConnect
                                                      0, // priority
                                                      0 // stackSize
                                                     ),
                                                     heartbeatCnt(0)
{

    // Normal Conducting
    createParam(CURRENT_VALUE_NC_STRING, asynParamFloat64, &CurrentValueNCIndex);
    createParam(STORED_VALUE_NC_STRING, asynParamFloat64, &StoredValueNCIndex);
    createParam(CONDITION_VALUE_NC_STRING, asynParamUInt32Digital, &ConditionValueNCIndex);
    createParam(TOLERANCE_VALUE_NC_STRING, asynParamFloat64, &ToleranceValueNCIndex);
    createParam(LOGIC_TYPE_VALUE_NC_STRING, asynParamInt32, &LogicTypeValueNCIndex);
    createParam(TOLERANCE_ID_NC_STRING, asynParamInt32, &ToleranceIdNCIndex);
    createParam(CONDITION_ID_NC_STRING, asynParamInt32, &ConditionIdNCIndex);
    createParam(TRIP_MSG_NC_STRING, asynParamOctet, &TripMsgNCIndex);
    createParam(DEVICE_PARAM_SIZE_NC_STRING, asynParamInt32, &DeviceParamSizeNCIndex);

    // Super Conducting
    createParam(CURRENT_VALUE_SC_STRING, asynParamFloat64, &CurrentValueSCIndex);
    createParam(STORED_VALUE_SC_STRING, asynParamFloat64, &StoredValueSCIndex);
    createParam(CONDITION_VALUE_SC_STRING, asynParamUInt32Digital, &ConditionValueSCIndex);
    createParam(TOLERANCE_VALUE_SC_STRING, asynParamFloat64, &ToleranceValueSCIndex);
    createParam(LOGIC_TYPE_VALUE_SC_STRING, asynParamInt32, &LogicTypeValueSCIndex);
    createParam(TOLERANCE_ID_SC_STRING, asynParamInt32, &ToleranceIdSCIndex);
    createParam(CONDITION_ID_SC_STRING, asynParamInt32, &ConditionIdSCIndex);
    createParam(TRIP_MSG_SC_STRING, asynParamOctet, &TripMsgSCIndex);
    createParam(DEVICE_PARAM_SIZE_SC_STRING, asynParamInt32, &DeviceParamSizeSCIndex);

    // Universal
    createParam(MONITOR_CYCLE_STRING, asynParamFloat64, &MonitorCycleIndex);
    createParam(SNAPSHOT_TRIGGER_STRING, asynParamUInt32Digital, &SnapshotTriggerIndex);
    createParam(SNAPSHOT_RESET_TRIGGER_STRING, asynParamUInt32Digital, &SnapshotResetTriggerIndex);
    createParam(DISPLAY_MSG_STRING, asynParamOctet, &DisplayMsgIndex);
    createParam(MPS_PERMIT_STRING, asynParamUInt32Digital, &MpsPermitIndex);
    createParam(GUARDIAN_ON_STRING, asynParamUInt32Digital, &GuardianOnIndex);
    createParam(ARM_VALUE_STRING, asynParamUInt32Digital, &ArmValueIndex);
    createParam(SS_STRING, asynParamUInt32Digital, &SSIndex);
    createParam(TRIP_ID_STRING, asynParamInt32, &tripIdIndex);
    createParam(GUARDIAN_MODE_STRING, asynParamUInt32Digital, &GuardianModeIndex);

    asynStatus status;
    status = (asynStatus)(epicsThreadCreate("FELpulseEnergyMonitor", epicsThreadPriorityMedium, epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)::FELpulseEnergyMonitor, this) == NULL);
    if (status)
    {
        std::cout << status << "\n";
        return;
    }
}

void FELpulseEnergyMonitor(void* driverPointer)
{
    pGDriver->FELpulseEnergyMonitor();
}

// Helper function to set which asyn parameters to use based off the mode
void GuardianDriver::setDeviceIndexesBasedOffMode() {
    if (guardianMode == NC) {
        CurrentValueIndex = CurrentValueNCIndex;
        StoredValueIndex = StoredValueNCIndex;
        ConditionValueIndex = ConditionValueNCIndex;
        ToleranceValueIndex = ToleranceValueNCIndex;
        LogicTypeValueIndex = LogicTypeValueNCIndex;
        ToleranceIdIndex = ToleranceIdNCIndex;
        ConditionIdIndex = ConditionIdNCIndex;
        TripMsgIndex = TripMsgNCIndex;
        DeviceParamSizeIndex = DeviceParamSizeNCIndex;
    }
    else { 
        CurrentValueIndex = CurrentValueSCIndex;
        StoredValueIndex = StoredValueSCIndex;
        ConditionValueIndex = ConditionValueSCIndex;
        ToleranceValueIndex = ToleranceValueSCIndex;
        LogicTypeValueIndex = LogicTypeValueSCIndex;
        ToleranceIdIndex = ToleranceIdSCIndex;
        ConditionIdIndex = ConditionIdSCIndex;
        TripMsgIndex = TripMsgSCIndex;
        DeviceParamSizeIndex = DeviceParamSizeSCIndex;
    }
    getIntegerParam(DeviceParamSizeIndex, &DEVICE_PARAMS_SIZE); // Set amouunt of devices to monitor
}

// Helper function to grab tolerance, current, and stored values of device
void GuardianDriver::getDeviceParameterValues(int deviceIndex, double &tolVal, double &curDeviceVal, double &desiredDeviceVal) {
    int tolId;
    getIntegerParam(deviceIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(deviceIndex, CurrentValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(deviceIndex, StoredValueIndex, &desiredDeviceVal); // stored pvs
} 

/* Trip Logic functions */

// Type 1
bool GuardianDriver::outsidePercentageTolerance(int deviceIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.BC1_Etols * 0.01;
        // qq = stats.BC1_energy_state;
        // QQ = stored.BC1_energy_setpt;
        // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
        //     trip = 1;
        //     out.message = 'BC1 energy feedback state outside stored range';
        // end

    double tolVal, curDeviceVal, desiredDeviceVal;
    pGDriver->getDeviceParameterValues(deviceIndex, tolVal, curDeviceVal, desiredDeviceVal);

    double tolValPercent = tolVal * 0.01;

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (desiredDeviceVal*tolValPercent + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolValPercent))
    {
        return true;
    }
    return false;
}

// Overload - used for special cases (comparing 2 different device pvs)
// the current compared to the desired value within the current tolerance
bool GuardianDriver::outsidePercentageTolerance(int curValIndex, int desiredValIndex) {
    int tolId; double tolVal, curDeviceVal, desiredDeviceVal;
    getIntegerParam(curValIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(curValIndex, CurrentValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(desiredValIndex, StoredValueIndex, &desiredDeviceVal); // stored pvs

    double tolValPercent = tolVal * 0.01;

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (desiredDeviceVal*tolValPercent + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolValPercent))
    {
        return true;
    }
    return false;
}

// Type 2 - almost identical to type 1 except this takes the absolute value of the current/stored
bool GuardianDriver::outsideAbsPercentageTolerance(int deviceIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.BC1_Etols * 0.01;
        // qq = abs(stats.BC1_energy_state);
        // QQ = abs(stored.BC1_energy_setpt);
        // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
        //     trip = 1;
        //     out.message = 'BC1 energy feedback state outside stored range';
        // end

    double tolVal, curDeviceVal, desiredDeviceVal;
    pGDriver->getDeviceParameterValues(deviceIndex, tolVal, curDeviceVal, desiredDeviceVal);

    curDeviceVal = std::abs(curDeviceVal);
    desiredDeviceVal = std::abs(desiredDeviceVal);

    double tolValPercent = tolVal * 0.01;

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (desiredDeviceVal*tolValPercent + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolValPercent))
    {
        return true;
    }
    return false;
}

// Type 3
bool GuardianDriver::outsideAbsValueTolerance(int deviceIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.BC2_verntols;
        // qq = abs(stats.BC2_vernier);
        // QQ = abs(stored.BC2_vernier);
        // if qq > (tols + QQ) || qq < (QQ - tols)
        //     trip = 1;
        //     out.message = 'BC2 Vernier (SXR) has been changed. Check FEL pulse energy.';
        //     return
        // end

    double tolVal, curDeviceVal, desiredDeviceVal;
    pGDriver->getDeviceParameterValues(deviceIndex, tolVal, curDeviceVal, desiredDeviceVal);

    curDeviceVal = std::abs(curDeviceVal);
    desiredDeviceVal = std::abs(desiredDeviceVal);

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (tolVal + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - tolVal))
    {
        return true;
    }
    return false;
}

// Type 4
bool GuardianDriver::outsideAbsDifferencePercentageTolerance(int deviceIndex) {
    // Original FELpulseEnergyMonitor.m
        // tols = stats.SlottedFoiltols * 0.01;
        // qq = stats.SlottedFoil804_pos;
        // QQ = stored.SlottedFoil804_pos;
        // if abs(QQ - qq) > abs(tols*QQ)
        //     trip = 1;
        //     out.message = 'Slotted Foil 804 has moved. Check FEL pulse energy.';
        //     return
        // end

    double tolVal, curDeviceVal, desiredDeviceVal;
    pGDriver->getDeviceParameterValues(deviceIndex, tolVal, curDeviceVal, desiredDeviceVal);

    double tolValPercent = tolVal * 0.01;

    // Ensure current device value is within desired tolerance
    if (std::abs(curDeviceVal - desiredDeviceVal) > std::abs(tolValPercent*desiredDeviceVal)) {
        return true;
    }
    return false;
}

// Special case
bool GuardianDriver::outsideCollimatorTolerance() {
    int tolId; double tolVal;
    double curLeftColVal, curRightColVal, desiredLeftColVal, desiredRightColVal;
    getIntegerParam(13, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &tolVal); // Tolerance 'control' pvs
    getDoubleParam(13, CurrentValueIndex, &curLeftColVal); // device pvs
    getDoubleParam(14, CurrentValueIndex, &curRightColVal); // device pvs
    getDoubleParam(13, StoredValueIndex, &desiredLeftColVal); // stored pvs
    getDoubleParam(14, StoredValueIndex, &desiredRightColVal); // stored pvs

    // Check BC1 colls are within 0.030 mm (or user entered delta) of stored position
    if (curLeftColVal > (tolVal + desiredLeftColVal) ||
        curLeftColVal < (desiredLeftColVal - tolVal) ||
        curRightColVal > (tolVal + desiredRightColVal) ||
        curRightColVal < (desiredRightColVal - tolVal))
    {
        return true;
    }
    return false;
}

// These special cases will have hardcoded numbers here (can't generalize these unfortunately)
std::tuple<bool, std::string> GuardianDriver::tripSpecialCase(int deviceIndex) {

    // Special cases have a condition
    // Get value of condition

    uint32_t conditionVal, conditionVal2;
    bool tripped = false; std::string tripMsg;
    if (guardianMode == NC) { // Normal conducting special cases
        switch (deviceIndex) 
        {
        case 0: // devices 1,2,3 as well
            getUIntDigitalParam(1, ConditionValueIndex, &conditionVal, 1);
            getUIntDigitalParam(2, ConditionValueIndex, &conditionVal2, 1);
            if (conditionVal == 1) {
                tripped = pGDriver->outsidePercentageTolerance(0); // Check bunch charge feedback setpoint unchanged
                if (tripped) {
                    getStringParam(0, TripMsgIndex, tripMsg);
                    break;
                }
                tripped = pGDriver->outsidePercentageTolerance(1, 0); // Check bunch charge feedback state within user entered % of stored setpt
                if (tripped) {
                    getStringParam(1, TripMsgIndex, tripMsg);
                    break;
                }
            } 
            else if (conditionVal2 == 1) {
                tripped = pGDriver->outsidePercentageTolerance(2); // Check matlab bunch charge feedback setpoint unchanged
                if (tripped) {
                    getStringParam(2, TripMsgIndex, tripMsg);
                    break;
                }
                tripped = pGDriver->outsidePercentageTolerance(3, 2); // Check matlab bunch charge feedback state within user entered % of stored
                if (tripped) {
                    getStringParam(3, TripMsgIndex, tripMsg);
                    break;
                }
            }
            else { tripMsg = "WARNING: Neither bunch charge feedback active!"; }
            break;
        case 4: // devices 5,6 as well
            getUIntDigitalParam(3, ConditionValueIndex, &conditionVal, 1);
            if (conditionVal == 1) {
                tripped = pGDriver->outsidePercentageTolerance(5, 4); // Check if BC1 energy feedback is on, then check that the SXR state is within (tols)% of stored setpoint
                if (tripped) {
                    getStringParam(5, TripMsgIndex, tripMsg);
                    break;
                }
                tripped = pGDriver->outsidePercentageTolerance(6); // Check BC1 vernier setpoint unchanged 
                if (tripped) {
                    getStringParam(6, TripMsgIndex, tripMsg);
                    break;
                }
            }
            else { tripMsg = "WARNING: BC1 Energy Feedback is OFF"; }
            break;
        case 7: // devices 8,9 as well
            getUIntDigitalParam(4, ConditionValueIndex, &conditionVal, 1);
            getUIntDigitalParam(5, ConditionValueIndex, &conditionVal2, 1);
            if (conditionVal == 1) {
                // Use the appropriate setpoint (ACT is always the SXR one) and
                // check BC1 current feedback state within (tols)% of stored setpoint
                tripped = pGDriver->outsidePercentageTolerance(8, 7); 
                if (conditionVal2 == 1) {
                    tripped = pGDriver->outsidePercentageTolerance(8, 9); 
                }
                if (tripped) {
                    getStringParam(8, TripMsgIndex, tripMsg);
                    break;
                }
                tripped = pGDriver->outsidePercentageTolerance(7);
                if (tripped) {
                    getStringParam(7, TripMsgIndex, tripMsg);
                    break;
                }
            }
            else { tripMsg = "WARNING: BC1 Bunch Current Feedback is OFF"; }
            break;
        case 10: // devices 11,12 as well
            getUIntDigitalParam(6, ConditionValueIndex, &conditionVal, 1);
            getUIntDigitalParam(7, ConditionValueIndex, &conditionVal2, 1);
            if (conditionVal == 1) {
                double curVal;
                getDoubleParam(12, CurrentValueIndex, &curVal);
                if (curVal > 25000) { // only check if BLEN reading is OK
                    tripMsg = "WARNING: BC2 bunch current reading is Garbage!";
                    break;
                }
                // Using the appropriate setpoint,
                // check first that it hasn't changed, then check that the BC2 current feedback 
                // state is within (tols)% of stored setpoint
                if (conditionVal2 == 1) {
                    tripped = pGDriver->outsidePercentageTolerance(11);
                    if (tripped) {
                        getStringParam(11, TripMsgIndex, tripMsg);
                        break;
                    }
                }
                else {
                    tripped = pGDriver->outsidePercentageTolerance(10);
                    if (tripped) {
                        getStringParam(10, TripMsgIndex, tripMsg);
                        break;
                    }
                    break;
                }
            }
            else { tripMsg = "WARNING: BC2 Bunch Current Feedback is OFF"; }
            break;
        case 13: // devices 14 as well
            tripped = pGDriver->outsideCollimatorTolerance();
            if (tripped) {
                getStringParam(13, TripMsgIndex, tripMsg);
            }
            break;
        default:
            break;
        } 
    }
    else { // Super conducting special cases

    }

    return std::make_tuple(tripped, tripMsg);
}

void GuardianDriver::tripLogic() {

    // 1) Loop through every deviceParam
    bool tripped = false; std::string tripMsg;
    for (int deviceIndex = 0; deviceIndex < DEVICE_PARAMS_SIZE; deviceIndex++) {
    
        // get value of logic type
        int logicType;
        getIntegerParam(deviceIndex, LogicTypeValueIndex, &logicType);
        switch (logicType)
        {
        case 0: // special case
            std::tie(tripped, tripMsg) = pGDriver->tripSpecialCase(deviceIndex);
            break;
        case outsidePercentageToleranceEnum:
            tripped = pGDriver->outsidePercentageTolerance(deviceIndex);
            break;
        case outsideAbsPercentageToleranceEnum:
            tripped = pGDriver->outsideAbsPercentageTolerance(deviceIndex);
            break;
        case outsideAbsValueToleranceEnum:
            tripped = pGDriver->outsideAbsValueTolerance(deviceIndex);
            break;
        case outsideAbsDifferencePercentageToleranceEnum: 
            tripped = pGDriver->outsideAbsDifferencePercentageTolerance(deviceIndex);
            break;
        default:
            std::cout << "*ERROR* Invalid logic type number: " << logicType
            << " for parameter: " << deviceIndex << "\n";                                                                                                                                                          
            break; 
        }
        if (tripped) {
            // 1) get msg
            if (tripMsg == "") { // If not special case (no tripMsg returned), get msg
                getStringParam(deviceIndex, TripMsgIndex, tripMsg);
            }

            // 2) write to MPS pv to turn off beam
            setUIntDigitalParam(MpsPermitIndex, 0, 1); // Tell MPS to turn off beam
            
            // 3) write to out message pv - only on initial trip
            setStringParam(DisplayMsgIndex, tripMsg);
            setIntegerParam(tripIdIndex, deviceIndex); // Used for display
            #ifdef TEST
                std::cout << "OFF BEAM\n";
                std::cout << "Trip Msg: " << tripMsg << "\n";
            #endif

            callParamCallbacks();
            usleep(3000); // sleep 3ms to give time for epics records to be written, adjust if needed
            return;
        }
    }

    /* Not Tripped */ 
    // 1) get msg
    if (tripMsg == "") { // If not special case (no tripMsg returned), set to no issues
        tripMsg = "No Issues";
    }

    // 2) write to MPS pv to leave as is
    setUIntDigitalParam(MpsPermitIndex, 1, 1); // Tell MPS not to worry

    // 3) write to out message pv - only on initial trip
    setStringParam(DisplayMsgIndex, tripMsg);
    setIntegerParam(tripIdIndex, -1); // Used for display

    callParamCallbacks();
}

// Function that resets the stored values
void GuardianDriver::resetSnapshot()
{   
    for (int deviceIndex = 0; deviceIndex < DEVICE_PARAMS_SIZE; deviceIndex++) {

        // Set device desired stored PVs to value
        setDoubleParam(deviceIndex, StoredValueIndex, 0); 

    }
    setUIntDigitalParam(SSIndex, 1, 1); // Trigger snapshot writing to device records
    callParamCallbacks();
    setUIntDigitalParam(SSIndex, 0, 1); // Reset back to 0
    callParamCallbacks();
}

// Function that stores the current values
void GuardianDriver::takeSnapshot()
{   
    epicsFloat64 curVal, storedVal;
    for (int deviceIndex = 0; deviceIndex < DEVICE_PARAMS_SIZE; deviceIndex++) {

        // Get value from device storage PVs
        getDoubleParam(deviceIndex, CurrentValueIndex, &curVal);

        // Set device desired stored PVs to value
        setDoubleParam(deviceIndex, StoredValueIndex, curVal); 

        getDoubleParam(deviceIndex, StoredValueIndex, &storedVal);
        if (curVal != storedVal) {
            std::cout << "** Device: " << deviceIndex << ", stored val not set properly. Current: "
                     << curVal << ", Stored: " << storedVal << " **\n"; 
        }

    }
    setUIntDigitalParam(SSIndex, 1, 1); // Trigger snapshot writing to device records
    callParamCallbacks();
    setUIntDigitalParam(SSIndex, 0, 1); // Reset back to 0
    callParamCallbacks();
}

void GuardianDriver::initGuardian() {
    /* Initialize */
    sleep(3); // IMPORTANT - Let epics records load then initialize
    std::cout << "Initializing Guardian Please Wait...\n\n";

    setUIntDigitalParam(MpsPermitIndex, 1, 1); // Initialize mps permit to 1
    // Initialize trip id to -1
    setIntegerParam(tripIdIndex, -1); // Used for display
    
    callParamCallbacks(); // Call this to write the values back into epics records

    sleep(3); // Wait some time for every device data pv to populate with real values
    #ifdef TEST
        std::cout << "\n\n** TESTING **\n\n";
    #endif
}

void GuardianDriver::FELpulseEnergyMonitor(void)
{
    pGDriver->initGuardian();
    uint32_t snapshotTriggerVal, snapshotResetVal, mpsPermitVal, guardianOnVal, armVal=0, prevGuardianMode=2;
    while (true) {
        // Cycle time, snapshots, and mode switching can only occur when guardian is 'unarmed'
        // once 'armed' then tripLogic applies every cycle
        while (armVal == 0) {
            getDoubleParam(MonitorCycleIndex, &MONITOR_CYCLE_TIME);
            usleep(MONITOR_CYCLE_TIME);
            getUIntDigitalParam(ArmValueIndex, &armVal, 1);
            // Check snapshot_trg param, take snapshot if true
            getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1);
            if (snapshotTriggerVal == 1) {
                pGDriver->takeSnapshot();
                setUIntDigitalParam(SnapshotTriggerIndex, 0, 1); // Reset back to 0
                callParamCallbacks();
                usleep(3000); // sleep 3ms to give time for epics records to be written, adjust if needed
            }

            // Check snapshot_reset param, reset snapshot if true
            getUIntDigitalParam(SnapshotResetTriggerIndex, &snapshotResetVal, 1);
            if (snapshotResetVal == 1) {
                pGDriver->resetSnapshot();
                setUIntDigitalParam(SnapshotResetTriggerIndex, 0, 1); // Reset back to 0
                callParamCallbacks();
                usleep(3000); // sleep 3ms to give time for epics records to be written, adjust if needed
            }

            // Mode can only be changed while 'unarmed'
            getUIntDigitalParam(GuardianModeIndex, &guardianMode, 1);
            if (prevGuardianMode != guardianMode) {
                pGDriver->setDeviceIndexesBasedOffMode();
            }

            // guardian doesnt trip the mps if its not running 
            getUIntDigitalParam(MpsPermitIndex, &mpsPermitVal, 1);
            if (mpsPermitVal == 0) {
                setUIntDigitalParam(MpsPermitIndex, 1, 1);
                setIntegerParam(tripIdIndex, -1); // Used for display
                callParamCallbacks();
                usleep(3000); // sleep 3ms to give time for epics records to be written, adjust if needed
            }

            // tell mps guardian is not running (unarmed) if current status is running
            getUIntDigitalParam(GuardianOnIndex, &guardianOnVal, 1);
            if (guardianOnVal == 1) {
                setUIntDigitalParam(GuardianOnIndex, 0, 1);
                callParamCallbacks();
                usleep(3000); // sleep 3ms to give time for epics records to be written, adjust if needed
            }

        }
        getUIntDigitalParam(ArmValueIndex, &armVal, 1);
        usleep(MONITOR_CYCLE_TIME);

        // Tell mps guardian is running if current status is not running
        getUIntDigitalParam(GuardianOnIndex, &guardianOnVal, 1);
        if (guardianOnVal == 0) {
            setUIntDigitalParam(GuardianOnIndex, 1, 1);
            callParamCallbacks();
            usleep(3000); // sleep 3ms to give time for epics records to be written, adjust if needed
        }

        // heartbeat
        heartbeatCnt++;

        pGDriver->tripLogic();
    }
}

/* Configure the guardian driver which may include the port, filepath maybe other ports */
int GuardianDriverConfigure(const char* portName)
{
    pGDriver = new GuardianDriver(portName);
    return asynSuccess;
}
 
static const iocshArg initArg0 = {"portName", iocshArgString};
static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"GuardianDriverConfigure", 1, initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    GuardianDriverConfigure(args[0].sval);
}
void GuardianDriverRegister(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
}

extern "C"
{
    epicsExportRegistrar(GuardianDriverRegister);
}