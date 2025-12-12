# CANopen TMC5160 Motor Controller

A complete implementation of CiA 402 motion control profile using Lely CANopen Stack on STM32F407 microcontroller with bare-metal (non-HAL) drivers for TMC5160 stepper motor control.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: STM32F4](https://img.shields.io/badge/Platform-STM32F4-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f4-series.html)
[![CANopen: CiA 402](https://img.shields.io/badge/CANopen-CiA%20402-green.svg)](https://www.can-cia.org/)

## 📋 Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Project Structure](#project-structure)
- [Installation & Setup](#installation--setup)
- [Usage](#usage)
- [CANopen Implementation Details](#canopen-implementation-details)
- [Motion Control](#motion-control)

## 🎯 Project Overview

This project demonstrates a production-ready implementation of CANopen communication protocol with CiA 402 motion control profile on STM32F407 microcontroller. The implementation uses **bare-metal (non-HAL) drivers** for precise timing control and minimal memory footprint, making it suitable for integration into existing bare-metal firmware projects.

### Key Objectives

- **CANopen Integration**: Full implementation of CANopen stack using Lely library
- **CiA 402 Compliance**: Complete 8-state power drive system state machine
- **Bare-Metal Architecture**: Non-HAL drivers for deterministic real-time performance
- **TMC5160 Motor Control**: Professional stepper motor control with motion profiling
- **Dual Communication Channels**: SDO for configuration, PDO for real-time control

### Why Non-HAL?

- **Deterministic Timing**: Direct register access eliminates HAL function call overhead
- **Memory Efficiency**: 5-10× smaller code size compared to HAL-based implementations
- **Full Hardware Control**: No hidden abstraction layers
- **Firmware Integration**: Compatible with existing bare-metal codebases

## ✨ Features

### CANopen Protocol
- ✅ Full CiA 301 (CANopen Application Layer) implementation
- ✅ Network Management (NMT) with state control
- ✅ Service Data Object (SDO) for parameter configuration
- ✅ Process Data Object (PDO) for real-time data exchange
- ✅ Heartbeat producer (1000 ms interval)
- ✅ Emergency (EMCY) object support

### CiA 402 Motion Control Profile
- ✅ 8-state power drive system state machine
- ✅ Profile Position Mode (Mode 1)
- ✅ Homing Mode (Mode 6) - Method 35: Current Position as Zero
- ✅ Controlword/Statusword communication
- ✅ Safety-compliant motion triggering (rising edge detection)
- ✅ Target reached detection

### Bare-Metal Drivers
- ✅ RCC: System clock configuration (168 MHz)
- ✅ GPIO: Pin configuration for peripherals
- ✅ SysTick: 1 ms timebase for stack timing
- ✅ SPI: TMC5160 register communication (Mode 3, 1.3 MHz)
- ✅ CAN: Interrupt-driven RX with 32-message ring buffer
- ✅ TMC5160: Motion profile control with ramp generator

### Python Master Interface
- ✅ Interactive CLI for motor control
- ✅ SDO and PDO operation modes
- ✅ Real-time motion monitoring
- ✅ Parameter configuration commands
- ✅ CAN message logging support

## 🗺️ System Architecture

```
┌──────────────────────────────────────────────────────────┐
│                   Application Layer                      │
│  ┌──────────────────────────────────────────────────┐  │
│  │          Lely CANopen Stack                       │  │
│  │  ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐ │  │
│  │  │  NMT   │  │  SDO   │  │  PDO   │  │  SYNC  │ │  │
│  │  └────────┘  └────────┘  └────────┘  └────────┘ │  │
│  └──────────────────────────────────────────────────┘  │
│                                                          │
│         CiA 402 State Machine & Motion Logic            │
├──────────────────────────────────────────────────────────┤
│                    Driver Layer (Non-HAL)               │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  │
│  │   CAN   │  │   SPI   │  │ TMC5160 │  │ SysTick │  │
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘  │
├──────────────────────────────────────────────────────────┤
│                    Hardware Layer                       │
│        STM32F407 (CAN1, SPI1, GPIO, RCC, NVIC)         │
└──────────────────────────────────────────────────────────┘
```

### Communication Flow

```
Master (PC)                 STM32F407                    TMC5160
    │                           │                            │
    │ ─── CAN Message ───>      │                            │
    │                           │ (Interrupt)                │
    │                           │ → Ring Buffer              │
    │                           │ → Lely Stack               │
    │                           │ → Callback                 │
    │                           │                            │
    │                           │ ─── SPI Command ─────>     │
    │                           │                            │
    │                           │ <── SPI Response ──────    │
    │                           │                            │
    │                           │ (TPDO Trigger)             │
    │ <── CAN Response ──       │                            │
```

## 🔧 Hardware Requirements

### Core Components

| Component | Model | Purpose |
|-----------|-------|---------|
| **Microcontroller** | STM32F407VGT6 Discovery | Main controller (168 MHz, 1MB Flash, 192KB RAM) |
| **CAN Transceiver** | TCAN1042VDRQ1 | CAN bus physical layer interface |
| **Motor Driver** | TMC5160-BOB | Stepper motor driver with SPI interface |
| **Stepper Motor** | PK225PA | 200 steps/rev, 1.8° step angle |
| **CAN Adapter** | Kvaser Leaf Light v2 | USB-to-CAN interface for PC |

### Pin Connections

#### CAN Bus (PB8, PB9)
```
STM32F407          TCAN1042
PB8 (CAN_RX) ───── RXD
PB9 (CAN_TX) ───── TXD
                   CANH ───── CAN Bus High
                   CANL ───── CAN Bus Low
```

#### SPI Interface (PA4-PA7)
```
STM32F407          TMC5160
PA4 (CSN)    ───── CSN
PA5 (SCK)    ───── SCK
PA6 (MISO)   ───── SDO
PA7 (MOSI)   ───── SDI
```

#### Power Supply
- STM32: 5V via USB or external supply
- TMC5160: 12-48V motor supply voltage
- Common ground between all components

### Hardware Configuration Notes

- CAN bus requires 120Ω termination resistors at both ends
- TMC5160 clock: 12 MHz internal oscillator
- Recommended decoupling capacitors near power pins
- Heat sink recommended for TMC5160 at high currents

## 💻 Software Requirements

### Development Tools

| Tool | Version | Purpose |
|------|---------|---------|
| **STM32CubeIDE** | ≥ 1.10 | Development and debugging |
| **ARM GCC** | ≥ 10.3 | Compiler toolchain |
| **Python** | ≥ 3.8 | Master control scripts |
| **can-utils** | Latest | CAN bus monitoring (Linux) |

### Software Dependencies

#### Embedded (STM32)
```
- Lely CANopen Stack (commit 9e3267d2)
- CMSIS (STM32F4xx headers)
- No STM32 HAL required
```

#### Python Master Scripts
```bash
pip install python-can canopen colorama
```

### Supported Operating Systems

- **Firmware Flashing**: Windows, Linux, macOS (via STM32CubeIDE)
- **Python Scripts**: Linux (recommended), Windows (with SocketCAN driver)
- **CAN Monitoring**: Linux (native SocketCAN), Windows (via virtual CAN)

## 📁 Project Structure

```
CANopen_TMC/
├── Core/
│   ├── Inc/                          # Header files
│   │   ├── main.h
│   │   ├── stm32f4xx_hal_conf.h
│   │   └── stm32f4xx_it.h
│   ├── Src/
│   │   ├── main.c                    # Main application logic
│   │   ├── stm32f4xx_it.c            # Interrupt handlers
│   │   ├── syscalls.c                # POSIX syscall stubs
│   │   ├── sysmem.c                  # Memory management
│   │   └── system_stm32f4xx.c        # System initialization
│   └── Startup/
│       └── startup_stm32f407vgtx.s   # Startup assembly
│
├── Core/Src/Peripheral/              # Bare-metal drivers
│   ├── Inc/
│   │   ├── can.h                     # CAN driver header
│   │   ├── gpio.h                    # GPIO driver header
│   │   ├── rcc.h                     # Clock configuration header
│   │   ├── sdev.h                    # Object Dictionary header
│   │   ├── spi.h                     # SPI driver header
│   │   ├── systick.h                 # SysTick timer header
│   │   └── tmc5160.h                 # TMC5160 driver header
│   └── Src/
│       ├── can.c                     # CAN interrupt & ring buffer
│       ├── gpio.c                    # GPIO configuration
│       ├── rcc.c                     # 168 MHz clock setup
│       ├── sdev.c                    # Generated Object Dictionary
│       ├── spi.c                     # SPI Mode 3 implementation
│       ├── systick.c                 # 1ms timebase
│       └── tmc5160.c                 # TMC5160 register control
│
├── Drivers/                          # CMSIS & device headers
│   ├── CMSIS/
│   └── STM32F4xx_HAL_Driver/
│
├── lely-core/                        # Lely CANopen Stack (submodule)
│
├── Python/                           # Master control scripts
│   ├── script_master.py              # Interactive CLI controller
│   ├── test_pdo.py                   # PDO communication test
│   └── test_sdo.py                   # Basic SDO testing
│
├── slave.dcf                         # Object Dictionary configuration
├── .cproject                         # Eclipse CDT project
├── .project                          # Eclipse project
├── CANopen_TMC.ioc                   # STM32CubeMX config (minimal)
├── STM32F407VGTX_FLASH.ld           # Linker script
└── README.md                         # This file
```

### Key Files Description

#### Firmware Core
- **`main.c`**: CANopen stack integration, CiA 402 state machine, motion control logic
- **`sdev.c`**: Auto-generated from `slave.dcf` using Lely's `dcf2c` tool

#### Bare-Metal Drivers
- **`can.c`**: Interrupt-driven CAN RX with 32-message ring buffer, polling TX
- **`spi.c`**: SPI Mode 3 (CPOL=1, CPHA=1) for TMC5160 communication
- **`tmc5160.c`**: Register-level control of motion parameters and ramp generator

#### Python Scripts
- **`script_master.py`**: Production CLI with SDO/PDO modes, parameter configuration
- **`test_pdo.py`**: PDO communication validation and CiA 402 compliance testing

## 🚀 Installation & Setup

### 1. Clone Repository with Submodule

```bash
git clone --recursive https://github.com/aprioadam09/CANopen_TMC.git
cd CANopen_TMC
```

If you forgot `--recursive`, initialize submodule manually (make sure the commit version of lely is 9e3267d2):
```bash
git submodule update --init --recursive
```

### 2. Build Lely CANopen Stack

The Lely library must be cross-compiled for ARM Cortex-M4. Follow these steps **exactly**:

```bash
cd lely-core

# Step 1: Generate configure script
autoreconf -i

# Step 2: Create build directory
mkdir build && cd build

# Step 3: Configure for ARM bare-metal
../configure --host=arm-none-eabi \
  CFLAGS="-g -O2 -mcpu=cortex-m4 -mthumb -D_NEWLIB_ -mfpu=fpv4-sp-d16 -mfloat-abi=hard" \
  CXXFLAGS="-g -O2 -mcpu=cortex-m4 -mthumb -D_NEWLIB_ -mfpu=fpv4-sp-d16 -mfloat-abi=hard" \
  LDFLAGS="--specs=nosys.specs" \
  --disable-shared --enable-static --disable-tools --disable-tests \
  --disable-python --disable-threads --disable-daemon --disable-cxx \
  --disable-dcf --disable-master --disable-csdo --disable-gw

# Step 4: Build and install
make install DESTDIR=$(pwd)/../install

# Return to project root
cd ../..
```

**Verify Installation:**
```bash
ls lely-core/install/usr/local/lib/
# Should show: liblely-can.a, liblely-co.a, liblely-ev.a, etc.
```

### 3. Hardware Setup

1. **Connect CAN Bus**
   ```
   PC (Kvaser) ──[CAN H/L]── TCAN1042 ──[PB8/PB9]── STM32F407
   ```
   - Install 120Ω termination resistors at both ends

2. **Connect SPI to TMC5160**
   ```
   STM32 PA4-PA7 ──[SPI]── TMC5160 ──[Motor Phases]── Stepper Motor
   ```

3. **Power Supply**
   - STM32: Connect USB for programming and power
   - TMC5160: Connect 12-48V motor supply

### 4. Firmware Flashing

#### Using STM32CubeIDE (Recommended)

1. Open STM32CubeIDE
2. Import project:
   ```
   File → Open Projects from File System → Select CANopen_TMC folder
   ```
3. Build:
   ```
   Project → Build All (Ctrl+B)
   ```
4. Flash:
   ```
   Run → Debug (F11)
   ```

#### Using Command Line (OpenOCD)

```bash
# Build
make clean
make all

# Flash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/CANopen_TMC.elf verify reset exit"
```

### 5. CAN Interface Setup (Linux)

```bash
# Bring up CAN interface at 125 kbps
sudo ip link set can0 type can bitrate 125000
sudo ip link set can0 up

# Verify
ip -details link show can0
```

### 6. Python Environment Setup

```bash
# Install dependencies
pip install python-can canopen colorama

# Verify installation
python3 -c "import canopen; print('CANopen library OK')"
```

### 7. Initial Test

```bash
# Terminal 1: Monitor CAN traffic
candump can0

# Terminal 2: Run master script
cd Python
python3 script_master.py
```

Expected output:
```
================================================================
  CANopen TMC5160 Interactive CLI v1.0
  Type 'help' to see list of commands
================================================================
canopen> connect
Connecting to CAN bus...
Node 2 connected successfully
```

## 📘 Usage

### Python Master CLI

The `script_master.py` provides an interactive command-line interface for controlling the motor via CANopen.

#### Quick Start Sequence

```bash
canopen> connect          # Establish CANopen communication
canopen> enable           # Power on the motor
canopen> home             # Calibrate zero position
canopen> move 50000 wait  # Move to position 50000 and wait
canopen> status           # Check motor status
canopen> disconnect       # Safe shutdown
```

### Command Reference

#### Connection Management

| Command | Description | Example |
|---------|-------------|---------|
| `connect` | Connect to CAN bus and initialize node | `connect` |
| `disconnect` | Safely disconnect from CAN bus | `disconnect` |

#### Drive Control

| Command | Description | Example |
|---------|-------------|---------|
| `enable` | Enable drive (CiA 402 state sequence) | `enable` |
| `disable` | Disable drive (return to Shutdown) | `disable` |
| `home` | Perform homing (Method 35) | `home` |

#### Motion Commands

| Command | Description | Example |
|---------|-------------|---------|
| `move <pos> [wait]` | Move to absolute position | `move 100000`<br>`move 50000 wait` |
| `wait [timeout]` | Wait for motion completion | `wait`<br>`wait 20` |

#### Parameter Configuration

| Command | Description | Example |
|---------|-------------|---------|
| `setvel <value>` | Set profile velocity (0x6081) | `setvel 100000` |
| `setaccel <value>` | Set profile acceleration (0x6083) | `setaccel 5000` |
| `setdecel <value>` | Set profile deceleration (0x6084) | `setdecel 5000` |
| `getparams` | Display current motion parameters | `getparams` |

#### Status & Monitoring

| Command | Description | Example |
|---------|-------------|---------|
| `status` | Display comprehensive motor status | `status` |
| `getpos` | Read current motor position | `getpos` |

#### Communication Mode

| Command | Description | Example |
|---------|-------------|---------|
| `usepdo` | Switch to PDO mode (fast) | `usepdo` |
| `usesdo` | Switch to SDO mode (reliable) | `usesdo` |

### Example Session

```bash
canopen> connect
Connecting to CAN bus...
Node 2 connected successfully

canopen> enable
Enabling drive...
Drive enabled! Statusword: 0x0237

canopen> home
Starting homing operation...
Homing completed successfully! Position zeroed.

canopen> setvel 50000
Velocity set to 50000

canopen> move 100000 wait
Moving to position 100000...
Motion command sent!
Waiting for motion to complete...
Target reached (2.34s)
Actual position: 100000

canopen> usepdo
PDO mode enabled. Commands will use RPDO.

canopen> move -50000 wait
Moving to position -50000...
Target reached (3.12s)
Actual position: -50000

canopen> disconnect
Disconnecting...
Disconnected safely
```

## 📌 CANopen Implementation Details

### Object Dictionary

The Object Dictionary (OD) is the "data map" of the CANopen device, defining all accessible parameters and their properties.

#### Generation from DCF

```bash
# Convert slave.dcf to C source (already done in this project)
dcf2c slave.dcf > Core/Src/Peripheral/Src/sdev.c
```

#### Key Objects

##### Communication Objects (0x1000-0x1FFF)

| Index | Name | Type | Access | Default | Description |
|-------|------|------|--------|---------|-------------|
| 0x1000 | Device Type | UNSIGNED32 | RO | 0x00000000 | Generic device |
| 0x1001 | Error Register | UNSIGNED8 | RO | 0x00 | Error status bits |
| 0x1017 | Heartbeat Time | UNSIGNED16 | RW | 1000 | Heartbeat interval (ms) |
| 0x1018 | Identity Object | RECORD | RO | - | Vendor ID: 0x360<br>Product: TMC5160 |

##### CiA 402 Profile Objects (0x6000-0x6FFF)

| Index | Name | Type | Access | Range | Unit | Description |
|-------|------|------|--------|-------|------|-------------|
| 0x6040 | Controlword | UNSIGNED16 | RWW | - | - | Master commands to slave |
| 0x6041 | Statusword | UNSIGNED16 | RO | - | - | Slave status to master |
| 0x6060 | Modes of Operation | INTEGER8 | RWW | - | - | 1=Profile Position<br>6=Homing |
| 0x6064 | Position Actual Value | INTEGER32 | RWR | ±2³¹ | counts | Current position (from TMC5160) |
| 0x607A | Target Position | INTEGER32 | RWW | ±2³¹ | counts | Desired position |
| 0x6081 | Profile Velocity | INTEGER32 | RWW | 0 to 500M | internal units | Maps to TMC5160 VMAX |
| 0x6083 | Profile Acceleration | UNSIGNED32 | RWW | 0 to 2³²-1 | internal units | Maps to TMC5160 AMAX |
| 0x6084 | Profile Deceleration | UNSIGNED32 | RWW | 0 to 2³²-1 | internal units | Maps to TMC5160 DMAX |

**Access Type Legend:**
- **RO**: Read Only
- **RW**: Read/Write
- **RWW**: Read/Write on Write (callback triggered on write)
- **RWR**: Read/Write on Read (callback triggered on read)

### NMT State Machine

```
        ┌──────────────────┐
        │  Initialization  │
        └────────┬─────────┘
                 │ (automatic)
                 ▼
        ┌──────────────────┐
   ┌────│ Pre-Operational  │────┐
   │    └──────────────────┘    │
   │                             │
   │ Start (0x01)    Stop (0x02)│
   │                             │
   ▼                             ▼
┌──────────────┐          ┌──────────┐
│ Operational  │          │ Stopped  │
└──────────────┘          └──────────┘
        │                        │
        └───── Reset (0x81) ─────┘
                   │
                   ▼
           (Reboot to Init)
```

### PDO Configuration

#### RPDO (Master → Slave)

| PDO | COB-ID | Mapping | Purpose |
|-----|--------|---------|---------|
| **RPDO1** | 0x202 | Controlword (16-bit) | State machine commands only |
| **RPDO2** | 0x302 | Controlword + Mode (8-bit) | Switch operation modes |
| **RPDO3** | 0x402 | Controlword + Target Pos (32-bit) | Motion commands (most used) |

#### TPDO (Slave → Master)

| PDO | COB-ID | Mapping | Trigger | Update Rate |
|-----|--------|---------|---------|-------------|
| **TPDO1** | 0x182 | Statusword (16-bit) | Statusword changes | Event (immediate) |
| **TPDO2** | 0x282 | Statusword + Actual Pos (32-bit) | Timer-based | 100 ms periodic |

### CiA 402 State Machine

```
                 ┌─────────────────────┐
                 │ Not Ready to        │
                 │ Switch On           │
                 └──────────┬──────────┘
                            │ (automatic)
                            ▼
                 ┌─────────────────────┐
      ┌──────────│ Switch On           │
      │          │ Disabled            │──────────┐
      │          └──────────┬──────────┘          │
      │                     │                      │
      │          Shutdown   │  Disable Voltage    │
      │          (0x06)     │  (0x00)             │
      │                     ▼                      │
      │          ┌─────────────────────┐          │
      │          │ Ready to            │          │
      │          │ Switch On           │          │
      │          └──────────┬──────────┘          │
      │                     │                      │
      │          Switch On  │                      │
      │          (0x07)     │                      │
      │                     ▼                      │
      │          ┌─────────────────────┐          │
      │          │ Switched On         │          │
      │          └──────────┬──────────┘          │
      │                     │                      │
      │     Enable Operation│  Disable Operation  │
      │          (0x0F)     │  (0x07)             │
      │                     ▼                      │
      │          ┌─────────────────────┐          │
      │          │ Operation           │          │
      └──────────│ Enabled             │──────────┘
                 └──────────┬──────────┘
                            │
                            │ Fault Detected
                            ▼
                 ┌─────────────────────┐
                 │ Fault               │
                 └──────────┬──────────┘
                            │
                     Fault Reset (0x80)
                            │
                            └─→ (Back to Switch On Disabled)
```

## 🎮 Motion Control

### Homing Operation

Homing establishes a zero reference point for absolute positioning. This implementation uses **Method 35: Current Position as Zero**.

#### Homing Sequence

1. **Mode Switch**: Master writes `Modes of Operation (0x6060) = 6`
2. **Start Homing**: Master sets Controlword bit 4 = 1 (`0x1F`)
3. **Firmware Action**:
   ```c
   tmc5160_write_register(TMC5160_XACTUAL, 0);  // Reset position counter
   tmc5160_write_register(TMC5160_XTARGET, 0);  // Reset target
   is_homing_attained = true;
   ```
4. **Status Update**:
   - Set Statusword bit 12 (`Homing Attained`)
   - Set Statusword bit 10 (`Target Reached`)
   - Trigger TPDO1 event
5. **Mode Return**: Automatically switch back to Profile Position Mode (1)

### Profile Position Mode

Profile Position Mode (Mode 1) provides CiA 402-compliant absolute positioning with motion profiling.

#### CiA 402 Compliance: 3-Step Trigger Sequence

**Safety Feature**: Prevents accidental motion from stale commands

```
┌──────────────────────────────────────────────────────────┐
│  Step 1: Write Target Position (Motor does NOT move)   │
├──────────────────────────────────────────────────────────┤
│  Master writes 0x607A = 50000                           │
│  Firmware: on_write_target_pos() → Save to buffer       │
│  Motor: STATIONARY                                      │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│  Step 2: Clear Controlword Bit 4 (Ensure clean state)  │
├──────────────────────────────────────────────────────────┤
│  Master writes Controlword = 0x0F (Bit 4 = 0)          │
│  Firmware: Detect bit 4 state                           │
│  Motor: STATIONARY                                      │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│  Step 3: Set Bit 4 = 1 (RISING EDGE → TRIGGER!)        │
├──────────────────────────────────────────────────────────┤
│  Master writes Controlword = 0x1F (Bit 4 = 1)          │
│  Firmware: process_controlword() → Detect 0→1 edge     │
│  Firmware: execute_target_position() → Write TMC5160    │
│  Motor: MOVING                                          │
└──────────────────────────────────────────────────────────┘
```

#### Motion Command Examples

**Using SDO (Reliable but Slow):**
```bash
canopen> usesdo
canopen> move 100000 wait
Moving to position 100000...
Target reached (2.45s)
```

**Using PDO (Fast Real-Time):**
```bash
canopen> usepdo
canopen> move -50000 wait
Moving to position -50000...
Target reached (3.12s)
```
