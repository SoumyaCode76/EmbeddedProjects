#!/usr/bin/env python3
"""
RAM Monitor - Identify programs consuming system RAM in decreasing order
Author: GitHub Copilot
Date: November 22, 2025

This script displays running processes sorted by their RAM usage in descending order.
It shows process ID, name, and memory consumption in both MB and percentage of total RAM.
"""

import psutil
import sys
from typing import List, Tuple, NamedTuple, Optional
from datetime import datetime

class ProcessInfo(NamedTuple):
    """Structure to hold process information"""
    pid: int
    name: str
    memory_mb: float
    memory_percent: float
    status: str

def get_system_memory_info() -> Tuple[float, float]:
    """
    Get total and available system memory in GB
    
    Returns:
        Tuple of (total_memory_gb, available_memory_gb)
    """
    memory = psutil.virtual_memory()
    total_gb = memory.total / (1024**3)
    available_gb = memory.available / (1024**3)
    return total_gb, available_gb

def get_processes_by_memory() -> List[ProcessInfo]:
    """
    Get all running processes sorted by memory usage in descending order
    
    Returns:
        List of ProcessInfo objects sorted by memory usage (highest first)
    """
    processes = []
    
    for proc in psutil.process_iter(['pid', 'name', 'memory_info', 'status']):
        try:
            # Get process info
            pid = proc.info['pid']
            name = proc.info['name']
            status = proc.info['status']
            
            # Get memory info
            memory_info = proc.info['memory_info']
            memory_bytes = memory_info.rss  # Resident Set Size (actual physical memory)
            memory_mb = memory_bytes / (1024**2)
            
            # Calculate memory percentage
            memory_percent = proc.memory_percent()
            
            processes.append(ProcessInfo(
                pid=pid,
                name=name,
                memory_mb=memory_mb,
                memory_percent=memory_percent,
                status=status
            ))
            
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            # Skip processes that are no longer running or inaccessible
            continue
    
    # Sort by memory usage in descending order
    processes.sort(key=lambda x: x.memory_mb, reverse=True)
    return processes

def format_memory_size(bytes_value: float) -> str:
    """
    Format memory size in human-readable format
    
    Args:
        bytes_value: Memory size in bytes
        
    Returns:
        Formatted string (e.g., "1.2 GB", "500.5 MB")
    """
    if bytes_value >= 1024**3:
        return f"{bytes_value / (1024**3):.1f} GB"
    elif bytes_value >= 1024**2:
        return f"{bytes_value / (1024**2):.1f} MB"
    elif bytes_value >= 1024:
        return f"{bytes_value / 1024:.1f} KB"
    else:
        return f"{bytes_value:.0f} B"

def display_memory_report(processes: List[ProcessInfo], top_n: Optional[int] = None) -> None:
    """
    Display formatted memory usage report
    
    Args:
        processes: List of ProcessInfo objects sorted by memory usage
        top_n: Number of top processes to display (None for all processes)
    """
    total_memory_gb, available_memory_gb = get_system_memory_info()
    used_memory_gb = total_memory_gb - available_memory_gb
    
    # Use all processes if top_n is None
    if top_n is None:
        top_n = len(processes)
    
    print("=" * 80)
    print(f"RAM USAGE REPORT - {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 80)
    print(f"Total RAM: {total_memory_gb:.2f} GB")
    print(f"Used RAM:  {used_memory_gb:.2f} GB ({(used_memory_gb/total_memory_gb)*100:.1f}%)")
    print(f"Free RAM:  {available_memory_gb:.2f} GB ({(available_memory_gb/total_memory_gb)*100:.1f}%)")
    print("=" * 80)
    if top_n == len(processes):
        print(f"ALL {len(processes)} PROCESSES BY RAM USAGE")
    else:
        print(f"TOP {top_n} PROCESSES BY RAM USAGE")
    print("=" * 80)
    
    # Header
    print(f"{'Rank':<4} {'PID':<8} {'Memory':<12} {'%RAM':<8} {'Status':<10} {'Process Name'}")
    print("-" * 80)
    
    # Display top processes
    for i, proc in enumerate(processes[:top_n], 1):
        memory_str = format_memory_size(proc.memory_mb * 1024**2)
        print(f"{i:<4} {proc.pid:<8} {memory_str:<12} {proc.memory_percent:<7.2f}% "
              f"{proc.status:<10} {proc.name}")
    
    print("=" * 80)
    
    # Summary statistics
    total_displayed_memory = sum(proc.memory_mb for proc in processes[:top_n])
    total_all_processes_memory = sum(proc.memory_mb for proc in processes)
    
    print(f"Memory used by top {top_n} processes: {format_memory_size(total_displayed_memory * 1024**2)}")
    print(f"Total memory used by all processes: {format_memory_size(total_all_processes_memory * 1024**2)}")
    print(f"Number of running processes: {len(processes)}")

def main():
    """Main function"""
    try:
        # Get command line arguments
        top_n = None  # Default to show all processes
        if len(sys.argv) > 1:
            try:
                top_n = int(sys.argv[1])
                if top_n <= 0:
                    raise ValueError("Number must be positive")
            except ValueError:
                print(f"Error: Invalid number '{sys.argv[1]}'. Showing all processes.")
                top_n = None
        
        # Check if psutil is available
        print("Scanning running processes...")
        
        # Get processes sorted by memory usage
        processes = get_processes_by_memory()
        
        if not processes:
            print("No processes found or unable to access process information.")
            return
        
        # Display the report
        display_memory_report(processes, top_n)
        
        # Option to save to file
        save_option = input("\nWould you like to save this report to a file? (y/N): ").strip().lower()
        if save_option in ['y', 'yes']:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"ram_report_{timestamp}.txt"
            
            # Redirect output to file
            import io
            from contextlib import redirect_stdout
            
            with open(filename, 'w', encoding='utf-8') as f:
                with redirect_stdout(f):
                    display_memory_report(processes, len(processes))  # Save all processes
            
            print(f"Report saved to: {filename}")
    
    except KeyboardInterrupt:
        print("\nOperation cancelled by user.")
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)

if __name__ == "__main__":
    # Check if psutil is installed
    try:
        import psutil
    except ImportError:
        print("Error: psutil library is not installed.")
        print("Please install it using: pip install psutil")
        sys.exit(1)
    
    main()