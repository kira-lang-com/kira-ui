#!/bin/sh
# Marginal CPU cost of one scroll frame, measured offscreen so no drawable wait
# lands in the number.
#
# The on-screen percentile bench times the frame handler *and* presentation:
# nextDrawable blocks until the compositor hands a surface back, so on a busy GPU
# the p90 is that wait, not our code. This script instead runs the same scripted
# navigation and scroll workload offscreen at two frame counts and reports the
# difference over the difference, which cancels startup and leaves the per-frame
# cost of a frame that is actually scrolling.
#
# usage: tools/bench-scroll-cpu.sh <runner> <image> [repeats]
set -eu

runner=$1
image=$2
repeats=${3:-3}

# The scroll starts partway into both runs, so the frames the longer run adds are
# all scrolling frames and the difference is the cost of scrolling one.
start=10

frames() {
  awk -v n="$1" -v s="$start" 'BEGIN {
    p = ""
    for (f = s; f < n; f += 2) {
      p = p (p == "" ? "" : ";") f ",700,430,0,-460,0,1"
    }
    print p
  }'
}

measure() {
  s=$(date +%s%N)
  KIRA_METAL_OFFSCREEN=1 \
  KIRA_METAL_OFFSCREEN_FRAMES="$1" \
  KIRA_UI_CLICK_SCRIPT='5@81,414;30@583,264;90@1247,32' \
  KIRA_UI_DRAG_SCRIPT="$2" \
  "$runner" "$image" >/dev/null 2>&1
  e=$(date +%s%N)
  echo $(((e - s) / 1000000))
}

i=0
while [ "$i" -lt "$repeats" ]; do
  a=$(measure 120 "$(frames 120)")
  b=$(measure 420 "$(frames 420)")
  awk -v a="$a" -v b="$b" -v i="$i" \
    'BEGIN { printf "run%d  120 scroll frames in %dms, 420 in %dms  ->  %.3f ms/scroll frame\n", i, a, b, (b - a) / 300 }'
  i=$((i + 1))
done