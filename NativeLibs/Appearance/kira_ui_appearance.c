#include "kira_ui_appearance.h"

#include <stdlib.h>
#include <string.h>

int kira_ui_appearance_override(void) {
    static int answer = -1;
    if (answer < 0) {
        const char *requested = getenv("KIRA_UI_APPEARANCE");
        answer = 0;
        if (requested != NULL && strcmp(requested, "light") == 0) answer = 1;
        if (requested != NULL && strcmp(requested, "dark") == 0) answer = 2;
    }
    return answer;
}

/* Every platform keeps its colour scheme somewhere different, and none of them
 * expose it through the C standard library, so this is the one place the four
 * answers live. */

#if defined(_WIN32)

#include <windows.h>

/* Windows keeps it in the user's Personalize key. `AppsUseLightTheme` is the one
 * that governs application chrome; `SystemUsesLightTheme` beside it governs the
 * taskbar and the start menu, which is not what a window is asking about.
 *
 * The key is opened ONCE and held. This is asked on every adaptive colour
 * resolution — hundreds of times a frame — and opening a registry key that often
 * would cost more than everything it is being asked about. Querying an open key
 * reads the already-mapped hive, so the answer stays live: the caller sees a
 * scheme change on the frame after the user makes it, with nothing cached on the
 * Kira side to go stale. */
int kira_ui_platform_appearance(void) {
    static HKEY key = NULL;
    if (key == NULL) {
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                          0,
                          KEY_QUERY_VALUE,
                          &key) != ERROR_SUCCESS) {
            key = NULL;
            return 0;
        }
    }

    DWORD light = 0;
    DWORD size = sizeof(light);
    DWORD type = 0;
    LONG status = RegQueryValueExW(key, L"AppsUseLightTheme", NULL, &type, (LPBYTE)&light, &size);

    if (status != ERROR_SUCCESS || type != REG_DWORD) {
        return 0;
    }
    return light != 0 ? 1 : 2;
}

#elif defined(__EMSCRIPTEN__)

#include <emscripten.h>

/* The web states it as a media query, and the query is live: asking again after
 * the user flips their system setting returns the new answer. */
int kira_ui_platform_appearance(void) {
    return emscripten_run_script_int(
        "(window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) ? 2 : 1");
}

#elif defined(__APPLE__)

#include <CoreFoundation/CoreFoundation.h>
#include <TargetConditionals.h>

#if TARGET_OS_OSX

#include <time.h>

/* macOS writes `AppleInterfaceStyle` into the global domain only while the user
 * is in dark mode; its absence IS light mode, which is why this reads a missing
 * value as light rather than as no preference.
 *
 * Unlike the registry key, a preferences read is a round trip through cfprefsd's
 * cache and its search list, and this is asked hundreds of times a rebuild. The
 * answer is held for a tenth of a second: a window still follows the user to
 * the new scheme within a few frames, and a rebuild pays for one read. */
static int kira_ui_appearance_read(void);

int kira_ui_platform_appearance(void) {
    static uint64_t asked_at = 0;
    static int answer = 0;
    uint64_t now = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    if (answer == 0 || now - asked_at > 100000000ULL) {
        answer = kira_ui_appearance_read();
        asked_at = now;
    }
    return answer;
}

static int kira_ui_appearance_read(void) {
    CFPropertyListRef value = CFPreferencesCopyAppValue(CFSTR("AppleInterfaceStyle"),
                                                       kCFPreferencesCurrentApplication);
    if (value == NULL) {
        return 1;
    }
    int dark = 0;
    if (CFGetTypeID(value) == CFStringGetTypeID()) {
        dark = CFStringHasPrefix((CFStringRef)value, CFSTR("Dark"));
    }
    CFRelease(value);
    return dark ? 2 : 1;
}

#else

/* On the UIKit platforms the scheme is a trait of the view hierarchy rather than
 * a preference, so it reaches the toolkit through the window it belongs to and
 * not through this call. */
int kira_ui_platform_appearance(void) {
    return 0;
}

#endif

#else

/* The free desktops publish the scheme over the XDG settings portal, which is a
 * D-Bus conversation rather than a library entry point. `GTK_THEME` is the one
 * value a process can read without one, and a name ending in `:dark` is the
 * convention every toolkit that reads it follows. */

int kira_ui_platform_appearance(void) {
    const char *theme = getenv("GTK_THEME");
    if (theme == NULL) {
        return 0;
    }
    return strstr(theme, ":dark") != NULL ? 2 : 1;
}

#endif
