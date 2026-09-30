#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    STATE_NORMAL = 0,
    STATE_WARNING,
    STATE_CRITICAL
} sys_state_t;

typedef struct {
    float    temp_c;
    bool     valid;
    uint32_t tick_ms;
} temp_sample_t;

typedef struct {
    float    rms_g;      /* AC RMS of acceleration magnitude (gravity removed) */
    float    peak_g;     /* max deviation from the window mean                 */
    bool     valid;
    uint32_t tick_ms;
} vib_sample_t;

typedef struct {
    float       temp_c;
    float       vib_rms_g;
    sys_state_t temp_state;
    sys_state_t vib_state;
    sys_state_t state;       /* overall = worst of the two (or WARNING on fault) */
    bool        temp_valid;
    bool        vib_valid;
    uint32_t    uptime_ms;
} sys_status_t;

#endif /* APP_TYPES_H */
