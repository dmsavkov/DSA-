#!/bin/bash

set -euo pipefail

ITEST_DIR=".itest"
status=true

cleanup()
{
    rm -rf "$ITEST_DIR"
}

trap cleanup EXIT
mkdir -p "$ITEST_DIR"

if ! python3 -c "import numpy" &> /dev/null; then
    echo "numpy not found. Installing..."
    python3 -m pip install --break-system-packages numpy
else
    echo "numpy is already installed."
fi

generate_case()
{
    local n=$1
    local input="$ITEST_DIR/input_${n}.csv"
    local golden="$ITEST_DIR/golden_${n}.csv"
    local output="$ITEST_DIR/output_${n}.csv"

    python3 - "$n" "$input" "$golden" <<'PY'
import sys
import numpy as np

n = int(sys.argv[1])
inp = sys.argv[2]
gold = sys.argv[3]
rng = np.random.default_rng(42 + n)
a = rng.standard_normal((n, n))
a += np.diag(np.sum(np.abs(a), axis=1) + 1.0) # Strictly Diagonally Dominant
b = rng.standard_normal(n)
x = np.linalg.solve(a, b)
aug = np.hstack([a, b.reshape(-1, 1)])
header = ",".join([f"c{j}" for j in range(n)] + ["rhs"])
with open(inp, "w", encoding="utf-8") as f:
    f.write(header + "\n")
    for row in aug:
        f.write(",".join(f"{v:.17g}" for v in row) + "\n")
with open(gold, "w", encoding="utf-8") as f:
    f.write("x\n")
    for v in x:
        f.write(f"{v:.15g}\n")
PY

    ./gauss "$input" >"$output"
    if cmp -s "$golden" "$output"; then
        echo "Gauss n=${n} ok!"
    else
        echo "Gauss n=${n} failed!" >&2
        status=false
    fi
}

generate_case 4
generate_case 12

$status
