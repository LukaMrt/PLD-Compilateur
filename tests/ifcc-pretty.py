#!/usr/bin/env python3
import sys
import re
import os

GREEN  = "\033[32m"
RED    = "\033[31m"
BOLD   = "\033[1m"
DIM    = "\033[2m"
RESET  = "\033[0m"

OUTPUT_DIR = os.path.join(os.path.dirname(__file__), '..', 'ifcc-test-output')

passed = []
failed = []  # list of (raw_name, reason)

current_raw  = None  # full subdir name, e.g. "tests-cases-5_addition"
current_name = None  # stripped, e.g. "5_addition"

def parse_name(raw):
    m = re.match(r'^(\d+)_(.+)$', raw)
    if m:
        return int(m.group(1)), m.group(2).replace('_', ' ')
    return 0, raw

def fmt_name(raw, num_width):
    num, label = parse_name(raw)
    return f"{str(num).rjust(num_width)} - {label}"

def read_file(path):
    try:
        content = open(path).read().strip()
        content = re.sub(r'exit status:\s*', '', content).strip()
        return content
    except FileNotFoundError:
        return None

for line in sys.stdin:
    line = line.rstrip()

    m = re.match(r'^TEST-CASE:\s+(.+)$', line)
    if m:
        current_raw  = m.group(1)
        current_name = re.sub(r'^tests-cases-', '', current_raw)
        continue

    if line == "TEST OK":
        if current_name:
            passed.append(current_name)
        current_raw = current_name = None
        continue

    m = re.match(r'^TEST FAIL\s*(?:\((.+)\))?$', line)
    if m:
        reason = m.group(1) or "unknown reason"
        if current_name:
            failed.append((current_raw, current_name, reason))
        current_raw = current_name = None
        continue

    # Non-test line (build output, etc.) — pass through immediately
    print(line, flush=True)

# ── Results ──────────────────────────────────────────────────────────────────

passed.sort(key=lambda n: parse_name(n)[0])
failed.sort(key=lambda x: parse_name(x[1])[0])

all_names = [n for n in passed] + [n for _, n, _ in failed]
num_width = max((len(str(parse_name(n)[0])) for n in all_names), default=1)

print()
print(f"{BOLD}Results{RESET}")
print("─" * 40)
for name in passed:
    print(f"  {GREEN}✓{RESET}  {fmt_name(name, num_width)}")

total = len(passed) + len(failed)
print()
if failed:
    print(f"{BOLD}Failures{RESET}")
    print("─" * 40)
    for raw, name, reason in failed:
        log_dir = os.path.normpath(os.path.join(OUTPUT_DIR, raw))
        print(f"  {RED}✗{RESET}  {BOLD}{fmt_name(name, num_width)}{RESET}")
        print(f"      {DIM}{reason}{RESET}")

        if "different results" in reason:
            expected = read_file(os.path.join(log_dir, 'gcc-3-execute.txt'))
            got      = read_file(os.path.join(log_dir, 'ifcc-3-execute.txt'))
            if expected is not None and got is not None:
                print(f"      {GREEN}expected:{RESET} {expected if expected else '(empty)'}")
                print(f"      {RED}got:     {RESET} {got      if got      else '(empty)'}")

        print(f"      {DIM}logs: {log_dir}{RESET}")
        print()

score_color = GREEN if not failed else RED
print(f"{score_color}{BOLD}{len(passed)}/{total} passed{RESET}")
print()
