# Reactive Round-Robin State Machine Embedded System

This document describes the PlantUML sequence diagrams for a reactive, round-robin based state machine embedded system designed for STM32 microcontrollers.

## System Overview

The embedded system implements a cooperative multitasking approach using:
- **Round-robin scheduler**: Provides time slices (10ms each) to different state machine tasks
- **Reactive design**: Tasks respond to external interrupts and timer events
- **State machines**: Each task is implemented as a finite state machine
- **HAL abstraction**: Uses STM32 HAL drivers for hardware peripheral access

## Architecture Components

### 1. System Timer (SysTick)
- Generates periodic interrupts (1ms resolution)
- Drives the round-robin scheduler
- Provides system timebase for task scheduling

### 2. Round-Robin Scheduler
- Manages three state machine tasks
- Allocates 10ms time slices to each task
- Handles inter-task communication via flags/queues
- Implements power management (sleep when idle)

### 3. State Machine Tasks

#### Task 1: Communication Handler
- **Purpose**: Handles UART/serial communication
- **Triggers**: External interrupts (UART RX, GPIO)
- **States**: IDLE → RECEIVING → PROCESSING → RESPONDING → IDLE
- **Error handling**: Timeout detection, data validation

#### Task 2: Sensor Controller  
- **Purpose**: Manages sensor data acquisition
- **Triggers**: Timer-based periodic execution (100ms cycles)
- **States**: INIT → IDLE → MEASURING → VALIDATING → TRANSMITTING → IDLE
- **Features**: I2C sensor communication, data validation

#### Task 3: System Monitor
- **Purpose**: Monitors system health and resources
- **Triggers**: Continuous monitoring during allocated time slices
- **States**: MONITORING → WARNING → CRITICAL → RECOVERY/SHUTDOWN
- **Functions**: Resource monitoring, error logging, recovery procedures

### 4. Hardware Abstraction Layer (HAL)
- Provides abstraction for STM32 peripherals
- Handles UART, I2C, GPIO, and timer configurations
- Implements interrupt service routines
- Manages hardware error conditions

## Key Features

### Reactive Behavior
- System responds immediately to external events through interrupts
- Event-driven state transitions
- Asynchronous processing with immediate response capability

### Round-Robin Scheduling
- Fair allocation of CPU time to all tasks
- Deterministic execution patterns
- Predictable response times (worst-case: 30ms for 3 tasks)

### Inter-Task Communication
- Shared memory regions for data exchange
- Flag-based signaling between tasks
- Queue mechanisms for buffered communication

### Error Handling
- Hierarchical error recovery (task level → system level)
- Watchdog timer integration
- Safe shutdown procedures for critical failures

### Power Management
- Automatic low-power mode when all tasks are idle
- Wake-up on external interrupts
- Dynamic clock frequency adjustment

## Timing Analysis

| Component | Timing Specification |
|-----------|---------------------|
| SysTick Interrupt | 1ms period |
| Task Time Slice | 10ms maximum |
| Round-Robin Cycle | 30ms (3 tasks × 10ms) |
| Sensor Sampling | 100ms period |
| UART Timeout | 50ms maximum |
| System Response | ≤ 30ms worst-case |

## File Structure

```
/home/runner/work/EmbeddedProjects/EmbeddedProjects/
├── reactive_state_machine_sequence.puml    # Main sequence diagram
├── state_machine_states.puml               # State machine internal states  
├── embedded_system_design.md               # This documentation file
└── README.md                              # Project overview
```

## Usage

### Viewing the Diagrams
1. Use any PlantUML viewer or online tool (e.g., plantuml.com/plantuml)
2. Copy the contents of the .puml files
3. Generate PNG/SVG diagrams for documentation

### Integration with STM32 Projects
The design patterns shown can be implemented using:
- STM32CubeMX for initial configuration
- STM32 HAL libraries for peripheral access
- FreeRTOS or custom scheduler for task management
- CMSIS-RTOS2 API for standardized RTOS interface

## Benefits of This Design

1. **Deterministic**: Predictable timing and response characteristics
2. **Scalable**: Easy to add new state machine tasks
3. **Maintainable**: Clear separation of concerns between tasks
4. **Testable**: Each state machine can be tested independently
5. **Real-time**: Guaranteed response times for critical events
6. **Power-efficient**: Automatic low-power modes during idle periods

## Implementation Notes

- Use volatile variables for shared data between tasks
- Implement proper critical sections for atomic operations
- Consider using DMA for high-throughput peripheral operations
- Add watchdog timer for system reliability
- Implement proper error logging and debugging interfaces

This design provides a robust foundation for embedded systems requiring real-time responsiveness with predictable behavior patterns.