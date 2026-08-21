#include "kira_ui_launch.h"

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

void kira_ui_launch_set(const char *name, const char *value) {
    if (name == 0 || name[0] == 0) {
        return;
    }
#if defined(_WIN32)
    char assignment[1024];
    _snprintf_s(assignment, sizeof(assignment), _TRUNCATE, "%s=%s", name, value != 0 ? value : "");
    _putenv(assignment);
#else
    if (value == 0 || value[0] == 0) {
        unsetenv(name);
        return;
    }
    setenv(name, value, 1);
#endif
}

#if defined(_WIN32)

int32_t kira_ui_launch_run(const char *executable, const char *argument) {
    if (executable == 0) {
        return -1;
    }
    char command[2048];
    if (argument != 0 && argument[0] != 0) {
        _snprintf_s(command, sizeof(command), _TRUNCATE, "\"%s\" \"%s\"", executable, argument);
    } else {
        _snprintf_s(command, sizeof(command), _TRUNCATE, "\"%s\"", executable);
    }
    STARTUPINFOA startup;
    PROCESS_INFORMATION information;
    memset(&startup, 0, sizeof(startup));
    memset(&information, 0, sizeof(information));
    startup.cb = sizeof(startup);
    /* The child inherits this process's environment, which is where the harness
     * knobs were just set. */
    if (CreateProcessA(0, command, 0, 0, TRUE, 0, 0, 0, &startup, &information) == 0) {
        return -1;
    }
    WaitForSingleObject(information.hProcess, INFINITE);
    DWORD status = 0;
    GetExitCodeProcess(information.hProcess, &status);
    CloseHandle(information.hThread);
    CloseHandle(information.hProcess);
    return (int32_t)status;
}

#else

int32_t kira_ui_launch_run(const char *executable, const char *argument) {
    if (executable == 0) {
        return -1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        if (argument != 0 && argument[0] != 0) {
            execl(executable, executable, argument, (char *)0);
        } else {
            execl(executable, executable, (char *)0);
        }
        /* Only reached when exec failed, and there is nothing left to run here:
         * this is the child's address space, and returning would hand the caller
         * a second copy of the launcher. */
        _exit(127);
    }
    int status = 0;
    while (waitpid(pid, &status, 0) < 0) {
        /* A signal interrupted the wait; the child is still running and still
         * the thing being waited for. */
    }
    if (WIFEXITED(status)) {
        return (int32_t)WEXITSTATUS(status);
    }
    return -1;
}

#endif
