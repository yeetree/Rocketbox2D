#!/usr/bin/env python3
import os
import sys
from pathlib import Path

def grep_todos(root_dir):
    target = "TODO: "
    results = []
    root = Path(root_dir).resolve()

    # Loop through all files in directory
    for path in root.rglob("*"):
        if path.is_dir():
            continue
            
        try:
            rel_path = path.relative_to(root)
        except ValueError:
            continue # Path isn't relative to root

        # Read file and look for TODO
        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                for line_num, line in enumerate(f, 1):
                    if target in line:
                        results.append({
                            "path": str(rel_path),
                            "line": line_num,
                            "content": line.strip()
                        })
        except (PermissionError, FileNotFoundError):
            continue

    # Sort results by filepath and line number
    results.sort(key=lambda x: (x["path"], x["line"]))


    # Print results
    for item in results:
        print(f"\033[35m{item['path']}\033[0m:\033[32m{item['line']}\033[0m: {item['content']}")

if __name__ == "__main__":
    todo_dir = "."
    if(len(sys.argv)) > 1:
        todo_dir = sys.argv[1]
    grep_todos(todo_dir)