# Roadmap

## Phase 0 - Repository
- [x] README, LICENSE, .gitignore, CI for host tests
- [x] Architecture, pin mapping and test-plan docs
- [ ] Block diagram / wiring photo in `docs/images/`

## Phase 1 - Bring-up (one peripheral at a time)
- [ ] CubeMX project created, LED blink
- [ ] UART printf / CSV header on boot
- [ ] ADC reads the NTC; value matches a reference thermometer
- [ ] I2C scan finds MPU6050 (0x68) and OLED (0x3C)
- [ ] OLED shows "hello"

## Phase 2 - FreeRTOS integration
- [ ] `temp_task`, `vibration_task`, `alarm_task`, `display_task`, `uart_task` running
- [ ] Queues + I2C mutex working, no missed deadlines at idle
- [ ] Alarm outputs (LED / buzzer) verified for all three states
- [ ] Sensor-fault handling verified (unplug MPU6050, open/short NTC)

## Phase 3 - Robustness
- [ ] Stack high-water-mark check, `configCHECK_FOR_STACK_OVERFLOW = 2`
- [ ] Independent watchdog (IWDG) fed only when all tasks report alive
- [ ] ADC + DMA for temperature sampling
- [ ] Page-wise OLED updates so the I2C bus is never held for ~25 ms
- [ ] 1-hour soak test, no hard fault

## Phase 4 - Stretch goals
- [ ] UART command interface (`STATUS`, `SET_TEMP_WARN 55`)
- [ ] FFT of vibration data, dominant-frequency readout
- [ ] Fault log with timestamps (last N events)
- [ ] Calibration routine at startup (baseline vibration)
- [ ] Modbus RTU output
- [ ] Custom PCB (schematic + layout in `docs/`)
