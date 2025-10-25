#!/usr/bin/env python3
"""
RST to Markdown Combiner for LVGL Documentation
Combines all RST files into a single comprehensive Markdown document
"""

import os
import re
from pathlib import Path
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def convert_rst_to_markdown(content):
    """Convert RST content to Markdown format"""
    
    # Convert headers (RST uses underlines, MD uses #)
    lines = content.split('\n')
    result_lines = []
    i = 0
    
    while i < len(lines):
        line = lines[i]
        
        # Check if next line is a header underline
        if i + 1 < len(lines):
            next_line = lines[i + 1]
            
            # RST headers with = underline (main title)
            if next_line.strip() and all(c == '=' for c in next_line.strip()) and len(next_line.strip()) >= len(line.strip()):
                result_lines.append(f"# {line.strip()}")
                i += 2  # Skip the underline
                continue
            
            # RST headers with - underline (section)
            elif next_line.strip() and all(c == '-' for c in next_line.strip()) and len(next_line.strip()) >= len(line.strip()):
                result_lines.append(f"## {line.strip()}")
                i += 2  # Skip the underline
                continue
            
            # RST headers with ~ underline (subsection)
            elif next_line.strip() and all(c == '~' for c in next_line.strip()) and len(next_line.strip()) >= len(line.strip()):
                result_lines.append(f"### {line.strip()}")
                i += 2  # Skip the underline
                continue
            
            # RST headers with ^ underline (subsubsection)
            elif next_line.strip() and all(c == '^' for c in next_line.strip()) and len(next_line.strip()) >= len(line.strip()):
                result_lines.append(f"#### {line.strip()}")
                i += 2  # Skip the underline
                continue
        
        # Convert RST directives to Markdown code blocks
        line = re.sub(r'^\.\. code-block:: (\w+)', r'```\1', line)
        line = re.sub(r'^\.\. code::', r'```', line)
        
        # Convert RST emphasis and strong
        line = re.sub(r'\*\*(.+?)\*\*', r'**\1**', line)  # Strong (already correct)
        line = re.sub(r'\*(.+?)\*', r'*\1*', line)  # Emphasis (already correct)
        line = re.sub(r'``(.+?)``', r'`\1`', line)  # Code (convert double backticks to single)
        
        # Convert RST links to Markdown
        line = re.sub(r':doc:`([^`]+)`', r'[\1]', line)
        line = re.sub(r':ref:`([^`]+)`', r'[\1]', line)
        
        # Convert RST notes and warnings to Markdown blockquotes
        if line.strip().startswith('.. note::'):
            result_lines.append('> **Note:**')
        elif line.strip().startswith('.. warning::'):
            result_lines.append('> **Warning:**')
        elif line.strip().startswith('.. important::'):
            result_lines.append('> **Important:**')
        else:
            result_lines.append(line)
        
        i += 1
    
    return '\n'.join(result_lines)

def collect_rst_files(directory):
    """Recursively collect all RST files from directory"""
    rst_files = []
    
    for root, dirs, files in os.walk(directory):
        # Skip certain directories
        skip_dirs = ['__pycache__', '_build', 'build', '.git']
        dirs[:] = [d for d in dirs if d not in skip_dirs]
        
        for file in files:
            if file.endswith('.rst'):
                rst_files.append(os.path.join(root, file))
    
    return sorted(rst_files)

def process_file(file_path, base_dir):
    """Process a single RST file"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Get relative path for organization
        rel_path = os.path.relpath(file_path, base_dir)
        
        # Convert RST to Markdown
        markdown_content = convert_rst_to_markdown(content)
        
        return f"\n\n---\n# File: {rel_path}\n---\n\n{markdown_content}"
    
    except Exception as e:
        logger.error(f"Error processing {file_path}: {e}")
        return f"\n\n---\n# File: {os.path.relpath(file_path, base_dir)} (ERROR)\n---\n\nError reading file: {e}\n"

def combine_rst_to_markdown(source_dir, output_file):
    """Main function to combine all RST files into one Markdown file"""
    
    logger.info(f"Scanning directory: {source_dir}")
    rst_files = collect_rst_files(source_dir)
    logger.info(f"Found {len(rst_files)} RST files")
    
    # Start with a header
    combined_content = f"""# LVGL Documentation - Complete Reference
*Generated from RST sources on {os.path.basename(source_dir)}*

This document contains the complete LVGL documentation compiled from multiple RST (reStructuredText) source files.

**Table of Contents**
"""
    
    # Add table of contents
    for rst_file in rst_files:
        rel_path = os.path.relpath(rst_file, source_dir)
        combined_content += f"- [{rel_path}](#{rel_path.replace('/', '-').replace('.rst', '').replace('_', '-')})\n"
    
    combined_content += "\n\n"
    
    # Process each file
    for rst_file in rst_files:
        logger.info(f"Processing: {rst_file}")
        file_content = process_file(rst_file, source_dir)
        combined_content += file_content
    
    # Write the combined file
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(combined_content)
        logger.info(f"Successfully created combined Markdown file: {output_file}")
        return True
    except Exception as e:
        logger.error(f"Error writing output file: {e}")
        return False

if __name__ == "__main__":
    # Configuration
    source_directory = "intermediate"  # RST source files
    output_filename = "LVGL_Complete_Documentation.md"
    
    if not os.path.exists(source_directory):
        logger.error(f"Source directory '{source_directory}' not found")
        exit(1)
    
    success = combine_rst_to_markdown(source_directory, output_filename)
    
    if success:
        file_size = os.path.getsize(output_filename)
        logger.info(f"Combined documentation created successfully!")
        logger.info(f"Output file: {output_filename}")
        logger.info(f"File size: {file_size / 1024:.2f} KB")
    else:
        logger.error("Failed to create combined documentation")
        exit(1)