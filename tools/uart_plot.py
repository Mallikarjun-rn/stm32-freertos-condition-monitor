#!/usr/bin/env python3
"""Live plot + optional CSV logging of the condition-monitor UART stream.

Expected line format (1 line per second):
    t_ms,temp_c,vib_rms_g,state,fault

Usage:
    python tools/uart_plot.py COM5
    python tools/uart_plot.py COM5 --baud 115200 --log logs/run1.csv
"""
import argparse
import csv
import os
from collections import deque

import matplotlib.animation as animation
import matplotlib.pyplot as plt
import serial

STATE_COLORS = {"NORMAL": "tab:green", "WARNING": "tab:orange", "CRITICAL": "tab:red"}


def parse_line(raw: bytes):
    """Return (t_s, temp, vib, state, fault) or None for headers/garbage."""
    try:
        parts = raw.decode("ascii", errors="ignore").strip().split(",")
        if len(parts) != 5 or not parts[0].isdigit():
            return None
        return int(parts[0]) / 1000.0, float(parts[1]), float(parts[2]), parts[3], int(parts[4])
    except ValueError:
        return None


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
    ap.add_argument("port", help="serial port, e.g. COM5 or /dev/ttyUSB0")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--window", type=int, default=120, help="samples kept on screen")
    ap.add_argument("--log", help="append parsed samples to this CSV file")
    args = ap.parse_args()

    ser = serial.Serial(args.port, args.baud, timeout=0.05)

    log_file = writer = None
    if args.log:
        os.makedirs(os.path.dirname(args.log) or ".", exist_ok=True)
        is_new = not os.path.exists(args.log)
        log_file = open(args.log, "a", newline="")
        writer = csv.writer(log_file)
        if is_new:
            writer.writerow(["t_s", "temp_c", "vib_rms_g", "state", "fault"])

    t, temp, vib = (deque(maxlen=args.window) for _ in range(3))
    state = {"name": "--"}

    fig, (ax_t, ax_v) = plt.subplots(2, 1, sharex=True, figsize=(9, 6))
    (line_t,) = ax_t.plot([], [], color="tab:red")
    (line_v,) = ax_v.plot([], [], color="tab:blue")
    ax_t.set_ylabel("Temperature (°C)")
    ax_v.set_ylabel("Vibration RMS (g)")
    ax_v.set_xlabel("Time (s)")
    for ax in (ax_t, ax_v):
        ax.grid(True, alpha=0.3)

    def update(_):
        while ser.in_waiting:
            sample = parse_line(ser.readline())
            if sample is None:
                continue
            ts, tc, vg, st, fault = sample
            t.append(ts); temp.append(tc); vib.append(vg)
            state["name"] = st
            if writer:
                writer.writerow(sample)
                log_file.flush()
        if t:
            line_t.set_data(t, temp)
            line_v.set_data(t, vib)
            for ax in (ax_t, ax_v):
                ax.relim(); ax.autoscale_view()
            fig.suptitle(f"State: {state['name']}", color=STATE_COLORS.get(state["name"], "black"),
                         fontweight="bold")
        return line_t, line_v

    _ani = animation.FuncAnimation(fig, update, interval=200, cache_frame_data=False)
    try:
        plt.show()
    finally:
        ser.close()
        if log_file:
            log_file.close()


if __name__ == "__main__":
    main()
