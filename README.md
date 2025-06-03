🚧 ESP32 – Barrier Control with IR Sensors, Bluetooth, and HTTP Server

🔍 Description
This application runs on an ESP32 Denky board and controls a servo-driven barrier using:

Two infrared sensors to detect direction (IN / OUT),

Bluetooth Serial connection for receiving access codes from a phone,

A REST API server for verifying access permissions,

An optional periodic server check for free access (no code required).

📦 Features
Detects vehicle direction using two IR sensors (GPIO 2 and GPIO 15).

Receives access codes via Bluetooth (SerialBT) from a phone.

Sends the code and direction to a backend server (/api/send-string).

Opens the barrier if the server responds with "access": true.

Periodically checks a secondary server endpoint (/api/free-access) for auto-granted access.

Displays Bluetooth connection status and reacts to connect/disconnect events.

