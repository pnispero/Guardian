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
                                                      asynFloat64Mask | asynInt32Mask | asynUInt32DigitalMask, // interruptMask
                                                      ASYN_MULTIDEVICE, // asynFlags
                                                      1, // autoConnect
                                                      0, // priority
                                                      0 // stackSize
                                                     ),
                                                     heartbeatCnt(0)
{
    createParam(MONITOR_CYCLE_STRING, asynParamFloat64, &MonitorCycleIndex);
    createParam(SNAPSHOT_TRIGGER_STRING, asynParamUInt32Digital, &SnapshotTriggerIndex);
    createParam(DEVICE_PARAM_SIZE_STRING, asynParamInt32, &DeviceParamSizeIndex);
    createParam(CONDITION_PARAM_SIZE_STRING, asynParamInt32, &ConditionParamSizeIndex);
    createParam(TOLERANCE_PARAM_SIZE_STRING, asynParamInt32, &ToleranceParamSizeIndex);
    createParam(STORED_VALUE_STRING, asynParamFloat64, &StoredValueIndex);
    createParam(SNAPSHOT_VALUE_STRING, asynParamFloat64, &SnapshotValueIndex);
    createParam(CONDITION_VALUE_STRING, asynParamUInt32Digital, &ConditionValueIndex);
    createParam(TOLERANCE_VALUE_STRING, asynParamFloat64, &ToleranceValueIndex);
    createParam(LOGIC_TYPE_VALUE_STRING, asynParamInt32, &LogicTypeValueIndex);
    createParam(TOLERANCE_ID_STRING, asynParamInt32, &ToleranceIdIndex);
    createParam(CONDITION_ID_STRING, asynParamInt32, &ConditionIdIndex);
    createParam(TRIP_MSG_STRING, asynParamOctet, &TripMsgIndex);
    createParam(MPS_TRIP_STRING, asynParamUInt32Digital, &MpsTripIndex);
    createParam(MPS_PERMIT_STRING, asynParamUInt32Digital, &MpsPermitIndex);
    createParam(HEARTBEAT_VALUE_STRING, asynParamInt32, &HeartbeatValueIndex);

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

/* Trip Logic functions */

// Type 1
std::tuple<bool, std::string> GuardianDriver::outsideTolerancePercentage(int paramIndex) {
    // tols = stats.BC1_Etols * 0.01;
    // qq = stats.BC1_energy_state;
    // QQ = stored.BC1_energy_setpt;
    // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
    //     trip = 1;
    //     out.message = 'BC1 energy feedback state outside stored range';
    // end

    int tolId; double curTol, curDeviceVal, desiredDeviceVal;
    getIntegerParam(paramIndex, ToleranceIdIndex, &tolId); // Get tolerance 'control' pv id
    getDoubleParam(tolId, ToleranceValueIndex, &curTol); // Tolerance 'control' pvs
    getDoubleParam(paramIndex, StoredValueIndex, &curDeviceVal); // device pvs
    getDoubleParam(paramIndex, SnapshotValueIndex, &desiredDeviceVal); // snapshot pvs
    
        std::cout << "Tol ID; " << tolId << "\n"; // TEMP

    double tolCompareVal = curTol * 0.01;

    // Ensure current device value is within desired tolerance
    if (curDeviceVal > (desiredDeviceVal*tolCompareVal + desiredDeviceVal) 
        ||  curDeviceVal < (desiredDeviceVal - desiredDeviceVal*tolCompareVal))
    {
        std::string tripMsg;
        // getStringParam() // TODO: will use waveform instead
        tripMsg = "Tripped outsideTolerancePercentage()"; // TEMP
        return std::make_tuple(true, tripMsg);
    }
    return std::make_tuple(false, "");
}

// Type 2
bool GuardianDriver::outsideAbsTolerancePercentage(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // tols = stats.L1Sphasetols * 0.01;
    // qq = abs(stats.L1S_phase_setpt);
    // QQ = abs(stored.L1S_phase_setpt);
    // if qq > (tols*QQ + QQ) || qq < (QQ - tols*QQ)
    //     trip = 1;
    //     out.message = 'L1S phase setpoint has been changed. Check FEL pulse energy.';
    //     return
    // end

    return false;
}

// Type 3
bool GuardianDriver::outsideAbsTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // tols = stats.BC2_verntols;
    // qq = abs(stats.BC2_vernier);
    // QQ = abs(stored.BC2_vernier);
    // if qq > (tols + QQ) || qq < (QQ - tols)
    //     trip = 1;
    //     out.message = 'BC2 Vernier (SXR) has been changed. Check FEL pulse energy.';
    //     return
    // end

    return false;
}

// Type 4
bool GuardianDriver::outsideDegreeTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // tols = stats.LHwaveplatetols;
    // qq = stats.LH1_waveplate;
    // QQ = stored.LH1_waveplate;
    // if abs(QQ - qq) > abs(tols*QQ)
    //     trip = 1;
    //     out.message = 'Waveplate (LH1) angle has been changed. Check FEL pulse energy';
    //     return
    // end

    return false;
}

// Type 5
bool GuardianDriver::dataUnchanged(uint32_t deviceDataId, uint32_t snapshotId) {
    // if stats.BC1_vernier ~= stored.BC1_vernier
    //     trip = 1;
    //     out.message = 'BC1 vernier setpoint has been changed. Not Allowed.';
    //     return
    // end

    return false;
}

// Type 6
bool GuardianDriver::outsideQuadsTolerance(uint32_t tolId, uint32_t deviceDataId, uint32_t snapshotId) {
    // for iquad = 1:26
    //     qq = stats.CQMQctrl(iquad);
    //     QQ = stored.CQMQctrl(iquad);
    //     tols = 0.001;
    //     if abs(QQ - qq) > abs(tols*QQ)
    //         trip = 1;
    //         out.CQMQtrip = 1;
    //         tweakedQuad = stats.CQMQpv(iquad);
    //         out.message = ['No tweaking the CQs or matching QUADs! I see you  ', char(tweakedQuad)];
    //     end
    // end

    return false;
}

bool GuardianDriver::tripSpecialCase(int paramIndex) {

    // Get value of condition
    int conditionId;
    getIntegerParam(paramIndex, ConditionIdIndex, &conditionId);
    std::cout << "Condition ID; " << conditionId << "\n"; // TEMP

    uint32_t conditionVal;
    getUIntDigitalParam(conditionId, ConditionValueIndex, &conditionVal, 1);
    std::cout << "Condition val; " << conditionVal << "\n"; // TEMP

    std::string tripMsg; // TODO: Will use waveform record for tripmsg
    getStringParam(paramIndex, TripMsgIndex, tripMsg); // TEMP, test with shorter string
    std::cout << "Trip msg; " << tripMsg << "\n"; // TEMP

    sleep(5); // TEMP
    return true; // TEMP
    return false;
}

void GuardianDriver::tripLogic() {

    // 1) Loop through every deviceParam
    bool tripped = false; std::string tripMsg;
    for (int paramIndex = 0; paramIndex < 2; paramIndex++) { // TEMP set to 2 for testing
    // for (int paramIndex = 0; paramIndex < DEVICE_PARAMS_SIZE; paramIndex++) {
    
        // get value of logic type
        int logicType;
        getIntegerParam(paramIndex, LogicTypeValueIndex, &logicType);
        std::cout << "Logic type; " << logicType << "\n"; // TEMP

        switch (logicType)
        {
        case 0: // special case
            tripped = pGDriver->tripSpecialCase(paramIndex);
            break;
        case 1:
            std::tie(tripped, tripMsg) = pGDriver->outsideTolerancePercentage(paramIndex);
            break;
        default:
            std::cout << "*ERROR* Invalid logic type number: " << logicType
            << " for parameter: " << paramIndex << "\n";                                                                                                                                                          
            break;
        }
        if (tripped) {
            // 1) write to trip PV
            setUIntDigitalParam(MpsTripIndex, 1, 1); // original trip PV

            // 2) write to MPS pv to turn off beam
            setUIntDigitalParam(MpsPermitIndex, 0, 1); // Tell MPS to turn off beam
            std::cout << "OFF BEAM\n"; // TEMP

            // // 3) write to out message pv - only on initial trip
            // setStringParam(outMessagePv, tripMsg); // TODO: This will be a waveform instead
            std::cout << "Trip Msg: " << tripMsg << "\n"; // TEMP
            sleep(5); //TEMP

            callParamCallbacks();
            return;
        }
    }

    /* Not Tripped */ 
    // 1) write to trip PV
    setUIntDigitalParam(MpsTripIndex, 0, 1); // original trip PV

    // 2) write to MPS pv to leave as is
    setDoubleParam(MpsPermitIndex, 1, 1);

    // 3) write to out message pv - only on initial trip
    // setStringParam(outMessagePv, "No Issues"); // TODO: This will be a waveform instead

    callParamCallbacks();
}


void GuardianDriver::takeSnapshot()
{
    double curVal;
    for (int paramIndex = 0; paramIndex < DEVICE_PARAMS_SIZE; paramIndex++) {

        // Get value from device storage PVs
        getDoubleParam(paramIndex, StoredValueIndex, &curVal);
        std::cout << "curVal after get : " << curVal << "\n"; 

        // Set device desired snapshot PVs to value
        setDoubleParam(paramIndex, SnapshotValueIndex, curVal);
    }
    callParamCallbacks();
}

void GuardianDriver::initGuardian() {
    /* Initialize */
    sleep(5); // IMPORTANT - Let epics records load then initialize
    std::cout << "Initializing Guardian Please Wait...\n\n";

    getIntegerParam(DeviceParamSizeIndex, &DEVICE_PARAMS_SIZE);
    getIntegerParam(ToleranceParamSizeIndex, &TOL_PARAMS_SIZE);
    getIntegerParam(ConditionParamSizeIndex, &CONDITION_PARAMS_SIZE);

    setUIntDigitalParam(MpsPermitIndex, 1, 1); // Initialize mps permit to 1
    
    callParamCallbacks(); // Call this to write the values back into epics records

    sleep(5); // Wait some time for every device data pv to populate with real values
    std::cout << "\nFinished initialization.\n\n";

}

void GuardianDriver::FELpulseEnergyMonitor(void)
{
    pGDriver->initGuardian();
    uint32_t snapshotTriggerVal;
    while (true) {
        getDoubleParam(MonitorCycleIndex, &MONITOR_CYCLE_TIME);
        sleep(MONITOR_CYCLE_TIME); // TODO: convert to ms
        
        // heartbeat
        heartbeatCnt++;

        // Check snapshot_trg param, take snapshot then reset if true
        getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1);
        if (snapshotTriggerVal == 1) {
            pGDriver->takeSnapshot();
            setUIntDigitalParam(SnapshotTriggerIndex, 0, 1);
            callParamCallbacks();

            getUIntDigitalParam(SnapshotTriggerIndex, &snapshotTriggerVal, 1); // TEMP

            std::cout << "Successfully triggered and resetted\n\n"; // TEMP
        }
        // TODO: You'd pass in 'mode' here (NC vs SC)
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