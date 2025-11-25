# Process Killer Tools

Two Python scripts to kill all instances of a specified executable.

## Files

1. **`process_killer.py`** - Full-featured version using psutil (cross-platform)
2. **`simple_killer.py`** - Simple version using Windows built-in commands (Windows only)

## Installation

### For process_killer.py (Recommended)
```bash
pip install psutil
```

### For simple_killer.py
No additional dependencies required (Windows only)

## Usage

### Basic Usage
```bash
# Kill all instances of notepad
python process_killer.py notepad.exe

# Kill all Chrome instances
python process_killer.py chrome

# Kill Visual Studio Code
python process_killer.py "code.exe"
```

### Advanced Options
```bash
# Force kill (immediate termination)
python process_killer.py notepad.exe --force
python process_killer.py chrome -f

# Skip confirmation prompt
python process_killer.py notepad.exe --yes
python process_killer.py chrome -y

# Force kill without confirmation
python process_killer.py chrome -f -y

# List processes without killing them
python process_killer.py chrome --list
python process_killer.py notepad.exe -l
```

### Windows Simple Version
```bash
# Same syntax but uses Windows taskkill
python simple_killer.py notepad.exe
python simple_killer.py chrome --force
```

## Features Comparison

| Feature | process_killer.py | simple_killer.py |
|---------|-------------------|------------------|
| Cross-platform | ✅ Yes | ❌ Windows only |
| External dependencies | psutil | ✅ None |
| Graceful termination | ✅ Yes | ❌ No |
| Detailed process info | ✅ Yes | ⚠️ Basic |
| Error handling | ✅ Comprehensive | ⚠️ Basic |
| Force kill | ✅ Yes | ✅ Yes |
| List mode | ✅ Yes | ✅ Yes |

## Examples

### Kill all instances of a browser
```bash
# Chrome
python process_killer.py chrome.exe

# Firefox  
python process_killer.py firefox.exe

# Edge
python process_killer.py msedge.exe
```

### Kill development tools
```bash
# Visual Studio Code
python process_killer.py code.exe

# Visual Studio
python process_killer.py devenv.exe

# JetBrains IDE
python process_killer.py idea64.exe
```

### Kill system utilities
```bash
# Notepad
python process_killer.py notepad.exe

# Calculator
python process_killer.py calc.exe

# Task Manager
python process_killer.py taskmgr.exe --force
```

## Safety Features

1. **Confirmation prompt** - Asks before killing processes (unless `-y` flag is used)
2. **Process listing** - Shows what processes will be killed before execution
3. **Graceful termination** - Tries to terminate gracefully before force killing
4. **Error handling** - Handles access denied and other common errors
5. **List mode** - Preview mode to see matching processes without killing

## Error Messages

- `Access denied` - Need administrator privileges
- `Process already terminated` - Process ended while script was running  
- `No running instances found` - No matching processes exist
- `psutil module not found` - Need to install psutil dependency

## Tips

1. **Use quotes** for process names with spaces: `"Visual Studio Code.exe"`
2. **Run as administrator** if you get access denied errors
3. **Use --list first** to preview what processes will be affected
4. **Add .exe extension** for Windows processes (auto-added if missing)
5. **Use --force carefully** - it kills processes immediately without cleanup

## Security Note

⚠️ **Use with caution!** These scripts can terminate critical system processes. Always:
- Double-check the process name
- Use list mode first to preview
- Avoid killing system-critical processes
- Run with appropriate privileges only