DO NOT create any "smoke test", "probe" and anything that isn't meant to either: showcase Kira UI or be driven by a real human. Anything called "smoke" or "probe" is outright **banned** from this repo.

This library has to be idiomatic, flawless, beautiful to read, kira native, using Kira's first party syntax. A single abstraction of the wrong shape, one stray helper, or one bad design and we FAILED.

Write KiraUI like you would write SwiftUI. An application on it uses `Views/` for pages and `Views/Components/` for every component, in nested folders like most SwiftUI apps.

## Where the default style's values live

`DefaultStyle`'s colours and text metrics come from KCoreUI's default catalog, not from literals in `Styles.kira`. The catalog is the source of truth.

- **Changing a value** means changing `Resources/Default.kcui` in the `kcoreui` repository, then running `kira run tools/tokens` from this repository root to rewrite `app/CatalogTokens.kira`. Never edit that file by hand.

- **Why it is generated rather than resolved live.** `DefaultStyle` fills seven token fields, each from its own field default, and a field default is evaluated on every construction. Resolving live costs a catalog load per token set: measured at 1.35ms per style construction against 6us for literals, and a theme constructs eleven styles. `app/Catalog.kira` holds the live path, which is what the generator and the drift test use, and what a threaded `UIResources` will use when the theme vocabulary is retired.

- **The drift guard.** `tests/kiraui_kik` resolves the same names live and compares them to the generated literals, so a catalog changed without re-running the generator fails rather than shipping. Run `kira test tests/kiraui_kik` before claiming a token change is done.
