#!/usr/bin/env python3
"""
Add SPDX-License-Identifier headers to all source files in vendor_intel_hardware_interfaces
License: Apache-2.0
"""

import os
import sys

SPDX_HEADER = "/* SPDX-License-Identifier: Apache-2.0 */\n"

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

def has_spdx_header(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read(500)
            return 'SPDX-License-Identifier' in content
    except:
        return False

def add_spdx_header(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        if 'SPDX-License-Identifier' in content:
            return False

        lines = content.split('\n')
        insert_at = 0
        if lines and lines[0].startswith('#!'):
            insert_at = 1

        in_comment = False
        comment_start = -1
        for i, line in enumerate(lines[insert_at:], insert_at):
            stripped = line.strip()
            if not in_comment and stripped.startswith('/*'):
                in_comment = True
                comment_start = i
            if in_comment:
                if '*/' in stripped:
                    insert_at = i + 1
                    break
        if comment_start == -1:
            insert_at = 0
            if lines and lines[0].startswith('#!'):
                insert_at = 1

        new_lines = lines[:insert_at] + [SPDX_HEADER.rstrip()] + lines[insert_at:]
        new_content = '\n'.join(new_lines)

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
                if has_spdx_header(filepath):
                    skipped += 1
                elif add_spdx_header(filepath):
                    processed += 1
                else:
                    errors += 1
    print(f"Processed: {processed} files")
    print(f"Skipped (already has SPDX): {skipped} files")
    print(f"Errors: {errors} files")

if __name__ == '__main__':
    main()
