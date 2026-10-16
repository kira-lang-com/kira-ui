#ifndef KIRA_UI_APPEARANCE_H
#define KIRA_UI_APPEARANCE_H

/* The colour scheme the platform is currently presenting.
 *
 * 0 = the platform states no preference, 1 = light, 2 = dark.
 *
 * Cheap enough to call once per frame on every platform it answers on, which is
 * what lets a running application follow the scheme changing under it rather
 * than only reading it at launch. */
int kira_ui_platform_appearance(void);

/* Whether one of the platform's accessibility settings is on: 1 when it is,
 * 0 when it is off or the platform keeps no such setting.
 *
 * 1 = reduce motion, 2 = reduce transparency, 3 = increase contrast,
 * 4 = differentiate without color, 5 = invert colors, 6 = a screen reader
 * (VoiceOver, Narrator) is running.
 *
 * Asked once a rebuild for each, so a running application follows a setting
 * changing under it. */
int kira_ui_accessibility_setting(int setting);

#endif
