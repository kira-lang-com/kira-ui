# KiraUI Navigation

KiraUI follows SwiftUI’s navigation model: the path is data, links append
values, and destination mappings turn those values into screens. Containers
own presentation. A screen never asks whether it is running on a phone or a
desktop.

```kira
NavigationValue SettingsRoute {
    navigationType { return "SettingsRoute" }
}

@State var navigationPath: NavigationPath = NavigationPath {}

NavigationStack(path = navigationPath) {
    VStack {
        NavigationLink(value = SettingsRoute()) {
            Text("Settings")
        }
    }
    .navigationDestination(
        forType = SettingsRoute(),
        destination = SettingsPage()
    )
}
```

The intended SwiftUI shape is:

```swift
NavigationStack(path: $path) {
    NavigationLink("Settings", value: Route.settings)
}
.navigationDestination(for: Route.self) { route in
    SettingsPage(route: route)
}
```

Kira route values are declarations backed by `NavigationValue`. The path stores
heterogeneous `Any NavigationValue` values, so each route remains a typed value;
there is no numeric route discriminator. Kira uses a typed route witness with
`forType` because it does not currently expose Swift's `Route.self` metatype
expression. This is a syntax limitation, not an integer-based navigation model.

## Path and containers

`NavigationPath` provides `append(value:)`, `removeLast()`, `removeAll()`,
`values()`, `lastMatches(value:)`, and `isEmpty()`. `NavigationLink(value:)`
uses the nearest path automatically. The root view is never part of the path,
so it cannot be popped.

A link whose label is a title has a second way to be constructed, matching
SwiftUI's `NavigationLink("Settings", value: route)`:

```kira
NavigationLink(title = "Settings", value = SettingsRoute())
```

Use `NavigationStack` for push navigation. Use `NavigationSplitView` when the
app has a leading navigation column and a detail column:

```kira
NavigationSplitView(path = path) {
    Sidebar()
} detail: {
    Detail(path = path)
}
```

The columns are child slots. The bare trailing block is the sidebar, because
the sidebar is the slot declared first; `content:` and `detail:` name theirs,
the way SwiftUI's labels do. Writing a `content:` block is what makes the view
three-column:

```kira
NavigationSplitView(path = path) {
    Sidebar()
} content: {
    ProjectList(path = path)
} detail: {
    Detail(path = path)
}
```

`NavigationSplitView` automatically collapses to one column below the compact
width threshold. It keeps the leading column on Tablet, PC, and TV. The path is
unchanged by resize, rotation, or collapse, so the same app can move between a
phone stack and a desktop split view without losing navigation state.

## Complex layered example

The runnable app in
[`Examples/navigation-app`](Examples/navigation-app) combines a split
sidebar with a nested detail stack. Its route graph is:

```text
Projects
└── Project overview
    └── Project files
        └── File detail

Activity
└── Activity detail

Settings
└── Account
```

The important part is that every level appends a value to the same path. The
detail stack registers all mappings once:

```kira
NavigationSplitView(path = navigationPath) {
    WorkspaceSidebar()
} detail: {
    WorkspaceDetail(path = navigationPath)
}

NavigationStack(path = navigationPath) {
    WorkspaceHome(path = navigationPath)
}
.navigationDestination(
    forType = ProjectsRoute(),
    destination = ProjectsPage(path = navigationPath)
)
.navigationDestination(
    forType = ProjectOverviewRoute(),
    destination = ProjectOverviewPage(path = navigationPath)
)
.navigationDestination(
    forType = ProjectFilesRoute(),
    destination = ProjectFilesPage(path = navigationPath)
)
```

For a deep link, the same path can be populated in order:

```kira
path.removeAll()
path.append(value = ProjectsRoute())
path.append(value = ProjectOverviewRoute())
path.append(value = ProjectFilesRoute())
```

On a wide window, the sidebar remains visible while the detail stack changes.
On a compact window, `NavigationSplitView` presents that same detail stack as a
phone-style push flow with Back navigation.

## Device behavior

The policy is automatic and driven by the live viewport:

- Phone / compact width: one column, back navigation, 44-point targets.
- Tablet: split at regular width, 48-point targets.
- PC: split at regular width, 40-point targets.
- TV: split with a 64-point target and toolbar size for 10-foot viewing.

`KIRA_UI_DEVICE=phone|tablet|pc|tv` is available for host and snapshot tests;
normal application code does not need to branch on it. TV links use the same
focusable KiraUI hit regions as pointer and touch links. Spatial remote focus
(Up/Down/Left/Right) belongs in the foundation input layer and should be added
there, rather than leaking TV-specific navigation code into every screen.

The design follows Apple’s current guidance: `NavigationStack` represents a
stack of destinations, while `NavigationSplitView` represents two or three
columns and collapses at narrow sizes. See Apple’s [NavigationStack
documentation](https://developer.apple.com/documentation/swiftui/navigationstack)
and [NavigationSplitView documentation](https://developer.apple.com/documentation/swiftui/navigationsplitview).
