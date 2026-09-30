#include "alarm_logic.h"

sys_state_t alarm_evaluate(sys_state_t current, float value,
                           float warn, float crit, float hyst)
{
    switch (current) {
    case STATE_CRITICAL:
        if (value >= crit - hyst) return STATE_CRITICAL;
        return (value >= warn - hyst) ? STATE_WARNING : STATE_NORMAL;

    case STATE_WARNING:
        if (value >= crit)         return STATE_CRITICAL;
        if (value >= warn - hyst)  return STATE_WARNING;
        return STATE_NORMAL;

    case STATE_NORMAL:
    default:
        if (value >= crit) return STATE_CRITICAL;
        if (value >= warn) return STATE_WARNING;
        return STATE_NORMAL;
    }
}

sys_state_t alarm_worst(sys_state_t a, sys_state_t b)
{
    return (a > b) ? a : b;
}

const char *alarm_state_name(sys_state_t s)
{
    switch (s) {
    case STATE_NORMAL:   return "NORMAL";
    case STATE_WARNING:  return "WARNING";
    case STATE_CRITICAL: return "CRITICAL";
    default:             return "UNKNOWN";
    }
}
