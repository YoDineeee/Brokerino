/**
 * @mainpage Brokerino - Lightweight MQTT 3.1.1 Broker
 *
 * @section intro Introduction
 *
 * **Brokerino** is a lightweight MQTT 3.1.1 broker written in **C++17**.
 *
 * It acts as the **central message bus** between my  AI emotion detection system and the Telegram bot.
 *
 * @section usecase Real-World Use Case
 *
 * Brokerino is the **main server** that connects:
 *
 * - **AI Emotion Detection System** (Python + DeepFace + OpenCV)
 * - **Telegram Bot** (receives alerts and sends them to users)
 *
 * ### How the data flows:
 *
 * 1. The AI continuously analyzes video frames.
 * 2. The **EmotionTracker** applies four strict gates before publishing:
 *    - Gate 1: Face detected
 *    - Gate 2: Emotion confidence ≥ threshold
 *    - Gate 3: Cooldown period passed (prevents spam)
 *    - Gate 4: Emotion is different from the previous one (optional)
 * 3. If all gates pass → AI publishes on topic **`emotions/detection`**
 * 4. Brokerino receives the message and forwards it to all subscribers (including the Telegram bot).
 * 5. The Telegram bot instantly sends a rich notification to the user.
 *
 * Brokerino also handles bidirectional **health monitoring** on topic **`$SYS/health/status`**.
 *
 * @section topics Topics Used
 *
 * | Topic                    | Direction          | QoS | Description                              |
 * |--------------------------|--------------------|-----|------------------------------------------|
 * | `emotions/detection`     | AI → Bot           | 1   | Emotion detection events                 |
 * | `$SYS/health/status`     | AI → Brokerino<br>Brokerino → AI | 1 | Health / online-offline status           |
 *
 * **Example payloads:**
 *
 * **emotions/detection** (QoS 1)
 * ```json
 * {
 *   "emotion": "happy",
 *   "confidence": 0.94,
 *   "timestamp": "2026-04-01T17:09:23.456Z",
 *   "frame_id": 1842
 * }
 * ```
 *
 * **$SYS/health/status** (QoS 1)
 * ```json
 * {
 *   "status": "online",
 *   "uptime_seconds": 3741,
 *   "clients_connected": 2
 * }
 * ```
 * (The AI sends `"online"` on startup and `"offline"` on graceful shutdown.)
 *
 * @section brokerino_role What Brokerino Actually Does
 *
 * Brokerino is **not** a simple pass-through. For every message it:
 * - Validates the MQTT packet
 * - Performs **wildcard matching** (`+` and `#`) using an efficient Trie
 * - Sends **PUBACK** for QoS 1 messages (guaranteed delivery)
 * - Stores the message persistently if the subscriber has a clean_session = false
 * - Restores sessions and inflight messages on restart (using SQLite)
 * - Routes the message to **all** matching subscribers (Telegram bot, future services, etc.)
 *
 * @section architecture Architecture
 *
 * ```
 *                ┌──────────────────────────────┐
 *                │     AI Emotion Detector      │
 *                │   (DeepFace + OpenCV)        │
 *                └──────────────┬───────────────┘
 *                               │
 *                emotions/detection (QoS 1)
 *                $SYS/health/status (QoS 1)
 *                               ▼
 *                    ┌─────────────────────┐
 *                    │     Brokerino       │ ← C++17 MQTT Broker
 *                    │ (persistence, Trie, │
 *                    │  sessions, PUBACK)  │
 *                    └──────────┬──────────┘
 *                               │
 *                emotions/detection (QoS 1)
 *                $SYS/health/status (QoS 1)
 *                               ▼
 *                    ┌─────────────────────┐
 *                    │    Telegram Bot     │
 *                    │   (sends alerts)    │
 *                    └─────────────────────┘
 * ```
 *
 * @section features Features
 * - Full MQTT 3.1.1 protocol (QoS 0 & 1)
 * - Persistent sessions + SQLite storage & recovery
 * - Efficient wildcard topic matching with Trie
 * - Simple authentication (username/password + token)
 * - Automatic session and message recovery after restart
 * - Thread-safe design with mutex protection
 * - Written in clean **C++17**
 *
 * @section getting_started Getting Started
 * See the project `README.md` for build and run instructions.
 *
 * @author Mohammed Dhiaeddine
 * @date April 2026
 * @version 0.1.0
 */