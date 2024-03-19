# Logic Types

## Description
Predefined logic types commonly used for checking the tolerance for devices in SXRSS Guardian. All comparisons are a form of checking the current device value to its stored value
#### Common Parameters
1. Current Value - the live/most current device value.
2. Stored Value - the desired device value set by triggering a snapshot by ops.
3. Tolerance - the amount a device is willing to tolerate before tripping.

### Special case (type 0)
Special because only occurs for one device. Usually has certain conditions and can make up multiple types of comparisons. These will be hardcoded
```
Example:
If (condition)
    ...
    If (condition2)
        ...
```

### Outside percentage tolerance (type 1)
Trip MPS if current device value is not within the tolerance (%) of the stored value.
```
tolerance = tolerance * 0.01                                // Percentage
current value > (stored value + stored value * tolerance)   // Upper limit
current value < (stored value - stored value * tolerance)   // Lower limit
```

### Outside absolute percentage tolerance  (type 2)
The same as type 1 except take the absolute values of current and stored.
```
tolerance = tolerance * 0.01                                        // Percentage
|current value| > (|stored value| + |stored value| * tolerance)     // Upper Limit
|current value| < (|stored value| - |stored value| * tolerance)     // Lower Limit
```

### Outside absolute value tolerance  (type 3)
Trip MPS if absolute current device value is not within the tolerance of the absolute stored value.
```
|current value| > (|stored value| + tolerance)      // Upper Limit
|current value| < (|stored value| - tolerance)      // Lower Limit
```

### Outside absolute difference percentage tolerance  (type 4)
Trip MPS if the absolute value of the difference of the current device value and the stored value, is not within the absolute value of the tolerance (%) of the stored value.
```
tolerance = tolerance * 0.01                                    // Percentage
|stored value - current value| > |tolerance * stored value|     // Upper & Lower limit
```
