#!/usr/bin/env python3
"""Fail when editor widgets bypass KiraUI's declarative Liquid Glass API.

This repository check keeps low-level retained-glass implementation limited to
existing framework infrastructure. Add a new public effect through KiraUI,
not by constructing a separate CAGlass, a material, or a compositor layer in
an application's widget.
"""
from pathlib import Path
import re
import sys

root = Path(__file__).resolve().parents[1]
editor_views = root.parent / 'project-matter/apps/nexteditor/app/Views'
# Internal glass endpoints still used by platform-specific chrome and effects.
# Counts are ceilings, so an additional ad-hoc rendering path is a failure.
internal_glass_sites = {
    'Shapes.kira': 1,
    'Glass.kira': 1,
    'Controls.kira': 1,
    'MenuMorph.kira': 1,
    'NavigationSplitView.kira': 1,
    'Scroll.kira': 1,
    'Menu.kira': 1,
}
raw_glass = re.compile(r'(?<![A-Za-z0-9_])(?:\.Glass\s*\(|CAGlass\s*\{|CAGlassMaterial\s*\{)')
app_bypasses = re.compile(
    r'(?<![A-Za-z0-9_])(?:\.Glass\s*\(|CAGlass\s*\{|CAGlassMaterial\s*\{'
    r'|liquidGlassDescription\s*\(|liquidGlassContent\s*\('
    r'|interactiveGlassDescription\s*\(|tabBarCapsule\s*\()'
)

def stripped(text: str) -> str:
    # Ignore comments, but not source: contract text in documentation does not
    # constitute an implementation or a bypass.
    return '\n'.join(line.split('//', 1)[0] for line in text.splitlines())

failures = []
for path in sorted(editor_views.rglob('*.kira')):
    for num, line in enumerate(stripped(path.read_text()).splitlines(), 1):
        if app_bypasses.search(line):
            failures.append(f'{path}:{num}: use .glassEffect() or KiraUI panel controls')

for path in sorted((root / 'app').glob('*.kira')):
    contents = stripped(path.read_text())
    count = len(re.findall(r'(?<![\w])(?:\.Glass\s*\()', contents))
    allowed = internal_glass_sites.get(path.name, 0)
    if count > allowed:
        failures.append(f'{path}: {count} low-level glass assignments; allowed {allowed}')

if failures:
    print('\n'.join(failures), file=sys.stderr)
    sys.exit(1)
print('Glass contract: OK (editor uses declarative glass; framework raw sites bounded)')
