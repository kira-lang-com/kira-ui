#include "kira_ui_open_url.h"

/* The platforms open a URL each through their own interface: the shell on
 * Windows, Launch Services on macOS, UIApplication on iOS, the page on the web,
 * and on the free desktops `xdg-open`, which asks the desktop the way its own
 * applications do. */

#if defined(_WIN32)

#include <windows.h>
#include <shellapi.h>

int kira_ui_open_url(const char *url) {
    int length = MultiByteToWideChar(CP_UTF8, 0, url, -1, NULL, 0);
    if (length <= 0) {
        return 0;
    }
    wchar_t *wide = (wchar_t *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)length * sizeof(wchar_t));
    if (wide == NULL) {
        return 0;
    }
    MultiByteToWideChar(CP_UTF8, 0, url, -1, wide, length);
    /* ShellExecute answers a value above 32 for success. */
    INT_PTR result = (INT_PTR)ShellExecuteW(NULL, L"open", wide, NULL, NULL, SW_SHOWNORMAL);
    HeapFree(GetProcessHeap(), 0, wide);
    return result > 32 ? 1 : 0;
}

#elif defined(__EMSCRIPTEN__)

#include <emscripten.h>

EM_JS(int, kira_ui_open_url_page, (const char *url), {
    return window.open(UTF8ToString(url), "_blank", "noopener") ? 1 : 0;
});

int kira_ui_open_url(const char *url) {
    return kira_ui_open_url_page(url);
}

#elif defined(__APPLE__)

#include <TargetConditionals.h>

#if TARGET_OS_OSX

#include <CoreServices/CoreServices.h>

int kira_ui_open_url(const char *url) {
    CFStringRef text = CFStringCreateWithCString(NULL, url, kCFStringEncodingUTF8);
    if (text == NULL) {
        return 0;
    }
    CFURLRef location = CFURLCreateWithString(NULL, text, NULL);
    CFRelease(text);
    if (location == NULL) {
        return 0;
    }
    OSStatus status = LSOpenCFURLRef(location, NULL);
    CFRelease(location);
    return status == noErr ? 1 : 0;
}

#else

#include <objc/message.h>
#include <objc/runtime.h>
#include <CoreFoundation/CoreFoundation.h>

/* `[UIApplication.sharedApplication openURL:url options:@{} completionHandler:nil]`,
 * spoken through the Objective-C runtime so this file stays C. */
int kira_ui_open_url(const char *url) {
    CFStringRef text = CFStringCreateWithCString(NULL, url, kCFStringEncodingUTF8);
    if (text == NULL) {
        return 0;
    }
    CFURLRef location = CFURLCreateWithString(NULL, text, NULL);
    CFRelease(text);
    if (location == NULL) {
        return 0;
    }
    id application = ((id (*)(Class, SEL))objc_msgSend)(
        objc_getClass("UIApplication"), sel_registerName("sharedApplication"));
    id options = ((id (*)(Class, SEL))objc_msgSend)(
        objc_getClass("NSDictionary"), sel_registerName("dictionary"));
    ((void (*)(id, SEL, id, id, id))objc_msgSend)(
        application, sel_registerName("openURL:options:completionHandler:"),
        (id)location, options, NULL);
    CFRelease(location);
    return application != NULL ? 1 : 0;
}

#endif

#else

#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/* `xdg-open` may wait on the application it starts, so it runs behind a shell
 * that leaves it in the background and returns: the shell is reaped here, and
 * `xdg-open` is the init process's to reap. The URL reaches it as the shell's
 * first argument, never as shell text. */
int kira_ui_open_url(const char *url) {
    char *const arguments[] = {
        "sh", "-c", "xdg-open \"$1\" >/dev/null 2>&1 &", "sh", (char *)url, NULL
    };
    pid_t shell = 0;
    if (posix_spawn(&shell, "/bin/sh", NULL, NULL, arguments, environ) != 0) {
        return 0;
    }
    int status = 0;
    if (waitpid(shell, &status, 0) != shell) {
        return 0;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 1 : 0;
}

#endif
