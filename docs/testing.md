# Test plan

## Host unit tests (no hardware)

```
gcc -std=c99 -Wall -Wextra -Iapp tests/test_logic.c app/alarm_logic.c app/dsp_utils.c app/ntc.c app/fmt_utils.c -lm -o test_logic
./test_logic
```
Covers alarm hysteresis, AC-RMS/peak, moving average, NTC conversion and fixed-point
formatting. Also runs automatically in GitHub Actions on every push.

## On-hardware tests

Fill in the *Result* column as you go.

| ID | Test | Expected | Result |
|---|---|---|---|
| TC-01 | Power on, open serial terminal at 115200 | CSV header, then one line per second | |
| TC-02 | Room temperature vs a reference thermometer | Within about ±2 °C | |
| TC-03 | Warm the NTC (finger / hair dryer) past the warning limit | State → WARNING, LED solid | |
| TC-04 | Keep heating past the critical limit | State → CRITICAL, LED blinks, buzzer beeps | |
| TC-05 | Let it cool | Returns to WARNING/NORMAL only after the hysteresis margin | |
| TC-06 | Board at rest | Vibration RMS small and stable (near noise floor) | |
| TC-07 | Tap / shake the sensor | RMS rises; WARNING / CRITICAL at the configured limits | |
| TC-08 | Unplug the MPU6050 while running | `fault` bit1 set, state ≥ WARNING within about 1 s | |
| TC-09 | Disconnect / short the NTC | `fault` bit0 set, state ≥ WARNING | |
| TC-10 | Reconnect sensors | Fault clears, values resume | |
| TC-11 | Check stack high-water marks (`uxTaskGetStackHighWaterMark`) | Every task keeps a safe margin | |
| TC-12 | 1-hour soak run | No reset, no hard fault, no drift in timing | |

## Notes to record

- Measured I2C bus load and dropped-sample count while the OLED updates
- Logic-analyzer capture of the 2 ms sampling period (jitter)
- Screenshots of the live plot during a shake test (save in `docs/images/`)
