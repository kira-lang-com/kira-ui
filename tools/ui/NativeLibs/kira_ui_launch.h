#ifndef KIRA_UI_LAUNCH_H
#define KIRA_UI_LAUNCH_H

#include <stdint.h>

/* Running another program, so the command line can drive an application it was
 * not linked against.
 *
 * The alternative is for every application to grow its own capture command, and
 * then for all of them to drift: one spells the flag `--out`, the next `-o`, a
 * third only reads an environment variable, and reviewing two applications side
 * by side means remembering which is which.
 *
 * It does not have to be linked against them because it does not have to be.
 * KiraGraphics already resolves its harness from the environment whenever a run
 * was handed none -- `KIRA_GRAPHICS_CAPTURE_FRAME`, `KIRA_GRAPHICS_CAPTURE_AT`,
 * `KIRA_GRAPHICS_QUIT_AFTER_FRAMES` -- so every Kira UI application is already
 * capturable and none of them needed changing. This sets those variables and
 * starts the program; the application does what it always did. */

/* Set a variable in THIS process's environment, which a child inherits.
 *
 * Setting it here rather than building an environment block per platform: a
 * child inherits by default everywhere, and the launcher is a command that runs
 * one program and exits, so there is nothing this could leak into. */
void kira_ui_launch_set(const char *name, const char *value);

/* Run `executable`, passing `argument` when it is not empty, and WAIT.
 *
 * Waiting is the point: the caller wants the frame the program wrote, and a
 * launcher that returned first would report on a file that is not there yet.
 * Answers the program's exit status, or -1 when it could not be started. */
int32_t kira_ui_launch_run(const char *executable, const char *argument);

#endif
