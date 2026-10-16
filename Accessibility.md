# KiraUI Accessibility

KiraUI follows SwiftUI's accessibility model. Every view a person can read or
act on is an element. Containers are not elements: their elements stand in
the window's list in the order they were written. The accessibility modifiers
change what an element says and does, or how a subtree makes elements.

```kira
VStack {
    Text("Battery")
    Text("80%")
}
.accessibilityElement(children: .Combine)

Image(name: "status")
    .accessibilityLabel("Online")
    .accessibilityIdentifier("status")

Button(title: "Archive", action: archive)
    .accessibilityHint("Moves the message out of the inbox")
```

The SwiftUI shape it follows is:

```swift
VStack {
    Text("Battery")
    Text("80%")
}
.accessibilityElement(children: .combine)
```

## The elements views make

| View | Role | Says |
|------|------|------|
| `Text` | `.StaticText`, `isStaticText` | its words |
| `Image(name:)` | `.Image`, `isImage` | its name |
| `Icon` | `.Image`, `isImage` | nothing until labeled |
| `Label` | `.StaticText` | its title; the glyph is part of it |
| `Button` | `.Button`, `isButton` | its label, merged into one element; activating it presses it |
| `Link` | `.Link`, `isLink` | its label; activating it opens the URL |
| `NavigationLink` | `.Button`, `isButton` (`isSelected` in a sidebar) | its label; activating it goes to its value |
| `Toggle` | `.Switch`, or `.CheckBox` on a Mac; `isToggle` | its title, value `1` or `0`; activating it flips it |
| `Slider` | `.Slider`, adjustable | value as a percent; adjusting steps it by a tenth |
| `ProgressView` | `.ProgressIndicator`, `updatesFrequently` | value as a percent |
| `TextField` / `SecureField` | `.TextField` / `.SecureTextField` | its title, its text (dots when secure), its prompt as the placeholder |
| `Section` header | `isHeader` | the header's words |
| `.onClick { }` | its content's elements | activating one runs the action, as a tap gesture's view does |

A disabled control is still an element, with `isEnabled` false and no
default action.

## Modifiers

| Modifier | SwiftUI |
|----------|---------|
| `.accessibilityLabel(label:)` | `.accessibilityLabel(_:)` |
| `.accessibilityValue(value:)` | `.accessibilityValue(_:)` |
| `.accessibilityHint(hint:)` | `.accessibilityHint(_:)` |
| `.accessibilityIdentifier(identifier:)` | `.accessibilityIdentifier(_:)` |
| `.accessibilityAddTraits(traits:)` | `.accessibilityAddTraits(_:)` |
| `.accessibilityRemoveTraits(traits:)` | `.accessibilityRemoveTraits(_:)` |
| `.accessibilityHeading(level:)` | `.accessibilityHeading(_:)` |
| `.accessibilityInputLabels(labels:)` | `.accessibilityInputLabels(_:)` |
| `.accessibilitySortPriority(priority:)` | `.accessibilitySortPriority(_:)` |
| `.accessibilityAction(kind:handler:)` | `.accessibilityAction(_:_:)`, and `.accessibilityAction(named:)` as `kind: .Named("Delete")` |
| `.accessibilityAdjustableAction(handler:)` | `.accessibilityAdjustableAction(_:)` |
| `.accessibilityRespondsToUserInteraction(responds:)` | `.accessibilityRespondsToUserInteraction(_:)` |
| `.accessibilityElement(children:)` | `.accessibilityElement(children:)`: `.Ignore`, `.Contain`, `.Combine` |
| `.accessibilityHidden(hidden:)` | `.accessibilityHidden(_:)` |
| `.accessibilityRepresentation(representation:)` | `.accessibilityRepresentation(representation:)` |

A modifier changes the elements the view it is on makes. That is the view's
own element when it is one, and otherwise each outermost element inside it.
A view that makes none, such as a shape or a color, becomes an element to
carry the change. Written twice, the outer modifier wins. A hidden view
stays hidden, whatever is said about it outside.

Traits combine with `|`, as `EdgeSet` does:

```kira
.accessibilityAddTraits(AccessibilityTraits.isButton | AccessibilityTraits.isSelected)
```

## Environment

The platform's settings are read before each rebuild:

```kira
@Environment(\.accessibilityReduceMotion) let reduceMotion: Bool
```

`accessibilityReduceMotion`, `accessibilityReduceTransparency`,
`accessibilityDifferentiateWithoutColor`, `accessibilityInvertColors`,
`accessibilityVoiceOverEnabled` and `colorSchemeContrast` (`.Standard`,
`.Increased`) come from the Windows system parameters, Apple's universal
access preferences and UIAccessibility, and the web's media queries. Linux
publishes these settings only over the XDG settings portal, which KiraUI
does not read, so they are all off there. Every motion in KiraUI follows
`accessibilityReduceMotion`.

## Reading the tree

After each layout the window reads its laid-out tree into
`AccessibilityElement`s. `accessibilityElementsOnScreen()` answers them until
the next build. Each element has its role, label, value, hint, placeholder,
identifier, traits, heading level, input labels, `isEnabled`, its `frame` in
the window in points, and the elements it holds. It runs its actions with
`activate()`, `perform(kind:)` and `adjustValue(direction:)`.

`accessibilityElementsForTest(root:width:height:)` builds and lays out a view
with no window and answers the same elements. KiraUI's own tests use it, in
`tests/kiraui_kik/app/AccessibilityTests.kira`.

A layer carries only the key of the element it is (`accessibilityElement` on
`CALayerDescription`), as it carries only the key of its press. What the
element says and the closures it runs are registered for the build that
described them.
