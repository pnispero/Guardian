# Logic Types

## Description
Predefined logic types commonly used for checking the tolerance for devices in SXRSS Guardian. All comparisons are a form of checking the current device value to its stored value (the desired value operators decide to take)
### Special case (type 0)
Special because only occurs for one device. Usually has certain conditions and make up multiple types of comparisons.
### Outside percentage tolerance (type 1)
1. Trip MPS if current device value is not within the tolerance (%) of the stored value.
```
current value > (stored value * tolerance + stored value) 
current value < (stored value - stored value * tolerance)
```


### Outside absolute percentage tolerance  (type 2)
1. The same as type 1 except take the absolute values of current and stored.
```
|current value| > (|stored value| + |stored value| * tolerance) 
|current value| < (|stored value| - |stored value| * tolerance)
```

### Outside absolute value tolerance  (type 3)
1. Trip MPS if absolue current device value is not within the tolerance of the absolute stored value.
```
|current value| > (|stored value| + tolerance) 
|current value| < (|stored value| - tolerance)
```

### Outside absolute difference tolerance  (type 4)
1. Trip MPS if the absolute value of the difference of the current device value and the stored value, is not within the absolute value of the tolerance (%) of the stored value.
```
|stored value - current value| > |tolerance * stored value|
```