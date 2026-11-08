DO NOT create any "smoke test", "probe" and anything that isn't meant to either: showcase Kira UI or be driven by a real human. Anything called "smoke" or "probe" is outright **banned** from this repo.

This library has to be idiomatic, flawless, beautiful to read, kira native, using Kira's first party syntax. A single abstraction of the wrong shape, one stray helper, or one bad design and we FAILED.

Write KiraUI like you would write SwiftUI. An application on it uses `Views/` for pages and `Views/Components/` for every component, in nested folders like most SwiftUI apps.

## SwiftUI API parity

When a UI port needs a SwiftUI API missing from KiraUI, read its current SDK interface first. Implement the **whole public overload family**, its default arguments, supported enum cases and observable behavior in native KiraUI (and KCoreAnimation where necessary), not only the spelling one caller happens to use. Preserve SwiftUI's call-site structure and layout contracts in Kira syntax. Add focused compile/behavior tests for the overloads. Where a dependency such as localization resources, platform presentation or a backend service does not exist yet, document the missing contract and do not advertise a no-op overload as parity.

# Specs

Any spec is the rule. Read it before changing what it covers and do exactly what it says. If the spec does not say it, it does not exist: never invent a layout, encoding, convention or meaning to make a case work, and never route around a limit the spec sets. A case the spec does not cover is a question for the user, not a decision.

## Release notes

Update `RELEASE.md` whenever a user-facing feature or fix is added. Keep it focused on the current package version: when the version changes, replace the heading and carry forward only notes for the new release. Write concrete, accurate notes before the change lands; the GitHub release workflow publishes this file.
