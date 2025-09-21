PID Fastest Line Follower Robot 🚗💨

This project implements a **Fastest Line Follower Robot** using a **PID (Proportional–Integral–Derivative) control algorithm** on Arduino.
The robot is optimized to follow a track with **high speed, stability, and smooth turns**, while handling **curves, spirals, junctions, forks, and lost-line recovery**.
---
✨ Features
* ✅ **PID Control Algorithm** for precise path following
* ✅ **Dynamic Speed Control** with ramp-up and ramp-down
* ✅ **Curve & Spiral Detection** with adaptive PID tuning
* ✅ **Lost Line Recovery (Wiggle Mode)** for robust navigation
* ✅ **Finish Line Detection & Stop Mechanism**
* ✅ Compatible with **L298N Motor Driver** and 3 IR sensors
---
⚡ Hardware Requirements
* Arduino UNO / Nano / Mega
* L298N Motor Driver
* 2 × DC Motors with wheels
* 3 × IR Sensors (Left, Center, Right)
* Power Supply (Li-ion/Li-Po recommended)
* Chassis with caster wheel
---
🔧 Pin Configuration

| Component       | Pin  |
| --------------- | ---- |
| Left Sensor     | 10   |
| Center Sensor   | 9    |
| Right Sensor    | 8    |
| ENA (Left PWM)  | 3    |
| IN1, IN2        | 6, 2 |
| ENB (Right PWM) | 5    |
| IN3, IN4        | 4, 7 |

---

📌 Algorithm Highlights

* **PID Control** adjusts motor speeds based on sensor error.
* **Curve & Spiral Detection** → applies stronger PID gains (`curveKP`, `spiralKP`).
* **Finish Line Detection** → stops when all sensors detect black.
* **T-Junction & Fork Handling** → slows down and stabilizes.
* **Lost Line Recovery** → wiggle based on last known error.
---
🚀 How It Works

1. The robot continuously reads **3 IR sensors**.
2. It calculates an **error value** based on the line’s position.
3. The **PID controller** computes a correction.
4. Motor speeds are updated dynamically to minimize error.
5. Special handling is applied for **curves, spirals, junctions, forks, and lost lines**.
6. Robot stops when the **finish line** is detected.

---

🛠️ Future Improvements

* 🔹 Multi-speed optimization for different track zones
* 🔹 Auto-calibration of PID constants
* 🔹 Upgrade to **5-sensor array** for higher precision
* 🔹 Add **Bluetooth/Wi-Fi telemetry** for live PID tuning

---

📜 License

This project is open-source and available under the **MIT License**.
