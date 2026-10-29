# KiraUI UI Testing

KiraUITesting is the KiraUI library for UI tests. A UI test launches the
application, finds its elements as assistive technology reads them (see
[Accessibility.md](Accessibility.md)), and taps, types and scrolls as a
person would. Tests are ordinary `@Test` functions from Foundation's testing
framework, and `expect!` checks them.

```kira
import Foundation
import KiraUI
import KiraUITesting

@Test
function submitsTheField() {
    let app = UITestApp()
    app.launch()

    let field = app.textFields.matching(identifier: "Name").element
    field.tap()
    field.typeText("Ada\n")

    expect!(app.staticTexts.matching(identifier: "Hello, Ada").element.exists)
    app.terminate()
}
```

The library has four types:

| Type | What it is |
|------|------------|
| `UITestApp` | The application under test: launched, terminated, typed into. |
| `UITestQueryProvider` | The construct an application, a locator and a query share: descendants by type. |
| `UITestQuery` | The elements of one type, optionally matched by identifier or label. |
| `UITestLocator` | One element on screen, found afresh every time it is read or acted on. |

Kira has no subscripts, so a lookup by identifier is written in its long
form, `app.buttons.matching(identifier: "Send").element`. It matches an
element's identifier or its label.

## Running UI tests

An application keeps its UI tests in a package of their own beside its
sources, `UITests/`:

```
Examples/showcase/
    package.kira
    app/
    UITests/
        package.kira        // depends on KiraUI and KiraUITesting
        app/
            main.kira       // @Main function main() {}
            ShowcaseUITests.kira
```

`ui test`, the KiraUI command line in `tools/ui`, builds the application and
runs the tests against it:

```bash
ui test Examples/showcase
ui test Examples/showcase --windowed
ui test Examples/showcase --tests path/to/OtherUITests
```

Each test runs in a process of its own, as `kira test` runs every test, and
each `UITestApp` launches its own copy of the application. That copy
draws with no window unless `--windowed` asked for one. It keeps its
`@AppStorage` data in the test's session, so every test starts from the
application as it was installed, and a relaunch in the same test finds what
the last launch stored. When the run ends its sessions are removed, and an
application a test left running ends with its session.

## UITestApp

| Member | What it does |
|--------|--------------|
| `UITestApp()` | A handle on the application `ui test` built. |
| `launch()` | Ends a running copy first, and returns once the first screen has settled. |
| `terminate()` | Ends the application. |
| `launchArguments` | The arguments the application is launched with, after the options every KiraUI application reads. |
| `launchEnvironment` | The variables set over the environment the application inherits, as `[EnvironmentEntry]`. |
| `typeText(text:)` | Types into the field holding the keyboard. A line break presses Return. |

## UITestQueryProvider

An application, a locator and a query each find their descendants by type:
`groups`, `otherElements`, `staticTexts`, `images`, `buttons`, `links`,
`textFields`, `secureTextFields`, `searchFields`, `switches`, `checkBoxes`,
`sliders`, `progressIndicators`, `popUpButtons`, `menuButtons`, `tabs`, and
`descendants(matching:)` with an `AccessibilityRole`.

## UITestQuery

| Member | What it does |
|--------|--------------|
| `matching(identifier:)` | The elements whose identifier or label is the given text. |
| `element` | The one element the query finds. |
| `first` | The first element the query finds. |
| `element(at:)` | The element at an index among those the query finds, in the order VoiceOver visits them. |
| `count` | How many elements the query finds now. |
| `allElements` | Every element the query finds, each standing for its place in the order. |

## UITestLocator

| Member | What it does |
|--------|--------------|
| `exists` | Whether the screen shows the element now. |
| `waitForExistence(timeout:)` | Waits up to `timeout` seconds for the element to appear, and answers whether it did. |
| `label`, `value`, `placeholderValue`, `identifier` | What the element reads as. |
| `role` | The element's `AccessibilityRole`. |
| `isEnabled`, `isSelected`, `isHittable` | The element's state. |
| `hasFocus` | Whether the element holds the keyboard. |
| `frame` | The element's box in the window, in points. |
| `tap()`, `doubleTap()` | A tap or double tap at the element's center. |
| `typeText(text:)` | Types into the element, which has to hold the keyboard. A line break presses Return. |
| `scroll(byDeltaX:deltaY:)` | Turns the scroll wheel over the element, by points along each axis. |

An element is found afresh on screen every time it is read or acted on.
Reading or acting on one the screen does not show, or one its query finds
more than once, fails the test with what it looked for.

## How it works

Every request waits for the application to settle. The application answers
once no animation is running in the interface or in its layers, so a test
reads the screen a person would see once it stopped moving.

The application is launched with `KIRAUI_UI_TEST_SESSION` naming its session
directory. A request is written there whole, by a rename, and the
application takes it on its next frame. It presses, types and scrolls through
the same events a window delivers, so the hit testing, focus and
`.onSubmit` a person meets are what the test meets. Its answer is one line on
its output, which the test waits on. `UITestProtocol.kira` in KiraUI is the
one place the conversation is written and read.
