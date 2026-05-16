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

SAMPLE=$'Hi \xD0\x93 \xF0\x9F\x9A\x80'
printf '%s' "$SAMPLE" >"$ITEST_DIR/sample.utf8"

run_roundtrip()
{
    local mode_fwd=$1
    local mode_back=$2
    local ext=$3
    local label=$4

    ./utfencoder "$mode_fwd" <"$ITEST_DIR/sample.utf8" >"$ITEST_DIR/round.$ext"
    ./utfencoder "$mode_back" <"$ITEST_DIR/round.$ext" >"$ITEST_DIR/round.out.utf8"

    if cmp -s "$ITEST_DIR/sample.utf8" "$ITEST_DIR/round.out.utf8"; then
        echo "$label roundtrip ok!"
    else
        echo "$label roundtrip failed!" >&2
        status=false
    fi
}

run_roundtrip -8to16 -16to8 utf16 "UTF-8/UTF-16"
run_roundtrip -8to32 -32to8 utf32 "UTF-8/UTF-32"

./utfencoder -8to16 <"$ITEST_DIR/sample.utf8" >"$ITEST_DIR/via16.utf16"
./utfencoder -16to32 <"$ITEST_DIR/via16.utf16" >"$ITEST_DIR/via32.utf32"
./utfencoder -32to8 <"$ITEST_DIR/via32.utf32" >"$ITEST_DIR/pipeline.out.utf8"

if cmp -s "$ITEST_DIR/sample.utf8" "$ITEST_DIR/pipeline.out.utf8"; then
    echo "UTF-8 -> UTF-16 -> UTF-32 -> UTF-8 pipeline ok!"
else
    echo "UTF-8 -> UTF-16 -> UTF-32 -> UTF-8 pipeline failed!" >&2
    status=false
fi

python3 -c 'import sys; sys.stdout.buffer.write(sys.stdin.buffer.read().decode("utf-8").encode("utf-16-le"))' \
    <"$ITEST_DIR/sample.utf8" >"$ITEST_DIR/oracle.utf16"

if cmp -s "$ITEST_DIR/via16.utf16" "$ITEST_DIR/oracle.utf16"; then
    echo "UTF-16 oracle ok!"
else
    echo "UTF-16 oracle failed!" >&2
    status=false
fi

./utfencoder -8to32 <"$ITEST_DIR/sample.utf8" >"$ITEST_DIR/via32direct.utf32"

python3 -c 'import sys; sys.stdout.buffer.write(sys.stdin.buffer.read().decode("utf-8").encode("utf-32-le"))' \
    <"$ITEST_DIR/sample.utf8" >"$ITEST_DIR/oracle.utf32"

if cmp -s "$ITEST_DIR/via32direct.utf32" "$ITEST_DIR/oracle.utf32"; then
    echo "UTF-32 oracle ok!"
else
    echo "UTF-32 oracle failed!" >&2
    status=false
fi

./utfencoder -8to32 <"$ITEST_DIR/sample.utf8" >"$ITEST_DIR/sample.utf32"
./utfencoder -32to16 <"$ITEST_DIR/sample.utf32" >"$ITEST_DIR/utf32to16.utf16"
./utfencoder -16to32 <"$ITEST_DIR/via16.utf16" >"$ITEST_DIR/utf16to32.utf32"

if cmp -s "$ITEST_DIR/sample.utf32" "$ITEST_DIR/utf16to32.utf32"; then
    echo "UTF-32/UTF-16 cross-path ok!"
else
    echo "UTF-32/UTF-16 cross-path failed!" >&2
    status=false
fi

$status
