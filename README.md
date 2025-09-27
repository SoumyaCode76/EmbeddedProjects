# EmbeddedProjects
Learn, enjoy, and grow with the metal!

## PlantUML System Design Diagrams

This repository now includes comprehensive PlantUML diagrams describing a reactive, round-robin based state machine embedded system:

### 📋 Available Diagrams

1. **`reactive_state_machine_sequence.puml`** - Main sequence diagram showing the complete system behavior
2. **`state_machine_states.puml`** - Individual state machine internal states and transitions  
3. **`system_architecture.puml`** - System architecture overview with component relationships
4. **`embedded_system_design.md`** - Comprehensive documentation explaining the system design

### 🎯 System Features

- **Round-robin scheduler** with 10ms time slices per task
- **Reactive design** responding to external interrupts and timer events
- **Three state machine tasks**: Communication Handler, Sensor Controller, System Monitor
- **STM32 HAL integration** for hardware abstraction
- **Power management** with automatic low-power modes
- **Inter-task communication** via flags and shared memory
- **Comprehensive error handling** and recovery mechanisms

### 🔧 Usage

To view the diagrams:
1. Copy the contents of any `.puml` file
2. Paste into [PlantUML Online Editor](http://www.plantuml.com/plantuml/uml/)
3. Generate PNG/SVG diagrams for documentation

### 📊 System Specifications

| Parameter | Value |
|-----------|-------|
| Task Time Slice | 10ms |
| System Cycle Time | 30ms |
| Sensor Sampling Rate | 100ms |
| Worst-case Response Time | ≤30ms |
| SysTick Resolution | 1ms |

The diagrams provide a complete reference for implementing real-time embedded systems with predictable behavior and robust error handling.
