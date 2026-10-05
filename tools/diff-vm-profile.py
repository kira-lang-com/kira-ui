#!/usr/bin/env python3
"""Diff two dispatch profiles: what scrolling costs that a still frame does not.

Reads the two `vm-profile function` listings the profiling runner prints and
prints the per-function delta, largest first. A scroll frame costs far more than
a still one, and the whole-run leaders are dominated by work both runs share, so
the delta is the only listing that points at the scroll cost.
"""
import re
import sys


def load(path):
    counts = {}
    with open(path) as handle:
        for line in handle:
            match = re.match(r"^(\d+)\s+(\S+)\s*$", line)
            if match:
                counts[match.group(2)] = int(match.group(1))
    return counts


def main(argv):
    if len(argv) != 3:
        print(__doc__.strip())
        return 2
    moving, still = load(argv[1]), load(argv[2])
    deltas = [
        (name, moving.get(name, 0), still.get(name, 0))
        for name in set(moving) | set(still)
    ]
    deltas.sort(key=lambda row: row[1] - row[2], reverse=True)
    print("%-30s %12s %12s %12s" % ("function", "scroll", "still", "delta"))
    for name, moving_count, still_count in deltas[:25]:
        print(
            "%-30s %12d %12d %12d"
            % (name, moving_count, still_count, moving_count - still_count)
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))