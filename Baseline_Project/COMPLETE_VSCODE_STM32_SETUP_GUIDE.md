# Complete Guide: VS Code Setup for STM32 Programming

## Table of Contents
1. [Prerequisites and Tools Installation](#1-prerequisites-and-tools-installation)
2. [Generate CMake Project using STM32CubeMX](#2-generate-cmake-project-using-stm32cubemx)
3. [Convert CMake Project to VS Code Workspace](#3-convert-cmake-project-to-vs-code-workspace)
4. [Install and Configure CMake Build Tools](#4-install-and-configure-cmake-build-tools)
5. [Setup Build Tasks and Debug Configuration](#5-setup-build-tasks-and-debug-configuration)
6. [Create Custom OpenOCD Configuration](#6-create-custom-openocd-configuration)
7. [Running and Debugging](#7-running-and-debugging)
8. [Build and Run vs Debug Modes](#8-build-and-run-vs-debug-modes)
9. [Serial Terminal Setup](#9-serial-terminal-setup)
10. [STM32F429ZI Specific Configuration](#10-stm32f429zi-specific-configuration)
11. [Troubleshooting and Common Fixes](#11-troubleshooting-and-common-fixes)
12. [Quick Reference](#12-quick-reference)

---

## 1. Prerequisites and Tools Installation

### 1.1 Required Software

#### A. Visual Studio Code
- **Download**: [https://code.visualstudio.com/](https://code.visualstudio.com/)
- **Installation**: Standard installation with default settings
- **Version**: Latest stable release

#### B. STM32CubeMX
- **Download**: [https://www.st.com/en/development-tools/stm32cubemx.html](https://www.st.com/en/development-tools/stm32cubemx.html)
- **Requirements**: Java 8 or higher
- **Installation**: Run installer and follow setup wizard
- **Purpose**: Generate hardware abstraction layer (HAL) code and project structure

#### C. STM32CubeCLT (Command Line Tools)
- **Download**: [https://www.st.com/en/development-tools/stm32cubeclt.html](https://www.st.com/en/development-tools/stm32cubeclt.html)
- **Components Included**:
  - ARM GCC toolchain (arm-none-eabi-gcc)
  - CMake build system
  - Ninja build tool
  - OpenOCD debugging tool
- **Installation Path**: Typically `C:\ST\STM32CubeCLT_x.x.x\`

#### D. Git (Optional but Recommended)
- **Download**: [https://git-scm.com/download/win](https://git-scm.com/download/win)
- **Purpose**: Version control and project management

### 1.2 VS Code Extensions

Install these essential extensions in VS Code:

```
1. C/C++ Extension Pack (Microsoft)
   - C/C++ IntelliSense, debugging, and code browsing
   
2. CMake Tools (Microsoft)
   - CMake language support and build integration
   
3. Cortex-Debug (marus25)
   - ARM Cortex-M debugging support
   
4. STM32 VS Code Extension (STMicroelectronics)
   - STM32 project management and integration
```

**Installation Steps**:
1. Open VS Code
2. Press `Ctrl+Shift+X` to open Extensions
3. Search for each extension name
4. Click "Install" for each extension
5. Restart VS Code after installation

### 1.3 Hardware Requirements

- **STM32 Development Board** (e.g., NUCLEO-F429ZI, STM32F429I-DISCO)
- **ST-Link Programmer** (usually built into development boards)
- **USB Cable** for connecting ST-Link to PC
- **Target MCU** (STM32F429ZI in this example)

---

## 2. Generate CMake Project using STM32CubeMX

### 2.1 Create New Project

1. **Launch STM32CubeMX**
   - Start STM32CubeMX application
   - Click "New Project" or `File → New Project`

2. **Select Target MCU**
   ```
   Method 1: Part Number Search
   - Enter part number (e.g., STM32F429ZIT6)
   - Select exact part from results
   
   Method 2: Board Selector
   - Click "Board Selector" tab
   - Choose your development board
   - STM32CubeMX will auto-configure MCU
   ```

3. **Accept Board Configuration**
   - Click "Yes" when prompted to initialize peripherals to default mode
   - This auto-configures pins for your specific board

### 2.2 Configure Peripherals

#### A. System Configuration
1. **Clock Configuration**
   - Click "Clock Configuration" tab
   - Set HCLK to maximum frequency (e.g., 180 MHz for F429)
   - Configure PLL settings automatically or manually

2. **Debug Configuration**
   - Go to "Pinout & Configuration" tab
   - Expand "System Core" → "SYS"
   - Set "Debug" to "Serial Wire" (SWD)

#### B. Peripheral Configuration (Example)
1. **UART for Debug Output**
   - Expand "Connectivity" → "USART1"
   - Set "Mode" to "Asynchronous"
   - Configure baud rate (e.g., 115200)

2. **GPIO for LED**
   - Click on GPIO pins in pinout view
   - Set LED pins to "GPIO_Output"
   - Configure as Push-Pull, No Pull

3. **Timer (Optional)**
   - Expand "Timers" → "TIM2"
   - Configure for your application needs

### 2.3 Project Settings and Code Generation

1. **Project Manager Tab**
   ```
   Project Settings:
   - Project Name: YourProjectName
   - Project Location: Choose directory
   - Toolchain/IDE: CMake
   ```

2. **Code Generator Tab**
   ```
   Essential Settings:
   ✓ Generate peripheral initialization as a pair of '.c/.h' files per peripheral
   ✓ Keep User Code when re-generating
   ✓ Delete previously generated files when not re-generated
   
   HAL Settings:
   ✓ Set all free pins as analog (to reduce power consumption)
   ✓ Enable Full Assert
   ```

3. **Generate Code**
   - Click "GENERATE CODE" button
   - Wait for code generation to complete
   - Click "Open Project" when prompted

### 2.4 Generated Project Structure

After generation, you'll have:
```
YourProject/
├── CMakeLists.txt          # Main CMake configuration
├── CMakePresets.json       # CMake build presets
├── YourProject.ioc        # STM32CubeMX project file
├── Core/
│   ├── Inc/               # Header files
│   │   ├── main.h
│   │   ├── stm32f4xx_hal_conf.h
│   │   └── ...
│   └── Src/               # Source files
│       ├── main.c
│       ├── stm32f4xx_hal_msp.c
│       └── ...
├── Drivers/               # STM32 HAL drivers
│   ├── STM32F4xx_HAL_Driver/
│   └── CMSIS/
└── cmake/                 # CMake configuration files
    └── gcc-arm-none-eabi.cmake
```

---

## 3. Convert CMake Project to VS Code Workspace

### 3.1 Open Project in VS Code

1. **Launch VS Code**
2. **Open Folder**
   - `File → Open Folder` or `Ctrl+K, Ctrl+O`
   - Navigate to your STM32CubeMX generated project
   - Select the root folder containing `CMakeLists.txt`

3. **Trust the Workspace**
   - VS Code will prompt about trusted workspace
   - Click "Yes, I trust the authors"

### 3.2 Configure CMake Integration

1. **CMake Tools Extension Setup**
   - VS Code should auto-detect CMakeLists.txt
   - You'll see CMake tools in the status bar
   - If not detected, press `Ctrl+Shift+P` → "CMake: Configure"

2. **Select Toolkit**
   - CMake will prompt to select a kit
   - Choose "GCC for arm-none-eabi" (from STM32CubeCLT)
   - If not available, follow section 4.2 to configure

3. **Configure Project**
   - Press `Ctrl+Shift+P` → "CMake: Configure"
   - Or click "Configure" in CMake status bar
   - Choose "Debug" preset when prompted

### 3.3 Create VS Code Workspace File

1. **Save Workspace**
   ```
   File → Save Workspace As...
   Name: YourProject.code-workspace
   ```

2. **Workspace Configuration**
   Create `.vscode/` folder in project root:
   ```
   YourProject/
   ├── .vscode/
   │   ├── settings.json      # VS Code settings
   │   ├── tasks.json         # Build tasks
   │   ├── launch.json        # Debug configurations
   │   └── c_cpp_properties.json  # IntelliSense config
   └── ...
   ```

### 3.4 Configure IntelliSense

Create `.vscode/c_cpp_properties.json`:
```json
{
    "configurations": [
        {
            "name": "STM32",
            "includePath": [
                "${workspaceFolder}/Core/Inc",
                "${workspaceFolder}/Drivers/STM32F4xx_HAL_Driver/Inc",
                "${workspaceFolder}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy",
                "${workspaceFolder}/Drivers/CMSIS/Device/ST/STM32F4xx/Include",
                "${workspaceFolder}/Drivers/CMSIS/Include"
            ],
            "defines": [
                "STM32F429xx",
                "USE_HAL_DRIVER"
            ],
            "compilerPath": "D:/ST/STM32CubeCLT_1.16.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-arm"
        }
    ],
    "version": 4
}
```

---

## 4. Install and Configure CMake Build Tools

### 4.1 Understanding CMake Build Tools

#### What is CMake?
- **Cross-platform build system generator**
- **Generates native build files** (Makefiles, Ninja files, etc.)
- **Handles complex dependencies** and library linking
- **Supports multiple toolchains** (GCC, Clang, MSVC)

#### CMake Components for STM32:
1. **CMake Core** - Build system generator
2. **ARM GCC Toolchain** - Cross-compiler for ARM Cortex-M
3. **Ninja Build Tool** - Fast parallel build executor
4. **OpenOCD** - On-chip debugger and programmer

### 4.2 Installation Methods

#### Method 1: STM32CubeCLT (Recommended)
STM32CubeCLT includes everything needed:

```bash
# Installation includes:
- CMake 3.28+
- ARM GCC Toolchain 12.3
- Ninja Build Tool
- OpenOCD 0.12+
- Python 3.11+

# Default Installation Path:
C:\ST\STM32CubeCLT_1.16.0\
├── CMake\              # CMake build system
├── GNU-tools-for-STM32\ # ARM GCC toolchain
├── OpenOCD\            # Debugging tools
└── Python\             # Python interpreter
```

**Environment Setup**:
```powershell
# Add to System PATH:
C:\ST\STM32CubeCLT_1.16.0\CMake\bin
C:\ST\STM32CubeCLT_1.16.0\GNU-tools-for-STM32\bin
C:\ST\STM32CubeCLT_1.16.0\OpenOCD\bin
```

#### Method 2: Individual Installation
If not using STM32CubeCLT:

1. **CMake**
   ```
   Download: https://cmake.org/download/
   Minimum Version: 3.20
   Installation: Add to PATH during installation
   ```

2. **ARM GCC Toolchain**
   ```
   Download: https://developer.arm.com/downloads/-/gnu-rm
   Version: arm-none-eabi-gcc 12.3 or later
   Components: Compiler, linker, debugger, utilities
   ```

3. **Ninja Build Tool**
   ```
   Download: https://ninja-build.org/
   Purpose: Fast parallel builds
   Alternative: Can use Make instead
   ```

4. **OpenOCD**
   ```
   Download: https://github.com/xpack-dev-tools/openocd-xpack
   Version: 0.12+ recommended
   Purpose: On-chip debugging and programming
   ```

### 4.3 Verify Installation

Create a verification script `verify_tools.ps1`:
```powershell
# Tool Verification Script
Write-Host "=== STM32 Development Tools Verification ===" -ForegroundColor Green

# Check CMake
try {
    $cmake = cmake --version | Select-Object -First 1
    Write-Host "✓ CMake: $cmake" -ForegroundColor Green
} catch {
    Write-Host "✗ CMake not found" -ForegroundColor Red
}

# Check ARM GCC
try {
    $gcc = arm-none-eabi-gcc --version | Select-Object -First 1
    Write-Host "✓ ARM GCC: $gcc" -ForegroundColor Green
} catch {
    Write-Host "✗ ARM GCC not found" -ForegroundColor Red
}

# Check Ninja
try {
    $ninja = ninja --version
    Write-Host "✓ Ninja: $ninja" -ForegroundColor Green
} catch {
    Write-Host "✗ Ninja not found" -ForegroundColor Red
}

# Check OpenOCD
try {
    $openocd = & "C:\ST\STM32CubeCLT_1.16.0\OpenOCD\bin\openocd.exe" --version 2>&1 | Select-Object -First 1
    Write-Host "✓ OpenOCD: $openocd" -ForegroundColor Green
} catch {
    Write-Host "✗ OpenOCD not found" -ForegroundColor Red
}

Write-Host "=== Verification Complete ===" -ForegroundColor Green
```

### 4.4 CMake Configuration

#### Understanding CMakePresets.json
STM32CubeMX generates this file with build configurations:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "Debug",
            "hidden": false,
            "displayName": "Debug",
            "description": "Debug build configuration",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/Debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_TOOLCHAIN_FILE": {
                    "type": "FILEPATH",
                    "value": "${sourceDir}/cmake/gcc-arm-none-eabi.cmake"
                }
            }
        }
    ],
    "buildPresets": [
        {
            "name": "Debug",
            "configurePreset": "Debug"
        }
    ]
}
```

#### Toolchain File (gcc-arm-none-eabi.cmake)
This file tells CMake how to use ARM GCC:

```cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain settings
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Compiler flags for STM32F429xx
set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${COMMON_FLAGS}")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage")
```

---

## 5. Setup Build Tasks and Debug Configuration

### 5.1 Configure Build Tasks (.vscode/tasks.json)

Create comprehensive build tasks:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Configure",
            "type": "shell",
            "command": "cmake",
            "args": [
                "--preset",
                "Debug"
            ],
            "group": "build",
            "problemMatcher": [],
            "detail": "Configure CMake build system"
        },
        {
            "label": "Build",
            "type": "shell",
            "command": "cmake",
            "args": [
                "--build",
                "build/Debug",
                "--config",
                "Debug",
                "--parallel",
                "4"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"],
            "detail": "Build STM32 project",
            "dependsOn": "CMake Configure"
        },
        {
            "label": "Clean",
            "type": "shell",
            "command": "cmake",
            "args": [
                "--build",
                "build/Debug",
                "--target",
                "clean"
            ],
            "group": "build",
            "problemMatcher": [],
            "detail": "Clean build artifacts"
        },
        {
            "label": "Flash",
            "type": "shell",
            "command": "openocd",
            "args": [
                "-f", "interface/stlink.cfg",
                "-f", "target/stm32f4x.cfg",
                "-c", "program build/Debug/YourProject.elf verify reset exit"
            ],
            "group": "build",
            "problemMatcher": [],
            "detail": "Flash firmware to STM32",
            "dependsOn": "Build"
        },
        {
            "label": "Reset Target",
            "type": "shell",
            "command": "openocd",
            "args": [
                "-f", "interface/stlink.cfg",
                "-f", "target/stm32f4x.cfg",
                "-c", "init; reset; resume; shutdown"
            ],
            "group": "build",
            "problemMatcher": [],
            "detail": "Reset and resume STM32 target"
        }
    ]
}
```

### 5.2 Debug Configuration (.vscode/launch.json)

Create debug configurations for different scenarios:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug STM32F429ZI",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/build/Debug/YourProject.elf",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "openocd",
            "configFiles": [
                "openocd_custom.cfg"
            ],
            "searchDir": [
                "${workspaceFolder}",
                "C:/ST/STM32CubeCLT_1.16.0/OpenOCD/share/openocd/scripts"
            ],
            "runToEntryPoint": "main",
            "showDevDebugOutput": "parsed",
            "device": "STM32F429ZI",
            "interface": "swd",
            "svdFile": "${workspaceFolder}/STM32F429.svd",
            "preLaunchTask": "Build"
        },
        {
            "name": "Attach to Running Target",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/build/Debug/YourProject.elf",
            "request": "attach",
            "type": "cortex-debug",
            "servertype": "openocd",
            "configFiles": [
                "openocd_custom.cfg"
            ],
            "searchDir": [
                "${workspaceFolder}",
                "C:/ST/STM32CubeCLT_1.16.0/OpenOCD/share/openocd/scripts"
            ],
            "device": "STM32F429ZI",
            "interface": "swd",
            "svdFile": "${workspaceFolder}/STM32F429.svd"
        }
    ]
}
```

### 5.3 VS Code Settings (.vscode/settings.json)

Configure project-specific settings:

```json
{
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build/Debug",
    "cmake.generator": "Ninja",
    "cmake.defaultVariants": {
        "buildType": {
            "default": "Debug",
            "choices": {
                "Debug": {
                    "short": "Debug",
                    "long": "Debug build",
                    "buildType": "Debug"
                },
                "Release": {
                    "short": "Release",
                    "long": "Release build",
                    "buildType": "Release"
                }
            }
        }
    },
    "cortex-debug.openocdPath": "C:/ST/STM32CubeCLT_1.16.0/OpenOCD/bin/openocd.exe",
    "cortex-debug.armToolchainPath": "C:/ST/STM32CubeCLT_1.16.0/GNU-tools-for-STM32/bin",
    "files.associations": {
        "*.h": "c",
        "*.c": "c"
    },
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

---

## 6. Create Custom OpenOCD Configuration

### 6.1 Understanding OpenOCD Configuration

OpenOCD (Open On-Chip Debugger) uses configuration files to:
- **Define hardware interface** (ST-Link, J-Link, etc.)
- **Specify target MCU** (STM32F429ZI, etc.)
- **Configure debugging behavior** (reset handling, flash programming)
- **Set up event handlers** (disconnect, attach, reset)

### 6.2 Basic Configuration Structure

Create `openocd_custom.cfg` in project root:

```tcl
# Custom OpenOCD Configuration for STM32F429ZI
# File: openocd_custom.cfg

#################################
# Interface Configuration
#################################
source [find interface/stlink.cfg]

# ST-Link interface settings
transport select hla_swd
hla_layout stlink
hla_device_desc "ST-LINK"
hla_vid_pid 0x0483 0x374b 0x0483 0x3748

# Set SWD speed (can be adjusted for stability)
adapter speed 2000

#################################
# Target Configuration  
#################################
source [find target/stm32f4x.cfg]

# Override default settings for F429ZI
set CHIPNAME stm32f429zit6
set BOARDNAME generic_stm32f429zi

# Flash configuration for STM32F429ZI
# 2MB Flash, 256KB RAM (192KB + 64KB CCM)
set FLASH_SIZE 0x200000
set RAM_SIZE   0x40000
set CCM_SIZE   0x10000

#################################
# Custom Event Handlers
#################################

# Handle GDB disconnect - auto resume target
$_TARGETNAME configure -event gdb-detach {
    echo "GDB disconnected - resetting and resuming target"
    catch {
        reset
        resume
    }
}

# Handle GDB attach
$_TARGETNAME configure -event gdb-attach {
    echo "GDB attached to target"
    halt
}

# Handle reset events
$_TARGETNAME configure -event reset-start {
    echo "Reset started"
}

$_TARGETNAME configure -event reset-end {
    echo "Reset completed"
}

#################################
# Programming Configuration
#################################

# Flash programming settings
flash bank $_FLASHNAME stm32f2x 0x08000000 $_FLASH_SIZE 0 0 $_TARGETNAME

# Faster programming
$_FLASHNAME configure -work-area-phys 0x20000000
$_FLASHNAME configure -work-area-size 0x20000
$_FLASHNAME configure -work-area-backup 0

#################################
# Debug Configuration
#################################

# Enable SWO (Serial Wire Output) for printf debugging
tpiu config internal swo.log uart off 180000000
itm port 0 on

# Custom reset procedure
proc reset_target {} {
    echo "Custom reset procedure"
    reset halt
    sleep 100
    reset run
}

#################################
# Helper Procedures
#################################

# Procedure to program flash
proc program_flash {filename} {
    echo "Programming flash with $filename"
    halt
    flash write_image erase $filename 0x08000000
    verify_image $filename 0x08000000
    reset run
    echo "Programming completed successfully"
}

# Procedure to erase flash
proc erase_flash {} {
    echo "Erasing entire flash"
    halt
    flash erase_address 0x08000000 $FLASH_SIZE
    echo "Flash erase completed"
}

# Procedure to read device info
proc device_info {} {
    echo "=== STM32F429ZI Device Information ==="
    halt
    set idcode [dap info 0]
    echo "IDCODE: $idcode"
    
    # Read device ID
    set dev_id [read_memory 0xE0042000 32 1]
    echo "Device ID: [format 0x%08x $dev_id]"
    
    # Read revision ID  
    set rev_id [expr {($dev_id >> 16) & 0xFFFF}]
    echo "Revision ID: [format 0x%04x $rev_id]"
    
    # Read flash size
    set flash_size [read_memory 0x1FFF7A22 16 1]
    echo "Flash Size: ${flash_size}KB"
    
    resume
    echo "=================================="
}

echo "Custom STM32F429ZI OpenOCD configuration loaded"
```

### 6.3 Advanced Configuration Examples

#### A. Multi-Core Debugging (if applicable)
```tcl
# For STM32H7 dual-core or similar
set DUAL_CORE 1
set CORE_COUNT 2

if {$DUAL_CORE} {
    target create stm32h7x.cpu0 cortex_m -endian little -chain-position stm32h7x.dap -coreid 0
    target create stm32h7x.cpu1 cortex_m -endian little -chain-position stm32h7x.dap -coreid 1
}
```

#### B. External Flash Configuration  
```tcl
# QSPI Flash configuration (if using external flash)
flash bank qspi_flash stmqspi 0x90000000 0 0 0 stm32f429zit6.cpu quadspi
```

#### C. RTT (Real-Time Transfer) Configuration
```tcl
# Enable RTT for debugging output
rtt setup 0x20000000 0x40000 "SEGGER RTT"
rtt start
rtt server start 9090 0
```

### 6.4 Target-Specific Configurations

#### For STM32F407VG:
```tcl
source [find interface/stlink.cfg]
source [find target/stm32f4x.cfg]

# F407VG specific settings
set FLASH_SIZE 0x100000  # 1MB Flash
set RAM_SIZE   0x20000   # 128KB RAM
set CCM_SIZE   0x10000   # 64KB CCM
```

#### For STM32H743ZI:
```tcl
source [find interface/stlink.cfg]
source [find target/stm32h7x.cfg]

# H743ZI specific settings  
set FLASH_SIZE 0x200000  # 2MB Flash
set RAM_SIZE   0x20000   # 128KB AXI SRAM
adapter speed 8000       # Higher speed supported
```

#### For STM32L476RG:
```tcl
source [find interface/stlink.cfg]
source [find target/stm32l4x.cfg]

# L476RG specific settings
set FLASH_SIZE 0x100000  # 1MB Flash
set RAM_SIZE   0x18000   # 96KB SRAM1
adapter speed 4000       # Medium speed for low power
```

### 6.5 Custom Commands and Macros

Add useful custom commands to your config:

```tcl
# Custom command to show memory usage
proc show_memory {} {
    echo "=== Memory Information ==="
    
    # Stack pointer
    set sp [read_memory 0x20030000 32 1]
    echo "Stack Pointer: [format 0x%08x $sp]"
    
    # Heap usage (if implemented)
    # Add your heap monitoring code here
    
    echo "========================="
}

# Custom command to show CPU state
proc cpu_state {} {
    echo "=== CPU State ==="
    reg
    echo "================"
}

# Custom breakpoint with actions
proc break_with_trace {address} {
    bp $address 4 hw
    $_TARGETNAME configure -event halted {
        echo "Hit breakpoint at PC: [format 0x%08x [reg pc]]"
        show_memory
    }
}
```

---

## 7. Running and Debugging

### 7.1 Build Process

#### Step 1: Configure Project
```bash
# Command line method
cmake --preset Debug

# VS Code method
Ctrl+Shift+P → "CMake: Configure"
```

#### Step 2: Build Project  
```bash
# Command line method
cmake --build build/Debug --parallel

# VS Code method
Ctrl+Shift+P → "Tasks: Run Task" → "Build"
# Or use Ctrl+Shift+B (default build task)
```

#### Step 3: Verify Build Output
Check for successful build:
```
build/Debug/
├── YourProject.elf     # Main executable
├── YourProject.bin     # Binary for flashing
├── YourProject.hex     # Intel HEX format
└── YourProject.map     # Memory map file
```

### 7.2 Programming the Target

#### Method 1: Using VS Code Task
```
Ctrl+Shift+P → "Tasks: Run Task" → "Flash"
```

#### Method 2: Command Line
```bash
openocd -f openocd_custom.cfg -c "program build/Debug/YourProject.elf verify reset exit"
```

#### Method 3: Manual OpenOCD Session
```bash
# Terminal 1: Start OpenOCD
openocd -f openocd_custom.cfg

# Terminal 2: Connect with telnet
telnet localhost 4444
> halt
> flash write_image erase build/Debug/YourProject.elf
> verify_image build/Debug/YourProject.elf
> reset run
> exit
```

### 7.3 Debugging Process

#### Step 1: Start Debug Session
```
Method 1: Press F5 in VS Code
Method 2: Run → Start Debugging
Method 3: Ctrl+Shift+P → "Debug: Start Debugging"
```

#### Step 2: Debug Session Flow
1. **Automatic Build**: Project builds if needed
2. **OpenOCD Startup**: Connects to target via ST-Link
3. **GDB Connection**: Connects to OpenOCD on port 3333
4. **Program Loading**: Flashes ELF file to target
5. **Initial Break**: Stops at main() function

#### Step 3: Debug Operations

**Setting Breakpoints**:
- Click left margin in code editor
- `F9` to toggle breakpoint on current line
- Conditional breakpoints: Right-click → "Add Conditional Breakpoint"

**Stepping Through Code**:
- `F10` - Step Over (execute current line)
- `F11` - Step Into (enter function calls)
- `Shift+F11` - Step Out (exit current function)
- `F5` - Continue execution

**Inspecting Variables**:
- Hover over variables to see values
- Variables panel shows local/global variables
- Watch panel for custom expressions
- Call Stack panel shows function hierarchy

**Memory and Registers**:
- Memory view for raw memory inspection
- Peripheral registers via SVD file
- Core registers (R0-R15, PSR, etc.)

### 7.4 Advanced Debugging Features

#### A. Live Expressions
Add expressions to Watch panel:
```c
// Examples:
*((uint32_t*)0x40020014)    // GPIOA IDR register
HAL_GetTick()               // Current system tick
&htim2                      // Timer handle address
```

#### B. SWO Trace Output
Enable printf debugging via SWO:
```c
// In main.c, redirect printf to SWO
int _write(int file, char *ptr, int len) {
    for(int i = 0; i < len; i++) {
        ITM_SendChar((*ptr++));
    }
    return len;
}

// Usage in code
printf("Debug: Counter = %d\n", counter);
```

#### C. Real-Time Variable Tracking
Use peripheral register view to monitor:
- GPIO states in real-time
- Timer values and configurations
- UART status and data registers
- Clock configuration registers

#### D. Post-Mortem Debugging
After hard fault or crash:
```c
// Add to HardFault_Handler in stm32f4xx_it.c
void HardFault_Handler(void) {
    __asm volatile("bkpt #01");  // Trigger debugger
    while(1);
}
```

### 7.5 Debugging Best Practices

#### Code Organization:
```c
// Use debug builds with optimization disabled
#ifdef DEBUG
    #define DBG_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
    #define DBG_PRINTF(f_, ...)
#endif

// Assertion macros for debugging
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            DBG_PRINTF("ASSERT failed: %s:%d\n", __FILE__, __LINE__); \
            __asm volatile("bkpt #01"); \
        } \
    } while(0)
```

#### Performance Monitoring:
```c
// Timing measurements
uint32_t start_time = HAL_GetTick();
// ... code to measure ...
uint32_t execution_time = HAL_GetTick() - start_time;
DBG_PRINTF("Execution time: %lu ms\n", execution_time);
```

#### Memory Usage Tracking:
```c
// Stack usage monitoring
extern uint32_t _estack;
extern uint32_t _Min_Stack_Size;

uint32_t get_stack_usage(void) {
    uint32_t sp;
    __asm volatile("mov %0, sp" : "=r"(sp));
    return (uint32_t)&_estack - sp;
}
```

---

## 8. Troubleshooting

### 8.1 Common Build Issues

#### Issue: CMake Configuration Fails
```
Symptoms: "CMake Tools: Error during configure"
Solutions:
1. Check ARM GCC toolchain path in cmake/gcc-arm-none-eabi.cmake
2. Verify STM32CubeCLT installation
3. Check CMakePresets.json syntax
4. Clear CMake cache: Ctrl+Shift+P → "CMake: Delete Cache and Reconfigure"
```

#### Issue: Linker Errors
```
Symptoms: "undefined reference to...", "region FLASH overflowed"
Solutions:
1. Check linker script (STM32F429ZITX_FLASH.ld)
2. Verify memory regions match your MCU
3. Check for missing source files in CMakeLists.txt
4. Reduce code size or increase flash region
```

#### Issue: Include Path Errors
```
Symptoms: "fatal error: 'stm32f4xx_hal.h' file not found"
Solutions:
1. Regenerate project with STM32CubeMX
2. Check include paths in c_cpp_properties.json
3. Verify HAL library files are present in Drivers/
```

### 8.2 Debug Connection Issues

#### Issue: OpenOCD Connection Failed
```
Symptoms: "Error: unable to open ftdi device with vid 0483, pid 374b"
Solutions:
1. Check ST-Link driver installation
2. Verify USB cable and connections
3. Try different USB port
4. Check Windows Device Manager for ST-Link
5. Update ST-Link firmware using STM32CubeProgrammer
```

#### Issue: Target Not Detected
```
Symptoms: "Error: target not halted", "Error: timed out while waiting for target halted"
Solutions:
1. Check target power supply (3.3V)
2. Verify SWD connections (SWDIO, SWCLK, GND)
3. Check for proper NRST connection
4. Try lower adapter speed in OpenOCD config
5. Check for BOOT pin configuration
```

#### Issue: GDB Connection Timeout
```
Symptoms: "Remote communication error. Target disconnected."
Solutions:
1. Kill existing OpenOCD/GDB processes
2. Check port 3333 availability: netstat -an | findstr :3333
3. Restart VS Code and try again
4. Use manual OpenOCD startup for diagnosis
```

### 8.3 Programming Issues

#### Issue: Flash Programming Failed
```
Symptoms: "Flash programming failed", "Verification failed"
Solutions:
1. Check flash protection settings
2. Verify sufficient flash space for program
3. Try full chip erase first
4. Check power supply stability during programming
5. Use slower programming speed
```

#### Issue: Code Not Running After Programming
```
Symptoms: Target programmed but doesn't execute
Solutions:
1. Check boot configuration (BOOT0/BOOT1 pins)
2. Verify reset vector and startup code
3. Check clock configuration
4. Verify linker script memory layout
5. Use debugger to check PC after reset
```

### 8.4 Performance Issues

#### Issue: Slow Build Times
```
Solutions:
1. Use parallel builds: cmake --build --parallel 4
2. Enable compiler cache (ccache)
3. Use SSD for project storage
4. Exclude unnecessary files from build
5. Use incremental builds instead of clean builds
```

#### Issue: Slow Debugging
```
Solutions:
1. Reduce SWD speed if connection unstable
2. Disable SWO tracing if not needed
3. Limit number of watched variables
4. Use conditional breakpoints sparingly
5. Close unused debug views in VS Code
```

### 8.5 Environment Issues

#### Issue: Path Problems
```
Symptoms: "command not found" errors
Solutions:
1. Check System PATH environment variable
2. Restart VS Code after PATH changes
3. Use full paths in configuration files
4. Verify STM32CubeCLT installation
```

#### Issue: Extension Conflicts
```
Symptoms: Unexpected VS Code behavior
Solutions:
1. Disable other ARM/embedded extensions temporarily
2. Check for conflicting debugger configurations
3. Reset VS Code settings to default
4. Update all extensions to latest versions
```

### 8.6 Debugging Checklist

Before reporting issues, verify:

**Hardware**:
- [ ] Target board powered (LED indicators)
- [ ] ST-Link connected and recognized by Windows
- [ ] USB cable functional (try different cable)
- [ ] Target voltage ~3.3V

**Software**:
- [ ] STM32CubeCLT properly installed
- [ ] VS Code extensions installed and enabled
- [ ] Project builds without errors
- [ ] OpenOCD config file exists and is valid

**Configuration**:
- [ ] Correct MCU specified in all config files
- [ ] Toolchain paths correct in cmake files
- [ ] Debug configuration matches project name
- [ ] SVD file present for peripheral debugging

**Process**:
- [ ] Clean build completed successfully
- [ ] No other debugging sessions active
- [ ] Target not held in reset externally
- [ ] Sufficient permissions (run as admin if needed)

---

## 8. Build and Run vs Debug Modes

### 8.1 Overview
Your STM32F429ZI project supports two execution modes optimized for different use cases:

### 8.2 Build and Run Mode (No Debug) - Recommended for Testing

**Purpose**: Flash firmware and let it run freely
**Use Case**: Testing UART output, LED blinking, normal operation

**How to Use**: 
- Press `Ctrl+Shift+P`
- Type "Tasks: Run Task"
- Select **"Build and Run"**

**What it does**:
1. Builds the project (`cmake --build`)
2. Flashes firmware to STM32 (`openocd`)
3. Resets and runs the target
4. Disconnects (no debugging session)

### 8.3 Debug Mode - For Development

**Purpose**: Step through code, set breakpoints, inspect variables
**Use Case**: Finding bugs, understanding code flow

**How to Use**: 
- Press `F5` or go to Run & Debug
- Select **"Debug STM32F429ZI with OpenOCD"**

**What it does**:
1. Builds the project
2. Flashes firmware
3. Starts GDB debugging session
4. Stops at `main()` function
5. Allows breakpoints and stepping

### 8.4 Key Differences

| Feature | Build and Run | Debug Mode |
|---------|---------------|------------|
| **Speed** | ⚡ Fast | 🐌 Slower |
| **Purpose** | Testing/Demo | Development |
| **Breakpoints** | ❌ No | ✅ Yes |
| **UART Output** | ✅ Continuous | ⚠️ May pause |
| **Target State** | 🏃 Running Free | 🛑 Controlled |
| **Use Case** | Serial Monitor, LEDs | Code Analysis |

### 8.5 When to Use Each

**Use Build and Run when**:
- ✅ Testing UART/Serial output
- ✅ Checking LED patterns
- ✅ Demonstrating functionality
- ✅ Performance testing
- ✅ Long-running tests

**Use Debug Mode when**:
- 🔍 Finding bugs
- 🔍 Understanding code flow
- 🔍 Checking variable values
- 🔍 Setting breakpoints
- 🔍 Step-by-step execution

### 8.6 Task Configuration

The **"Build and Run"** task automatically executes:
1. **Configure CMake** (if needed)
2. **Build** → `cmake --build build/Debug`
3. **Flash** → `openocd -f interface/stlink.cfg ...`

---

## 9. Serial Terminal Setup

### 9.1 Overview
This section shows how to view UART debug output from your STM32F429ZI using VS Code's built-in Serial Monitor extension.

### 9.2 Hardware Configuration
- **UART1** is configured for debug output
- **Baud Rate**: 115200
- **Pins**: PA9 (TX), PA10 (RX)
- **ST-Link Virtual COM Port**: COM6

### 9.3 Software Setup

#### Serial Monitor Extension
The Microsoft Serial Monitor extension should be installed:
- **Extension**: `ms-vscode.vscode-serial-monitor`
- **Publisher**: Microsoft

#### Code Configuration
The printf redirection is already implemented in the project:

**In `Core/Src/usart.c`:**
```c
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (const uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
```

**In `Core/Inc/usart.h`:**
```c
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
```

**In `Core/Src/main.c`:**
```c
printf("[%lu] Hello from UART! Toggle: %d\r\n", loop_counter, toggle);
```

### 9.4 How to Use Serial Terminal

#### Method 1: VS Code Command Palette
1. Press `Ctrl+Shift+P` to open Command Palette
2. Type "Serial Monitor" and select appropriate command
3. Configure connection:
   - **Port**: COM6
   - **Baud Rate**: 115200
   - **Data Bits**: 8
   - **Stop Bits**: 1
   - **Parity**: None

#### Method 2: VS Code Status Bar
1. Look for Serial Monitor icon in VS Code status bar
2. Click to open serial monitor
3. Select COM6 port at 115200 baud

#### Method 3: Manual Terminal Setup
If the extension doesn't work, use PowerShell:
```powershell
# Install PuTTY (if not already installed)
winget install PuTTY.PuTTY

# Open PuTTY with serial connection
putty -serial COM6 -sercfg 115200,8,n,1,N
```

### 9.5 Expected Output
You should see output like:
```
[1] Hello from UART! Toggle: 0
[2] Hello from UART! Toggle: 1
[3] Hello from UART! Toggle: 2
...
[50] LED toggled, reset counter
[51] Hello from UART! Toggle: 0
```

### 9.6 Advanced Usage

#### Custom Printf Functions
```c
// In main.c or usart.c
void UART_Printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

// Usage
UART_Printf("Sensor value: %d, Temperature: %.2f\r\n", sensor_val, temp);
```

#### Logging Levels
```c
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel_t;

void LOG_Print(LogLevel_t level, const char* format, ...) {
    const char* prefixes[] = {"[DEBUG]", "[INFO]", "[WARN]", "[ERROR]"};
    printf("%s ", prefixes[level]);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\r\n");
}

// Usage
LOG_Print(LOG_INFO, "System initialized");
LOG_Print(LOG_ERROR, "Sensor read failed: %d", error_code);
```

### 9.7 Serial Monitor Settings
Configure persistent settings in `.vscode/settings.json`:
```json
{
    "serial-monitor.defaultPort": "COM6",
    "serial-monitor.defaultBaudRate": "115200",
    "serial-monitor.defaultDataBits": "8",
    "serial-monitor.defaultStopBits": "1",
    "serial-monitor.defaultParity": "none",
    "serial-monitor.autoConnect": false,
    "serial-monitor.timestampFormat": "HH:mm:ss.fff"
}
```

---

## 10. STM32F429ZI Specific Configuration

### 10.1 STM32F429ZI Specifications
- **Core**: ARM Cortex-M4 with FPU
- **Max CPU Frequency**: 180 MHz
- **Flash Memory**: 2 MB
- **RAM**: 256 KB (192 KB + 64 KB CCM)
- **Package**: LQFP144 or BGA176
- **Debug Interface**: SWD/JTAG

### 10.2 Memory Configuration

#### Flash Memory Layout:
```
0x08000000 - 0x081FFFFF: Main Flash (2 MB)
0x1FFF0000 - 0x1FFF77FF: System Memory (30 KB)
0x1FFFC000 - 0x1FFFC00F: Option Bytes (16 B)
```

#### RAM Layout:
```
0x20000000 - 0x2002FFFF: SRAM1 (192 KB)
0x10000000 - 0x1000FFFF: CCM RAM (64 KB)
```

### 10.3 Debug Configuration for F429ZI

```json
{
    "name": "Debug STM32F429ZI with OpenOCD",
    "device": "STM32F429ZI",
    "cpuFrequency": 180000000,  // 180 MHz max frequency
    "swoFrequency": 2000000,    // 2 MHz SWO trace
    "svdFile": "STM32F429.svd"  // F429-specific peripheral definitions
}
```

### 10.4 Debugging Features Available

#### Hardware Debugging:
- ✅ **6 Hardware Breakpoints**: Set breakpoints without code modification
- ✅ **4 Watchpoints**: Monitor variable/memory access
- ✅ **Single Step Debugging**: Step through code line by line
- ✅ **Call Stack**: View function call hierarchy

#### SWO Tracing (Serial Wire Output):
- ✅ **Printf Debugging**: Output debug messages via SWO
- ✅ **ITM Trace**: Instrumentation Trace Macrocell data
- ✅ **Real-time Trace**: Monitor execution without stopping

#### Peripheral Register Viewing:
- ✅ **All STM32F429 Peripherals**: GPIO, Timers, UART, SPI, I2C, etc.
- ✅ **Real-time Updates**: Register values update during debugging
- ✅ **Bit-level Details**: Individual bit explanations

### 10.5 Performance Optimization

For STM32F429ZI Development:
- Use **180 MHz** CPU frequency for maximum performance
- Leverage **CCM RAM** (0x10000000) for critical data/stacks
- Enable **I-Cache** and **D-Cache** for faster execution
- Use **DMA** controllers for high-speed data transfers
- Consider **DSP instructions** available in Cortex-M4

---

## 11. Troubleshooting and Common Fixes

### 11.1 "Monitor" Command Error Fix

#### Problem Description
The error "invalid command name 'monitor'" occurs when:
1. GDB tries to execute `monitor` commands before connecting to OpenOCD
2. OpenOCD commands are incorrectly prefixed with `monitor` in `openOCDLaunchCommands`
3. Multiple GDB server instances are conflicting

#### Root Cause
- `monitor` commands are GDB commands that send commands to OpenOCD
- `openOCDLaunchCommands` should contain raw OpenOCD commands (without `monitor` prefix)
- `postLaunchCommands` can use `monitor` prefix (executed by GDB after connection)

#### Fix Applied ✅
**Before (Incorrect):**
```json
"openOCDLaunchCommands": [
    "monitor reset halt"  // ❌ Wrong: 'monitor' prefix not needed
]
```

**After (Correct):**
```json
"openOCDLaunchCommands": [
    "reset halt"  // ✅ Correct: Raw OpenOCD command
]
```

### 11.2 "Reset" Command Error Fix

#### Problem Analysis
The "invalid command name 'reset'" error occurs because:
1. OpenOCD commands are being executed in the wrong context
2. The cortex-debug extension may be misinterpreting command syntax
3. Complex launch configurations can cause command parsing issues

#### Solution Applied ✅
Simplified configuration by removing problematic elements:
- ❌ `openOCDLaunchCommands` - Can cause command context issues
- ❌ `swoConfig` - Can interfere with basic debugging setup
- ❌ `postLaunchCommands` - May execute before proper connection
- ❌ `overrideLaunchCommands` - Complex launch sequences

**Kept essential elements:**
- ✅ Basic OpenOCD server configuration
- ✅ STM32F429ZI device specification
- ✅ SVD file for peripheral viewing
- ✅ Standard SWD interface

### 11.3 Device Stuck in HOLD State Fix

#### Problem Description
When disconnecting the debugger from STM32F429ZI, the device remains in HALT state instead of resuming normal operation.

#### Root Cause
This happens because:
1. VS Code/GDB stops debugging but doesn't reset the target
2. OpenOCD keeps target halted when GDB disconnects
3. No automatic resume commands are configured

#### Solution Implemented ✅
Created `openocd_stm32f429zi.cfg` with automatic resume on disconnect:

```tcl
# Custom OpenOCD configuration for STM32F429ZI
source [find interface/stlink.cfg]
source [find target/stm32f4x.cfg]

# Hook to run when GDB disconnects
$_TARGETNAME configure -event gdb-detach {
    echo "GDB disconnected - resetting and resuming target"
    catch {
        reset
        resume
    }
}
```

#### Manual Solutions
**Immediate Fix (Target Currently Stuck)**:
```bash
C:\openocd\xpack-openocd-0.12.0-3\bin\openocd.exe -f interface/stlink.cfg -f target/stm32f4x.cfg -c "init; reset halt; reset; resume; shutdown"
```

### 11.4 Terminal Issues Fixed

#### Issues Found and Fixed ✅
1. **Device Configuration Mismatch**: Ensured consistent STM32F429ZI targeting
2. **Invalid Debug Properties**: Removed unsupported `postAttachCommands`  
3. **Terminal/GDB Server Issues**: Streamlined OpenOCD configuration

#### Current Status ✅
- ✅ **OpenOCD**: Successfully detects STM32F4x Cortex-M4 processor
- ✅ **ST-Link Connection**: Target voltage detected (2.89V)
- ✅ **Build System**: CMake builds successfully (Exit Code: 0)
- ✅ **Debug Interface**: SWD configured at 2MHz
- ✅ **GDB Server**: Listening on port 3333

### 11.5 Serial Terminal Troubleshooting

#### COM Port Issues
```powershell
# List all COM ports
Get-WmiObject -Class Win32_SerialPort | Select-Object Name, DeviceID

# Check if ST-Link is detected
Get-WmiObject -Class Win32_PnPEntity | Where-Object {$_.Name -like "*STLink*"}
```

#### Connection Problems
1. **No COM6**: Reconnect ST-Link USB cable
2. **No Output**: Check that firmware is running (LED should blink)
3. **Garbled Text**: Verify baud rate is 115200
4. **Permission Error**: Close other serial terminal programs

#### Code Issues
If printf doesn't work, check:
1. UART1 initialization: `MX_USART1_UART_Init()` is called
2. `__io_putchar()` implementation in `usart.c`
3. `PUTCHAR_PROTOTYPE` definition in `usart.h`

### 11.6 Logging Issues Fix

#### Problem: Logging Stops After 15 Iterations
This was caused by:
1. **Printf in Interrupt Context**: Timer callback had `printf` statement causing blocking
2. **Timer Configuration Issues**: Extremely long timer period (not 500ms as intended)  
3. **Missing Error Handling**: No debugging information to track issues

#### Fixes Applied ✅
1. **Cleaned Up Timer Callbacks**:
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1)
  {
    PeriodicScheduler = 1;  // Simple flag setting only - no printf in interrupt
  }
}
```

2. **Fixed Timer Configuration**:
```c
htim1.Init.Prescaler = 17999;  // 180MHz / (17999+1) = 10kHz
htim1.Init.Period = 4999;      // 10kHz / (4999+1) = 2Hz (500ms period)
```

3. **Enhanced Main Loop Debugging**:
```c
printf("[%lu] Hello from UART! Toggle: %d\r\n", loop_counter, toggle);
```

---

## 12. Quick Reference

### 12.1 VS Code Shortcuts
- `Ctrl+Shift+P` - Command palette
- `Ctrl+Shift+B` - Build task
- `F5` - Start debugging (with breakpoints)
- `Shift+F5` - Stop debugging
- `F9` - Toggle breakpoint
- `F10` - Step over
- `F11` - Step into

### 12.2 Quick Tasks (`Ctrl+Shift+P` → "Tasks: Run Task")
- **Build and Run** - Flash and run (no debugging) 🏃
- **Build** - Compile project only
- **Flash** - Program STM32 only  
- **Clean** - Clean build artifacts
- **Reset Target** - Reset if stuck in HALT state

### 12.3 Build Commands
```bash
# Configure CMake
cmake --preset Debug

# Build project  
cmake --build build/Debug --parallel

# Clean build
cmake --build build/Debug --target clean
```

### 12.4 Common File Locations
```
Project/
├── .vscode/
│   ├── tasks.json           # Build tasks
│   ├── launch.json          # Debug config
│   └── settings.json        # Project settings
├── openocd_stm32f429zi.cfg  # Custom OpenOCD config
├── CMakeLists.txt           # Build system
├── CMakePresets.json        # Build presets
└── build/Debug/             # Build output
    └── BaseLine_Project.elf # Executable
```

### 12.5 Essential Commands
```bash
# Manual reset target
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "init; reset; resume; shutdown"

# Flash manually  
openocd -f openocd_stm32f429zi.cfg -c "program build/Debug/BaseLine_Project.elf verify reset exit"

# Check target status
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "init; targets; shutdown"

# Check COM ports
Get-WmiObject -Class Win32_SerialPort | Select-Object Name, DeviceID
```

### 12.6 Troubleshooting Quick Fixes
1. **Build fails**: Check toolchain paths, run "Clean" task first
2. **Debug fails**: Kill OpenOCD processes, check hardware connection
3. **Target stuck**: Run "Reset Target" task or manual reset command
4. **No UART output**: Check Serial Monitor connection (COM6, 115200 baud)
5. **No IntelliSense**: Check c_cpp_properties.json configuration

### 12.7 Project Status Verification

**Hardware Checklist**:
- [ ] Target board powered (LED indicators)
- [ ] ST-Link connected and recognized by Windows
- [ ] USB cable functional
- [ ] Target voltage ~3.3V

**Software Checklist**:
- [ ] STM32CubeCLT properly installed
- [ ] VS Code extensions installed and enabled
- [ ] Project builds without errors (`Exit Code: 0`)
- [ ] OpenOCD config file exists and valid

**Configuration Checklist**:
- [ ] Correct MCU (STM32F429ZI) specified in all config files
- [ ] Toolchain paths correct in cmake files
- [ ] Debug configuration matches project name
- [ ] SVD file present for peripheral debugging
- [ ] Serial Monitor extension installed
- [ ] COM6 port available at 115200 baud

---

## Conclusion

This comprehensive guide provides everything needed to set up VS Code for professional STM32 development. The combination of STM32CubeMX, CMake, and VS Code creates a powerful, flexible development environment that rivals commercial IDEs while remaining free and open-source.

Key benefits of this setup:
- **Modern tooling** with excellent IntelliSense and debugging
- **Cross-platform** development capability
- **Extensible** with thousands of available extensions
- **Version control** friendly project structure
- **Professional debugging** with peripheral register viewing

For continued learning, explore:
- STM32 HAL documentation
- ARM Cortex-M programming guides  
- Advanced debugging techniques
- Real-time operating systems (FreeRTOS)
- Communication protocols (CAN, Ethernet, USB)

Happy STM32 programming with VS Code!