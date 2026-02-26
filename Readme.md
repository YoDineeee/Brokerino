# BROKERINO 

> A lightweight, health-oriented MQTT broker — the communication backbone of an emotion-aware IoT system.

---

## The Big Picture

BROKERINO is not just a broker. It is one piece of a larger system designed to **detect human faces and emotions in real time** and distribute that information across a network of Raspberry Pi devices.

Think of it like this:

```
┌─────────────────────────┐
│   Main Machine          │
│  (Face & Emotion AI)    │
│                         │
│  - Captures video feed  │
│  - Detects faces        │
│  - Classifies emotions  │
│    (happy, sad, angry…) │
└────────────┬────────────┘
             │  publishes results as MQTT messages
             ▼
┌─────────────────────────┐
│      BROKERINO          │
│   (this project)        │
│                         │
│  - Receives messages    │
│  - Routes them          │
│  - Logs & stores them   │
│  - Monitors health      │
└────────────┬────────────┘
             │  delivers to subscribed devices
             ▼
┌─────────────────────────┐
│   Raspberry Pi Nodes    │
│                         │
│  - React to emotions    │
│  - Trigger outputs      │
│    (lights, sounds,     │
│     displays, motors…)  │
└─────────────────────────┘
```


---

## What is BROKERINO?

BROKERINO is a custom MQTT broker written in modern C++20, built on top of **Boost.Asio** for async networking. Rather than using an off-the-shelf broker like Mosquitto, this project implements its own broker so it can be tightly integrated with the system's health monitoring and logging needs.

### Why a custom broker?

- Full control over connection lifecycle and message routing
- Built-in health tracking (which clients are alive, latency, message rates)
- Persistent event logging via SQLite — every emotion event can be stored
- Lightweight enough to potentially run on edge devices later

---

## Core Features (Planned / In Progress)

- **MQTT protocol handling** — connect, subscribe, publish, QoS 0/1 support
- **Async I/O** — non-blocking, handles many clients simultaneously via Boost.Asio
- **Health monitoring** — tracks broker and client health metrics
- **Persistent logging** — stores events and detections in a local SQLite database
- **Structured logging** — human-readable logs via spdlog
- **JSON payload support** — parse and inspect emotion data published by the AI system

---

## Project Structure

```
health-mqtt-broker/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .github/workflows/          # CI: build & tests
│
├── include/broker/             # Public headers (future library split)
│
├── src/
│   ├── main.cpp                # Entry point: parse config, start Broker
│   ├── broker/                 # Owns io_context, ties everything together
│   │   └── broker.hpp/cpp      # Top-level Broker class
│   │
│   ├── core/                   # MQTT 3.1.1 protocol layer
│   │   ├── mqtt_packet.hpp/cpp # Parse/serialize CONNECT, PUBLISH, SUBSCRIBE, etc.
│   │   ├── mqtt_session.hpp/cpp# Per-client state: subscriptions, inflight, clean/persistent
│   │   └── mqtt_codec.hpp/cpp  # Variable-length encoding, packet framing
│   │
│   ├── networking/             # Async TCP
│   │   ├── tcp_server.hpp/cpp  # Boost.Asio acceptor + strand per connection
│   │   └── connection.hpp/cpp  # Per-socket read loop → parse → dispatch
│   │
│   ├── message/                # Pub/Sub routing
│   │   ├── message.hpp/cpp
│   │   ├── topic_trie.hpp/cpp  # Wildcard topic matching (+ and #)
│   │   └── subscription_map.hpp/cpp
│   │
│   ├── persistence/            # SQLite-backed durability
│   │   ├── storage.hpp/cpp     # Save critical messages and sessions
│   │   └── recovery.cpp        # Reload persistent state on startup
│   │
│   ├── features/               # Health-oriented broker logic
│   │   ├── priority_queue.hpp/cpp   # normal / high / critical message queues
│   │   ├── enricher.hpp/cpp         # Attach broker metadata to messages
│   │   ├── escalation.hpp/cpp       # Re-alert if no ACK within timeout
│   │   └── health_publisher.hpp/cpp # Periodic $SYS/health/... status messages
│   │
│   ├── auth/
│   │   └── simple_auth.hpp/cpp # Username/password or token validation
│   │
│   ├── logging/
│   │   └── logger.hpp          # spdlog wrapper
│   │
│   └── config/
│       └── config.hpp/cpp      # Port, persistence path, urgency topics, etc.
│
├── tests/
│   ├── unit/                   # Packet parsing, topic matching, priority queue
│   └── integration/            # Full broker tests with a real MQTT client
│
├── scripts/
│   └── run_broker.sh           # Helper for Pi deployment
│
└── docs/
    └── architecture.md         # Mermaid flow diagrams
```

---

## Dependencies

| Library | Purpose |
|---|---|
| [Boost.Asio](https://think-async.com/Asio/) | Async networking core |
| [spdlog](https://github.com/gabime/spdlog) | Fast, structured logging |
| [SQLite3](https://www.sqlite.org/) | Local event persistence |
| [nlohmann/json](https://github.com/nlohmann/json) | JSON payload parsing |
| [cpr](https://github.com/libcpr/cpr) | HTTP client (webhooks / health callbacks) |
| [GoogleTest](https://github.com/google/googletest) | Testing framework |

---

## Building

```bash
cmake -S . -B build
cmake --build build
```

To run tests:

```bash
cd build && ctest --output-on-failure
```

> **Note:** In-source builds are disabled. Always use a separate build directory.

---

## Status

🚧 **Early development** — currently focused on the broker core.  
The face detection system and Raspberry Pi integration layers are separate sub-projects that will connect to this broker once it is stable.

---

## Roadmap

Development is split into phases, keeping scope tight and each phase shippable.

**Phase 1 — Core Protocol (MQTT 3.1.1 only)**
- [ ] Packet parsing: CONNECT, CONNACK, DISCONNECT
- [ ] Packet parsing: PUBLISH, PUBACK (QoS 0 and QoS 1)
- [ ] Packet parsing: SUBSCRIBE, SUBACK, UNSUBSCRIBE
- [ ] Variable-length encoding and packet framing (`mqtt_codec`)
- [ ] Per-client session state (`mqtt_session`)

**Phase 2 — Networking**
- [ ] Async TCP acceptor with Boost.Asio (`tcp_server`)
- [ ] Per-connection read loop with strand (`connection`)
- [ ] Top-level `Broker` class owning `io_context` and all subsystems

**Phase 3 — Pub/Sub Routing**
- [ ] Topic trie with `+` and `#` wildcard support
- [ ] Subscription map (client → topics)
- [ ] Message dispatch to matched subscribers

**Phase 4 — Persistence & Recovery**
- [ ] SQLite wrapper for critical message storage
- [ ] Persistent session support (clean session = false)
- [ ] Startup recovery: reload sessions and undelivered messages

**Phase 5 — Health-Oriented Features**
- [ ] Priority message queues (normal / high / critical)
- [ ] Message enricher (attach timestamp, broker ID, source metadata)
- [ ] Escalation engine (re-alert if ACK not received within timeout)
- [ ] `$SYS/health/` periodic status publisher

**Phase 6 — Auth & Config**
- [ ] Simple username/password or token-based auth
- [ ] JSON/TOML config file (port, persistence path, urgency topic patterns)

**Phase 7 — Testing & Integration**
- [ ] Unit tests: packet parsing, topic matching, priority queue logic
- [ ] Integration tests: full broker with a real MQTT client
- [ ] CI pipeline via GitHub Actions

**Future**
- [ ] Integration with the face detection system
- [ ] Raspberry Pi client examples and deployment scripts
- [ ] MQTT 5 support (out of scope for now — treated as a separate milestone)