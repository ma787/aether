#!/usr/bin/env python3
"""UCI smoke test: uci / isready / position startpos / go depth 1 / bestmove."""

import re
import subprocess
import sys


def main() -> int:
    """Runs UCI smoke tests."""
    exe = sys.argv[1]
    with subprocess.Popen(
        [exe],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    ) as proc:
        out, err = proc.communicate(
            input="uci\nisready\nposition startpos\ngo depth 1\nquit\n",
            timeout=60.0,
        )
        if proc.returncode != 0:
            if err:
                print(err, file=sys.stderr)
            return 1
        if "uciok" not in out or "readyok" not in out:
            print(out, file=sys.stderr)
            return 1
        if not re.search(r"^bestmove\s+\S+", out, re.MULTILINE):
            print(out, file=sys.stderr)
            return 1
        return 0


if __name__ == "__main__":
    raise sys.exit(main())
