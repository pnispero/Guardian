#include <map>


/* Type of logic used for TripParm->logic_type. Each element is a name of a function */
enum logicType {
    specialCase=0, // It is a special case if the logic only executes ONCE
    outsideTolerancePercentage,
    outsideAbsTolerancePercentage,
    outsideAbsTolerance,
    outsideDegreeTolerance,
    outsideQuadsTolerance,
    dataUnchanged
};

class TripParam {
public:
    // Constructor
    TripParam(logicType logic_type, uint32_t deviceDataId, uint32_t snapshotId, std::string tripMsg,
             uint32_t conditionId=0, uint32_t tolId=0, uint32_t specialCaseId=0, std::string otherMsg="") 
             {
                this->logic_type = logic_type;
                this->deviceDataId = deviceDataId;
                this->tripMsg = tripMsg;
                this->conditionId = conditionId;
                this->tolId = tolId;
                this->specialCaseId = specialCaseId;
                this->otherMsg = otherMsg;
             };

    /* Fields */
    logicType logic_type; // should make this a string and a bit more description
    // TODO: Then also add comments to each field here if its a certain type then to have those fields.
    //      Or you can force it (like an json structure)
    uint32_t deviceDataId; // device data pv - same id as the snapshot pv
    uint32_t tolId = 0; // Optional tolerance 'control' pv
    uint32_t conditionId = 0; // Optional (used for 'on/off' condition defined in condition pvs)
    uint32_t specialCaseId = 0; // Optional (used to specify which special case)
    std::string tripMsg; // message to print to operators when tripped
    std::string otherMsg; // message to print to operators
};

typedef std::map<uint32_t, TripParam> TripParamMap;

/* Define the trip parameter map for NC (LCLS-I) */
// TODO: Should move this declaration somewhere else, shouldn't be a global like this
// Each entry is a trip comparison case. In the original FELpulseEnergyMonitor.m
// There are about 70 comparison cases including the loops.
// Benefit of this structure: if developer wants to add a new device,
// if it matches an existing trip logic, then just add a new entry and done.
TripParamMap NCTripParamMap = {
    //  TripParam(uint32_t logic_type, uint32_t deviceDataId, uint32_t snapshotId, std::string tripMsg,
    //            uint32_t conditionId=0, uint32_t tolId=0, uint32_t specialCaseId=0, std::string otherMsg="") 

    { 1, TripParam(outsideTolerancePercentage, 31, 31, "Undulator 1 K value has changed", 0, 11) },
    { 2, TripParam(outsideTolerancePercentage, 32, 32, "Undulator 2 K value has changed", 0, 11) },
    { 3, TripParam(outsideTolerancePercentage, 33, 33, "Undulator 3 K value has changed", 0, 11) },
    { 4, TripParam(outsideTolerancePercentage, 34, 34, "Undulator 4 K value has changed", 0, 11) },
    { 5, TripParam(outsideTolerancePercentage, 35, 35, "Undulator 5 K value has changed", 0, 11) },
    { 5, TripParam(outsideTolerancePercentage, 35, 35, "Undulator 5 K value has changed", 0, 11) },
};

/* Define the trip parameter map for SC (LCLS-II) */
// TODO: When we do SC, we may want to make the IDs of the device to start at an offset
// so we can do like #define SCParamStart 71 or something like that, so
// we can utilize the same function for both NC and SC
TripParamMap SCTripParamMap = {
    { 1, TripParam(outsideTolerancePercentage, 1, 3, "tripped") },
    { 2, TripParam(outsideAbsTolerancePercentage, 1, 4, "tripped 2") }
};