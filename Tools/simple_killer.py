#!/usr/bin/env python3
"""
Simple Process Killer - Windows-specific version using taskkill

Usage:
    python simple_killer.py notepad.exe
    python simple_killer.py chrome --force

No external dependencies required (Windows only)
"""

import subprocess
import sys
import argparse
import os

def list_processes_windows(process_name: str):
    """List processes using Windows tasklist command"""
    try:
        if not process_name.lower().endswith('.exe'):
            process_name += '.exe'
            
        # Use tasklist to find processes
        result = subprocess.run([
            'tasklist', '/FI', f'IMAGENAME eq {process_name}'
        ], capture_output=True, text=True, shell=True)
        
        if result.returncode == 0:
            lines = result.stdout.strip().split('\n')
            # Skip header lines and filter actual process entries
            process_lines = [line for line in lines if process_name.lower() in line.lower()]
            
            if process_lines:
                print(f"🔍 Found {len(process_lines)} instance(s) of '{process_name}':")
                for line in process_lines:
                    parts = line.split()
                    if len(parts) >= 2:
                        print(f"   - {parts[0]} (PID: {parts[1]})")
                return len(process_lines)
            else:
                print(f"🔍 No running instances of '{process_name}' found")
                return 0
        else:
            print(f"❌ Error listing processes: {result.stderr}")
            return 0
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return 0

def kill_processes_windows(process_name: str, force_kill: bool = False) -> bool:
    """Kill processes using Windows taskkill command"""
    try:
        if not process_name.lower().endswith('.exe'):
            process_name += '.exe'
        
        # Build taskkill command
        cmd = ['taskkill', '/IM', process_name]
        
        if force_kill:
            cmd.append('/F')  # Force kill
            
        print(f"🎯 Executing: {' '.join(cmd)}")
        
        # Execute taskkill
        result = subprocess.run(cmd, capture_output=True, text=True, shell=True)
        
        if result.returncode == 0:
            print("✅ Command executed successfully")
            print(result.stdout)
            return True
        else:
            print(f"❌ Command failed with return code {result.returncode}")
            if result.stderr:
                print(f"Error: {result.stderr}")
            if result.stdout:
                print(f"Output: {result.stdout}")
            return False
            
    except Exception as e:
        print(f"❌ Error executing taskkill: {e}")
        return False

def main():
    # Check if running on Windows
    if os.name != 'nt':
        print("❌ This script is designed for Windows only")
        print("🐧 For Unix/Linux systems, use the psutil version")
        sys.exit(1)
    
    parser = argparse.ArgumentParser(
        description='Kill all instances of a specified executable (Windows)',
        epilog="""
Examples:
  %(prog)s notepad.exe              # Kill all notepad instances
  %(prog)s chrome --force           # Force kill all Chrome instances  
  %(prog)s "code.exe" -f -y         # Force kill VS Code, skip confirmation
        """,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument('process_name', 
                       help='Name of the executable to kill (e.g., notepad.exe, chrome)')
    
    parser.add_argument('--force', '-f', 
                       action='store_true',
                       help='Use force kill (/F flag)')
    
    parser.add_argument('--yes', '-y', 
                       action='store_true',
                       help='Skip confirmation prompt')
    
    parser.add_argument('--list', '-l',
                       action='store_true', 
                       help='Only list matching processes, don\'t kill them')
    
    args = parser.parse_args()
    
    try:
        if args.list:
            # List mode
            list_processes_windows(args.process_name)
            return
        
        # First, check if processes exist
        count = list_processes_windows(args.process_name)
        
        if count == 0:
            return
        
        # Confirmation prompt
        if not args.yes:
            kill_method = "FORCE KILL" if args.force else "terminate"
            response = input(f"\n❓ Are you sure you want to {kill_method} all instances? (y/N): ")
            if response.lower() not in ['y', 'yes']:
                print("🚫 Operation cancelled by user")
                return
        
        # Kill processes
        success = kill_processes_windows(args.process_name, args.force)
        
        if success:
            print("✅ Operation completed successfully")
        else:
            print("❌ Operation failed")
            sys.exit(1)
            
    except KeyboardInterrupt:
        print("\n🚫 Operation cancelled by user (Ctrl+C)")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()