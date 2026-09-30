#ifndef ALARM_LOGIC_H
#define ALARM_LOGIC_H

#include "app_types.h"

/* Threshold check with hysteresis (pure logic, no hardware).
 * - Escalates immediately when value >= warn / crit.
 * - De-escalates only after the value falls hyst below the threshold,
 *   so the state does not flicker around a limit. */
sys_state_t alarm_evaluate(sys_state_t current, float value,
                           float warn, float crit, float hyst);

sys_state_t alarm_worst(sys_state_t a, sys_state_t b);
const char *alarm_state_name(sys_state_t s);

#endif /* ALARM_LOGIC_H */
