#!/usr/bin/env python3
"""
Fix SPDX-License-Identifier headers to ensure they are the first line (after shebang)
"""

import os
import sys

EXCLUDE_DIRS = ['.git', 'test', 'tests', 'docs', 'man', 'tools', 'misc', 'distribution_integration', '2.0', '2.1', '1.0', 'aidl_api']
EXCLUDE_FILES = ['COPYING', 'README.txt', 'SECURITY.md', 'Makefile.am', 'configure.ac', 'autogen.sh', 'Android.mk', 'Android.bp', 'METADATA', 'NOTICE', 'MODULE_LICENSE_APACHE2', 'OWNERS', 'add_spdx.py', 'fix_spdx.py', 'fix_duplicate_spdx.py']

def should_process_file(filepath):
    for excl in EXCLUDE_DIRS:
        if f'/{excl}/' in filepath or filepath.startswith(f'{excl}/'):
            return False
    for excl in EXCLUDE_FILES:
        if filepath.endswith(f'/{excl}') or filepath == excl:
            return False
    ext = os.path.splitext(filepath)[1]
    if ext not in ['.c', '.cpp', '.h', '.cc', '.cxx', '.hpp', '.hxx', '.aidl']:
        return False
    return True

def fix_spdx_header(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        if 'SPDX-License-Identifier' not in content:
            return False

        lines = content.split('\n')
        spdx_line = None
        spdx_idx = -1
        
        for i, line in enumerate(lines):
            if 'SPDX-License-Identifier' in line:
                spdx_line = line.strip()
                spdx_idx = i
                break
        
        if spdx_line is None:
            return False
            
        # Check if SPDX is already first (after shebang)
        first_content_idx = 0
        if lines and lines[0].startswith('#!'):
            first_content_idx = 1
        
        if spdx_idx == first_content_idx:
            return False  # Already in correct position
        
        # Remove SPDX line from current position
        lines.pop(spdx_idx)
        
        # Insert at correct position
        lines.insert(first_content_idx, spdx_line)
        
        new_content = '\n'.join(lines)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        return True
    except Exception as e:
        print(f"Error processing {filepath}: {e}")
        return False

def main():
    root_dir = sys.argv[1] if len(sys.argv) > 1 else '/mnt/aosp/UPSTREAM/vendor_intel_hardware_interfaces'
    processed = 0
    skipped = 0
    errors = 0

    for dirpath, dirnames, filenames in os.walk(root_dir):
        dirnames[:] = [d for d in dirnames if d not in EXCLUDE_DIRS]
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            relpath = os.path.relpath(filepath, root_dir)
            if should_process_file(relpath):
                if fix_spdx_header(filepath):
                    processed += 1
                else:
                    skipped += 1
    print(f"Fixed: {processed} files")
    print(f"Skipped (already correct): {skipped} files")
    print(f"Errors: {errors} files")

if __name__ == '__main__':
    main()
