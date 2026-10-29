#include "kira_ui_appearance.h"

/* The platforms' accessibility settings, beside the colour scheme and read
 * from the same places: the Windows system parameters and Personalize key,
 * Apple's universal access preferences and UIAccessibility, and the web's
 * media queries. Linux publishes them over the XDG settings portal, a D-Bus
 * conversation rather than a value a process can read, so it has no row. */

enum {
    KIRA_UI_REDUCE_MOTION = 1,
    KIRA_UI_REDUCE_TRANSPARENCY = 2,
    KIRA_UI_INCREASE_CONTRAST = 3,
    KIRA_UI_DIFFERENTIATE_WITHOUT_COLOR = 4,
    KIRA_UI_INVERT_COLORS = 5,
    KIRA_UI_SCREEN_READER = 6
};

#if defined(_WIN32)

#include <windows.h>

/* Transparency effects are a Personalize value, as the colour scheme is; the
 * rest are system parameters. Windows keeps no setting for inverting colours
 * or for differentiating without colour. */
static int kira_ui_transparency_reduced(void) {
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
    DWORD enabled = 1;
    DWORD size = sizeof(enabled);
    DWORD type = 0;
    if (RegQueryValueExW(key, L"EnableTransparency", NULL, &type, (LPBYTE)&enabled, &size) != ERROR_SUCCESS ||
        type != REG_DWORD) {
        return 0;
    }
    return enabled == 0;
}

int kira_ui_accessibility_setting(int setting) {
    switch (setting) {
    case KIRA_UI_REDUCE_MOTION: {
        BOOL animates = TRUE;
        if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animates, 0)) {
            return 0;
        }
        return !animates;
    }
    case KIRA_UI_REDUCE_TRANSPARENCY:
        return kira_ui_transparency_reduced();
    case KIRA_UI_INCREASE_CONTRAST: {
        HIGHCONTRASTW contrast;
        contrast.cbSize = sizeof(contrast);
        if (!SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0)) {
            return 0;
        }
        return (contrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    }
    case KIRA_UI_SCREEN_READER: {
        BOOL running = FALSE;
        if (!SystemParametersInfoW(SPI_GETSCREENREADER, 0, &running, 0)) {
            return 0;
        }
        return running != FALSE;
    }
    default:
        return 0;
    }
}

#elif defined(__EMSCRIPTEN__)

#include <emscripten.h>

/* The web states each as a media query, live like the colour scheme's. It
 * keeps no query for differentiating without colour or for a screen reader. */
int kira_ui_accessibility_setting(int setting) {
    switch (setting) {
    case KIRA_UI_REDUCE_MOTION:
        return emscripten_run_script_int(
            "(window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches) ? 1 : 0");
    case KIRA_UI_REDUCE_TRANSPARENCY:
        return emscripten_run_script_int(
            "(window.matchMedia && window.matchMedia('(prefers-reduced-transparency: reduce)').matches) ? 1 : 0");
    case KIRA_UI_INCREASE_CONTRAST:
        return emscripten_run_script_int(
            "(window.matchMedia && window.matchMedia('(prefers-contrast: more)').matches) ? 1 : 0");
    case KIRA_UI_INVERT_COLORS:
        return emscripten_run_script_int(
            "(window.matchMedia && window.matchMedia('(inverted-colors: inverted)').matches) ? 1 : 0");
    default:
        return 0;
    }
}

#elif defined(__APPLE__)

#include <CoreFoundation/CoreFoundation.h>
#include <TargetConditionals.h>

#if TARGET_OS_OSX

#include <time.h>

/* macOS keeps them in the universal access domain. A preferences read is a
 * round trip through cfprefsd, as the colour scheme's is, so each answer is
 * held for a tenth of a second. */
static int kira_ui_universal_access(CFStringRef name) {
    Boolean valid = false;
    Boolean on = CFPreferencesGetAppBooleanValue(name, CFSTR("com.apple.universalaccess"), &valid);
    return valid && on;
}

static int kira_ui_accessibility_read(int setting) {
    switch (setting) {
    case KIRA_UI_REDUCE_MOTION:
        return kira_ui_universal_access(CFSTR("reduceMotion"));
    case KIRA_UI_REDUCE_TRANSPARENCY:
        return kira_ui_universal_access(CFSTR("reduceTransparency"));
    case KIRA_UI_INCREASE_CONTRAST:
        return kira_ui_universal_access(CFSTR("increaseContrast"));
    case KIRA_UI_DIFFERENTIATE_WITHOUT_COLOR:
        return kira_ui_universal_access(CFSTR("differentiateWithoutColor"));
    case KIRA_UI_INVERT_COLORS:
        return kira_ui_universal_access(CFSTR("whiteOnBlack"));
    case KIRA_UI_SCREEN_READER:
        return kira_ui_universal_access(CFSTR("voiceOverOnOffKey"));
    default:
        return 0;
    }
}

int kira_ui_accessibility_setting(int setting) {
    static uint64_t asked_at[KIRA_UI_SCREEN_READER + 1];
    static int answer[KIRA_UI_SCREEN_READER + 1];
    if (setting < KIRA_UI_REDUCE_MOTION || setting > KIRA_UI_SCREEN_READER) {
        return 0;
    }
    uint64_t now = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    if (asked_at[setting] == 0 || now - asked_at[setting] > 100000000ULL) {
        answer[setting] = kira_ui_accessibility_read(setting);
        asked_at[setting] = now;
    }
    return answer[setting];
}

#else

/* UIKit answers each through a C function of UIAccessibility's. */
extern _Bool UIAccessibilityIsReduceMotionEnabled(void);
extern _Bool UIAccessibilityIsReduceTransparencyEnabled(void);
extern _Bool UIAccessibilityDarkerSystemColorsEnabled(void);
extern _Bool UIAccessibilityShouldDifferentiateWithoutColor(void);
extern _Bool UIAccessibilityIsInvertColorsEnabled(void);
extern _Bool UIAccessibilityIsVoiceOverRunning(void);

int kira_ui_accessibility_setting(int setting) {
    switch (setting) {
    case KIRA_UI_REDUCE_MOTION:
        return UIAccessibilityIsReduceMotionEnabled();
    case KIRA_UI_REDUCE_TRANSPARENCY:
        return UIAccessibilityIsReduceTransparencyEnabled();
    case KIRA_UI_INCREASE_CONTRAST:
        return UIAccessibilityDarkerSystemColorsEnabled();
    case KIRA_UI_DIFFERENTIATE_WITHOUT_COLOR:
        return UIAccessibilityShouldDifferentiateWithoutColor();
    case KIRA_UI_INVERT_COLORS:
        return UIAccessibilityIsInvertColorsEnabled();
    case KIRA_UI_SCREEN_READER:
        return UIAccessibilityIsVoiceOverRunning();
    default:
        return 0;
    }
}

#endif

#endif
