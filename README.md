PID Fastest Line Follower Robot 🚗💨
This project implements a Fastest Line Follower Robot using a PID (Proportional–Integral–Derivative) control algorithm on Arduino.
The robot is optimized to follow a track with high speed, stability, and smooth turns, while handling curves, spirals, junctions, forks, and lost-line recovery.
✨ Features
✅ PID Control Algorithm for precise path following
✅ Dynamic Speed Control with ramp-up and ramp-down
✅ Curve & Spiral Detection with adaptive PID tuning
✅ Lost Line Recovery (Wiggle Mode) for robust navigation
✅ Finish Line Detection & Stop Mechanism
✅ Compatible with L298N Motor Driver and 3 IR sensors
⚡ Hardware Requirements
Arduino UNO / Nano / Mega
L298N Motor Driver
2 × DC Motors with wheels
3 × IR Sensors (Left, Center, Right)
Power Supply (Li-ion/Li-Po recommended)
Chassis with caster wheel
🔧 Pin Configuration
Component	Pin
Left Sensor	10
Center Sensor	9
Right Sensor	8
ENA (Left PWM)	3
IN1, IN2	6, 2
ENB (Right PWM)	5
IN3, IN4	4, 7
📌 Algorithm Highlights
PID Control adjusts motor speeds based on sensor error.
Curve & Spiral Detection → applies stronger PID gains (curveKP, spiralKP).
Finish Line Detection → stops when all sensors detect black.
T-Junction & Fork Handling → slows down and stabilizes.
Lost Line Recovery → wiggle based on last known error.
🔄 Flow Diagram (Algorithm + Working Principle)
flowchart TD
    A[Start] --> B[Read Left, Center, Right IR Sensors]
    B --> C{All Sensors = 1?}
    C -- Yes --> D[Finish Line Detected → Stop Motors]
    C -- No --> E{Lost Line? (All = 0)}
    E -- Yes --> F[Use Last Error + Wiggle Motion]
    E -- No --> G{Sensor State}
    G -- Left=1, Center=0, Right=0 --> H[Hard Left → Error = -2]
    G -- Left=0, Center=0, Right=1 --> I[Hard Right → Error = +2]
    G -- (Left=1,Center=1,Right=0) OR (Left=0,Center=1,Right=1) --> J[Gentle Curve (Error = -1 or +1)]
    G -- Left=0, Center=1, Right=0 --> K[Straight (Error = 0)]
    G -- Left=1, Center=0, Right=1 --> L[Ambiguous Fork → Slow Down]
    H & I & J & K & L --> M[Apply PID Control (Kp, Ki, Kd)]
    M --> N[Dynamic Speed Adjustment]
    N --> O{Curve/Spiral Detected?}
    O -- Curve --> P[Use Curve PID (curveKP, curveKD)]
    O -- Spiral --> Q[Use Spiral PID (spiralKP, spiralKD)]
    O -- No --> R[Use Normal PID (Kp, Ki, Kd)]
    P & Q & R --> S[Calculate Motor Speeds]
    S --> T[Set Motor Directions (Forward/Backward)]
    T --> U[Update PWM Speeds]
    U --> B
🚀 How It Works
The robot continuously reads 3 IR sensors.
It calculates an error value based on the line’s position.
The PID controller computes a correction.
Motor speeds are updated dynamically to minimize error.
Special handling is applied for curves, spirals, junctions, forks, and lost lines.
Robot stops when the finish line is detected.
🛠️ Future Improvements
🔹 Multi-speed optimization for different track zones
🔹 Auto-calibration of PID constants
🔹 Upgrade to 5-sensor array for higher precision
🔹 Add Bluetooth/Wi-Fi telemetry for live PID tuning
📜 License

This project is open-source and available under the MIT License.
