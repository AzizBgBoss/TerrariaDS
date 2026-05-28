import sys
import re

MAP_FILE = sys.argv[1]

bss = 0
data = 0

with open(MAP_FILE, "r", errors="ignore") as f:
    for line in f:
        # match .bss and .data section sizes
        m = re.search(r"\.(bss|data|sbss|sdata)\s+[\da-fx]+\s+0x([0-9a-fA-F]+)", line)
        if m:
            size = int(m.group(2), 16)
            if m.group(1) in ["bss", "sbss"]:
                bss += size
            else:
                data += size

total = bss + data
limit = 4 * 1024 * 1024

percent = (total / limit) * 100

print("=== DS RAM USAGE ===")
print(f"BSS  : {bss} bytes")
print(f"DATA : {data} bytes")
print(f"TOTAL: {total} bytes")
print(f"LIMIT: {limit} bytes (4MB)")
print(f"USAGE: {percent:.2f}%")
print()