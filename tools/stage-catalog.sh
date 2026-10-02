#!/bin/sh
# Copies KiraUI's catalog into each example's build bundle, where a running
# example reads it. `kira run` does not refresh the bundle, so run this after
# regenerating Resources/Default.kcui.
set -eu
root=$(cd "$(dirname "$0")/.." && pwd)
catalog="$root/Resources/Default.kcui"

for example in "$root"/Examples/*/; do
    bundle="$example.kira-build/KiraUI.klbundle/resources/Resources"
    mkdir -p "$bundle"
    cp "$catalog" "$bundle/Default.kcui"
    echo "staged $(basename "$example")"
done
