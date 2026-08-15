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

#endif
