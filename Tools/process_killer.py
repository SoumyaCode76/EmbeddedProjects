#!/usr/bin/env python3
"""
Process Killer - Kill all instances of a specified executable

Usage:
    python process_killer.py notepad.exe
    python process_killer.py chrome --force
    python process_killer.py "Visual Studio Code.exe" -f

Requirements:
    pip install psutil
"""

import psutil
import sys
import argparse
import os
from typing import List, Tuple

def find_processes_by_name(process_name: str) -> List[Tuple[int, str]]:
    """
    Find all processes matching the given name
    
    Args:
        process_name (str): Name of the executable to find
        
    Returns:
        List[Tuple[int, str]]: List of (PID, process_name) tuples
    """
    processes = []
    
    # Normalize process name (add .exe if not present on Windows)
    if os.name == 'nt' and not process_name.lower().endswith('.exe'):
        process_name += '.exe'
    
    print(f"🔍 Searching for processes named: '{process_name}'")
    
    for proc in psutil.process_iter(['pid', 'name', 'exe']):
        try:
            proc_name = proc.info['name']
            
            # Case-insensitive comparison
            if proc_name and proc_name.lower() == process_name.lower():
                processes.append((proc.info['pid'], proc_name))
                
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            continue
    
    return processes

def kill_process_by_pid(pid: int, process_name: str, force_kill: bool = False) -> bool:
    """
    Kill a process by PID
    
    Args:
        pid (int): Process ID to kill
        process_name (str): Name of the process (for logging)
        force_kill (bool): If True, use SIGKILL immediately
        
    Returns:
        bool: True if successfully killed, False otherwise
    """
    try:
        proc = psutil.Process(pid)
        
        if force_kill:
            # Force kill immediately
            proc.kill()
            print(f"💀 Force killed: {process_name} (PID: {pid})")
        else:
            # Try graceful termination first
            proc.terminate()
            
            try:
                # Wait up to 5 seconds for graceful termination
                proc.wait(timeout=5)
                print(f"✅ Gracefully terminated: {process_name} (PID: {pid})")
            except psutil.TimeoutExpired:
                # Force kill if graceful termination fails
                proc.kill()
                print(f"💀 Force killed (after timeout): {process_name} (PID: {pid})")
        
        return True
        
    except psutil.NoSuchProcess:
        print(f"⚠️  Process already terminated: {process_name} (PID: {pid})")
        return True  # Consider this as success
        
    except psutil.AccessDenied:
        print(f"❌ Access denied: Cannot kill {process_name} (PID: {pid})")
        print("   Try running as administrator/sudo")
        return False
        
    except Exception as e:
        print(f"❌ Error killing {process_name} (PID: {pid}): {e}")
        return False

def kill_all_processes(process_name: str, force_kill: bool = False, skip_confirmation: bool = False) -> Tuple[int, int]:
    """
    Kill all instances of a process
    
    Args:
        process_name (str): Name of executable to kill
        force_kill (bool): Use force kill instead of graceful termination
        skip_confirmation (bool): Skip user confirmation
        
    Returns:
        Tuple[int, int]: (successful_kills, total_found)
    """
    # Find all matching processes
    processes = find_processes_by_name(process_name)
    
    if not processes:
        print(f"🔍 No running instances of '{process_name}' found")
        return 0, 0
    
    print(f"\n📋 Found {len(processes)} instance(s) of '{process_name}':")
    for pid, name in processes:
        print(f"   - {name} (PID: {pid})")
    
    # Confirmation prompt
    if not skip_confirmation:
        kill_method = "FORCE KILL" if force_kill else "terminate"
        response = input(f"\n❓ Are you sure you want to {kill_method} all {len(processes)} instance(s)? (y/N): ")
        if response.lower() not in ['y', 'yes']:
            print("🚫 Operation cancelled by user")
            return 0, len(processes)
    
    # Kill processes
    print(f"\n🎯 Killing {len(processes)} process(es)...")
    successful_kills = 0
    
    for pid, name in processes:
        if kill_process_by_pid(pid, name, force_kill):
            successful_kills += 1
    
    return successful_kills, len(processes)

def main():
    parser = argparse.ArgumentParser(
        description='Kill all instances of a specified executable',
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
                       help='Use force kill (SIGKILL) instead of graceful termination')
    
    parser.add_argument('--yes', '-y', 
                       action='store_true',
                       help='Skip confirmation prompt')
    
    parser.add_argument('--list', '-l',
                       action='store_true', 
                       help='Only list matching processes, don\'t kill them')
    
    args = parser.parse_args()
    
    try:
        if args.list:
            # List mode - just show matching processes
            processes = find_processes_by_name(args.process_name)
            if processes:
                print(f"\n📋 Found {len(processes)} instance(s) of '{args.process_name}':")
                for pid, name in processes:
                    print(f"   - {name} (PID: {pid})")
            else:
                print(f"🔍 No running instances of '{args.process_name}' found")
            return
        
        # Kill mode
        successful, total = kill_all_processes(
            args.process_name, 
            force_kill=args.force,
            skip_confirmation=args.yes
        )
        
        # Summary
        print(f"\n📊 Summary:")
        print(f"   Total found: {total}")
        print(f"   Successfully killed: {successful}")
        print(f"   Failed: {total - successful}")
        
        if successful == total and total > 0:
            print("✅ All processes killed successfully")
        elif successful > 0:
            print("⚠️  Some processes could not be killed")
            sys.exit(1)
        elif total > 0:
            print("❌ Failed to kill any processes")
            sys.exit(1)
            
    except KeyboardInterrupt:
        print("\n🚫 Operation cancelled by user (Ctrl+C)")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    # Check if psutil is available
    try:
        import psutil
    except ImportError:
        print("❌ Error: psutil module not found")
        print("📦 Install it with: pip install psutil")
        sys.exit(1)
    
    main()