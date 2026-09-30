# Pin mapping and CubeMX settings

Two common boards are covered. The firmware itself uses the CubeMX-generated handles
(`hadc1`, `hi2c1`, `huart1`) and user labels, so it is not tied to specific pins.

## Pins

| Function | Blue Pill (STM32F103C8T6) | Nucleo-F446RE | CubeMX label |
|---|---|---|---|
| NTC divider (ADC1_IN0) | PA0 | PA0 (A0) | - |
| I2C1 SCL (MPU6050 + OLED) | PB6 | PB8 (D15) | - |
| I2C1 SDA (MPU6050 + OLED) | PB7 | PB9 (D14) | - |
| UART TX to PC | PA9 (USART1) | PA2 (USART2, ST-LINK VCP) | - |
| UART RX from PC | PA10 (USART1) | PA3 (USART2, ST-LINK VCP) | - |
| Alarm LED | PB13 | PB13 | `ALARM_LED` |
| Buzzer (via transistor) | PB12 | PB12 | `BUZZER` |
| Heartbeat LED | PC13 (on-board, active low) | PA5 (LD2) | `STATUS_LED` |

On the Nucleo, set `APP_UART_HANDLE` to `huart2` in `app/app_config.h`.
Verify pin alternate functions in CubeMX for your exact board.

## Wiring

```
NTC divider (3.3 V ADC reference):
  3V3 ──[ 10k fixed ]──┬──[ 10k NTC ]── GND
                       └── PA0 (ADC)

I2C bus (both modules in parallel, 3.3 V):
  MPU6050 (GY-521)  VCC=3V3  GND  SCL  SDA   address 0x68 (AD0 = GND)
  SSD1306 OLED      VCC=3V3  GND  SCL  SDA   address 0x3C
  (GY-521 and most OLED modules already include pull-up resistors)

Alarm:
  PB13 ── 330R ── LED ── GND
  PB12 ── 1k ── NPN base;  buzzer between 5V/3V3 and the collector; emitter to GND
```

Use the same ground for everything. Keep the ADC wire short and away from the buzzer wiring.

## CubeMX settings

**System**
- SYS → Debug: Serial Wire; Timebase Source: a spare timer (e.g. TIM4), not SysTick
- Clock: Blue Pill 72 MHz (HSE 8 MHz + PLL); Nucleo-F446RE default or 84 MHz is fine

**ADC1**
- IN0, single conversion, software trigger, 12-bit, right aligned
- Sampling time: 71.5 cycles (F1) / 84 cycles or more (F4) because the divider is high-impedance
- STM32F1: ADC clock ≤ 14 MHz (prescaler /6 at 72 MHz)

**I2C1**: Fast Mode, 400 kHz

**USART (huart1 / huart2)**: Asynchronous, 115200 baud, 8N1

**GPIO**: outputs with the user labels in the table above (initial level low)

**FreeRTOS** (CMSIS_V1 or V2 - the app uses the native FreeRTOS API)
- `TICK_RATE_HZ` = 1000, `USE_PREEMPTION` = enabled, `USE_MUTEXES` = enabled
- `CHECK_FOR_STACK_OVERFLOW` = Option 2
- Include `vTaskDelayUntil` (and `vTaskDelay`)
- Memory allocation: dynamic. `TOTAL_HEAP_SIZE`: about 9 KB on the Blue Pill (20 KB RAM),
  16-32 KB on the Nucleo
- The default task CubeMX creates can be deleted or left idle
