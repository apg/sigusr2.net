#!/usr/bin/env python3

import sys
import re

def main():
    content = sys.stdin.read()
    lines = content.split('\n')
    
    # Extract metadata from % lines
    title = ""
    publish_date = ""
    
    # Find the first three % lines
    percent_lines = []
    for line in lines:
        if line.startswith('%'):
            percent_lines.append(line)
        if len(percent_lines) >= 3:
            break
    
    if len(percent_lines) >= 1:
        title = percent_lines[0][1:].strip()  # Remove % and whitespace
    if len(percent_lines) >= 3:
        publish_date = percent_lines[2][1:].strip()  # Remove % and whitespace
    
    # Find content after the % lines
    content_start = 0
    percent_count = 0
    for i, line in enumerate(lines):
        if line.startswith('%'):
            percent_count += 1
            if percent_count == 3:
                content_start = i + 1
                break
    
    # Extract content lines (skip empty lines after metadata)
    content_lines = lines[content_start:]
    while content_lines and content_lines[0].strip() == '':
        content_lines.pop(0)
    
    # Build anchor dictionary from reference links at bottom
    anchor_dict = {}
    
    # Process lines from bottom to find reference links
    for line in reversed(content_lines):
        line = line.strip()
        # Match pattern like [foo]: http://example.com
        match = re.match(r'^\s*\[([^\]]+)\]:\s*(.+)$', line)
        if match:
            anchor_key = match.group(1)
            raw_url = match.group(2).strip()
            # Extract just the URL part, removing any trailing junk
            url_match = re.search(r'(https?://[^\s]+)', raw_url)
            if url_match:
                clean_url = url_match.group(1)
                # Remove common trailing punctuation that's not part of URLs
                clean_url = re.sub(r'[.,;:!?"\')\]]+$', '', clean_url)
                anchor_dict[anchor_key] = clean_url
            else:
                # If no http/https URL found, use the original (might be a relative URL)
                anchor_dict[anchor_key] = raw_url
    
    # Remove reference link lines from content
    filtered_content = []
    for line in content_lines:
        if not re.match(r'^\s*\[[^\]]+\]:\s*.+$', line.strip()):
            filtered_content.append(line)
    
    # Replace [WORD][anchor] with [WORD](URL)
    processed_content = []
    for line in filtered_content:
        # Find all [WORD][anchor] patterns
        def replace_link(match):
            word = match.group(1)
            anchor = match.group(2)
            if anchor in anchor_dict:
                return f'[{word}]({anchor_dict[anchor]})'
            else:
                return match.group(0)  # Return unchanged if anchor not found
        
        processed_line = re.sub(r'\[([^\]]+)\]\[([^\]]+)\]', replace_link, line)
        processed_content.append(processed_line)
    
    # Output the transformed document
    print("---")
    print(f"title: {title}")
    print(f"publish_date: {publish_date}")
    print("blog: true")
    print("---")
    
    for line in processed_content:
        print(line)

if __name__ == "__main__":
    main()