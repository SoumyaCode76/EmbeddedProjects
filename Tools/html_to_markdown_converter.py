#!/usr/bin/env python3
"""
HTML to Markdown Converter for LVGL Documentation
Alternative approach: Convert HTML output to Markdown for better formatting preservation
"""

import os
import re
from pathlib import Path
import logging
from bs4 import BeautifulSoup
import html2text

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def setup_html2text_converter():
    """Configure html2text converter for optimal Markdown output"""
    h = html2text.HTML2Text()
    h.ignore_links = False
    h.ignore_images = False
    h.ignore_emphasis = False
    h.body_width = 0  # No line wrapping
    h.unicode_snob = True  # Use unicode characters
    h.mark_code = True
    h.wrap_links = False
    h.skip_internal_links = False
    return h

def clean_markdown_content(content):
    """Clean up the converted Markdown content"""
    
    # Remove excessive blank lines
    content = re.sub(r'\n{3,}', '\n\n', content)
    
    # Fix code block markers
    content = re.sub(r'```\s*\n\s*```', '```', content)
    
    # Clean up table formatting
    content = re.sub(r'\|(\s*\|)+', '|', content)
    
    # Fix header spacing
    content = re.sub(r'^(#{1,6})\s*$', '', content, flags=re.MULTILINE)
    
    return content.strip()

def process_html_file(file_path, converter, base_dir):
    """Process a single HTML file and convert to Markdown"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            html_content = f.read()
        
        # Parse HTML and extract main content
        soup = BeautifulSoup(html_content, 'html.parser')
        
        # Try to find main content area (common in Sphinx themes)
        main_content = None
        for selector in ['.document', '#document', 'main', '.content', '.body', '.rst-content']:
            element = soup.select_one(selector)
            if element:
                main_content = element
                break
        
        if not main_content:
            # Fallback: remove header, footer, navigation
            for tag in soup(['nav', 'header', 'footer', 'aside']):
                tag.decompose()
            main_content = soup.find('body') or soup
        
        # Convert to Markdown
        markdown_content = converter.handle(str(main_content))
        markdown_content = clean_markdown_content(markdown_content)
        
        # Get relative path for organization
        rel_path = os.path.relpath(file_path, base_dir)
        
        return f"\n\n---\n# {rel_path}\n---\n\n{markdown_content}"
    
    except Exception as e:
        logger.error(f"Error processing {file_path}: {e}")
        return f"\n\n---\n# {os.path.relpath(file_path, base_dir)} (ERROR)\n---\n\nError processing file: {e}\n"

def collect_html_files(directory):
    """Recursively collect all HTML files from directory"""
    html_files = []
    
    for root, dirs, files in os.walk(directory):
        # Skip certain directories
        skip_dirs = ['_static', '_sources', '.doctrees']
        dirs[:] = [d for d in dirs if d not in skip_dirs]
        
        for file in files:
            if file.endswith('.html') and file != 'search.html':
                html_files.append(os.path.join(root, file))
    
    return sorted(html_files)

def combine_html_to_markdown(source_dir, output_file):
    """Main function to combine all HTML files into one Markdown file"""
    
    try:
        import html2text
    except ImportError:
        logger.error("html2text package not found. Install with: pip install html2text beautifulsoup4")
        return False
    
    logger.info(f"Scanning HTML directory: {source_dir}")
    html_files = collect_html_files(source_dir)
    logger.info(f"Found {len(html_files)} HTML files")
    
    if not html_files:
        logger.error("No HTML files found")
        return False
    
    # Setup converter
    converter = setup_html2text_converter()
    
    # Start with a header
    combined_content = f"""# LVGL Documentation - Complete Reference (HTML Export)
*Generated from HTML build output*

This document contains the complete LVGL documentation compiled from HTML files with preserved formatting.

**Total Files Processed**: {len(html_files)}

**Table of Contents**
"""
    
    # Add table of contents
    for html_file in html_files:
        rel_path = os.path.relpath(html_file, source_dir)
        anchor = rel_path.replace('/', '-').replace('\\', '-').replace('.html', '').replace('_', '-')
        combined_content += f"- [{rel_path}](#{anchor})\n"
    
    combined_content += "\n\n"
    
    # Process each file
    for html_file in html_files:
        logger.info(f"Processing: {html_file}")
        file_content = process_html_file(html_file, converter, source_dir)
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
    html_source_directory = "build/html"  # HTML build output
    output_filename = "LVGL_Complete_Documentation_HTML.md"
    
    if not os.path.exists(html_source_directory):
        logger.error(f"HTML source directory '{html_source_directory}' not found")
        logger.info("Run 'python build.py html' first to generate HTML documentation")
        exit(1)
    
    success = combine_html_to_markdown(html_source_directory, output_filename)
    
    if success:
        file_size = os.path.getsize(output_filename)
        logger.info(f"HTML-based documentation created successfully!")
        logger.info(f"Output file: {output_filename}")
        logger.info(f"File size: {file_size / 1024:.2f} KB")
    else:
        logger.error("Failed to create HTML-based documentation")
        exit(1)