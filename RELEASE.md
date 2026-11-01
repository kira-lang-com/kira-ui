# 26.10.4

**Project:** Kira UI  
**Release week:** 2026-10-26 – 2026-11-01

## Highlights

The first source release of Kira UI includes:

- A declarative widget layer with composable views, modifiers, and GPU rendering.
- Text, stacks, surfaces, cards, buttons, gradients, and navigation stack and split-view components.
- Materials, layout modifiers, and example applications including the basic demo and theme showcase.

## Changed

- Audited and scoped raw-pointer, foreign-memory and native callback operations throughout view, input, accessibility, navigation, and rendering integrations.
- Split the larger picker and toolbar implementations into `PickerViews.kira` and `ToolbarState.kira` while retaining their source-level APIs.

## Breaking changes

- Affected implementations and examples require a Kira compiler with `unsafe` statement support.

## Known limitations

- The KLF-hosted Kira frontend still needs matching `unsafe` parsing and enforcement.
