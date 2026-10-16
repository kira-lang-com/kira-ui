# KiraUI UI Testing

KiraUITesting is XCUITest for KiraUI. A UI test launches the application,
finds its elements as assistive technology reads them (see
[Accessibility.md](Accessibility.md)), and taps, types and scrolls as a
person would. Tests are ordinary `@Test` functions from Foundation's testing
framework, and `expect!` checks them.

```kira
import Foundation
import KiraUI
import KiraUITesting

@Test
function submitsTheField() {
    let app = XCUIApplication()
    app.launch()

    let field = app.textFields.matching(identifier: "Name").element
    field.tap()
    field.typeText("Ada\n")

    expect!(app.staticTexts.matching(identifier: "Hello, Ada").element.exists)
    app.terminate()
}
```

The XCUITest shape it follows is:

```swift
func testSubmitsTheField() {
    let app = XCUIApplication()
    app.launch()

    let field = app.textFields["Name"]
    field.tap()
    field.typeText("Ada\n")

    XCTAssert(app.staticTexts["Hello, Ada"].exists)
    app.terminate()
}
```

Kira has no subscripts, so XCUITest's `app.buttons["Send"]` is written in its
long form, `app.buttons.matching(identifier: "Send").element`. It matches an
element's identifier or its label, as the subscript does.

## Running UI tests

An application keeps its UI tests in a package of their own beside its
sources, `UITests/`, as an Xcode project keeps a UI testing target:

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
each `XCUIApplication` launches its own copy of the application. That copy
draws with no window unless `--windowed` asked for one. It keeps its
`@AppStorage` data in the test's session, so every test starts from the
application as it was installed, and a relaunch in the same test finds what
the last launch stored. When the run ends its sessions are removed, and an
application a test left running ends with its session.

## XCUIApplication

| Member | XCUITest |
|--------|----------|
| `XCUIApplication()` | `XCUIApplication()` |
| `launch()` | `launch()`: ends a running copy first, and returns once the first screen has settled |
| `terminate()` | `terminate()` |
| `launchArguments` | `launchArguments`, after the options every KiraUI application reads |
| `launchEnvironment` | `launchEnvironment`, as `[EnvironmentEntry]` |
| `typeText(text:)` | `typeText(_:)`, into the field holding the keyboard |

An application, an element and a query each find their descendants by type,
as XCUITest's `XCUIElementTypeQueryProvider` does: `groups`, `otherElements`,
`staticTexts`, `images`, `buttons`, `links`, `textFields`,
`secureTextFields`, `searchFields`, `switches`, `checkBoxes`, `sliders`,
`progressIndicators`, `popUpButtons`, `menuButtons`, `tabs`, and
`descendants(matching:)` with an `AccessibilityRole`.

## XCUIElementQuery

| Member | XCUITest |
|--------|----------|
| `matching(identifier:)` | `matching(identifier:)` and the subscript |
| `element` | `element`: the one element it finds |
| `firstMatch` | `firstMatch` |
| `element(boundBy:)` | `element(boundBy:)` |
| `count` | `count` |
| `allElementsBoundByIndex` | `allElementsBoundByIndex` |

## XCUIElement

| Member | XCUITest |
|--------|----------|
| `exists` | `exists` |
| `waitForExistence(timeout:)` | `waitForExistence(timeout:)` |
| `label`, `value`, `placeholderValue`, `identifier` | the same |
| `elementType` | `elementType`, as an `AccessibilityRole` |
| `isEnabled`, `isSelected`, `isHittable` | the same |
| `hasFocus` | `hasFocus`: the field holds the keyboard |
| `frame` | `frame`, in points in the window |
| `tap()`, `doubleTap()` | the same, at the element's center |
| `typeText(text:)` | `typeText(_:)`: the element has to hold the keyboard; a line break presses Return |
| `scroll(byDeltaX:deltaY:)` | `scroll(byDeltaX:deltaY:)`, a scroll wheel over the element |

An element is found afresh on screen every time it is read or acted on.
Reading or acting on one the screen does not show, or one its query finds
more than once, fails the test with what it looked for.

## How it works

Every request waits for the application to settle, as XCUITest waits for
its app to go idle. The application answers once no animation is running in
the interface or in its layers, so a test reads the screen a person would
see once it stopped moving.

The application is launched with `KIRAUI_UI_TEST_SESSION` naming its session
directory. A request is written there whole, by a rename, and the
application takes it on its next frame. It presses, types and scrolls through
the same events a window delivers, so the hit testing, focus and
`.onSubmit` a person meets are what the test meets. Its answer is one line on
its output, which the test waits on. `UITestProtocol.kira` in KiraUI is the
one place the conversation is written and read.
