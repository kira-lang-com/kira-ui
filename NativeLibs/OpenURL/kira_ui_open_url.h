#ifndef KIRA_UI_OPEN_URL_H
#define KIRA_UI_OPEN_URL_H

/* Hands `url` to the platform, which opens it in the application it opens such
 * URLs with: a web page in the browser, a `mailto:` in the mail client.
 *
 * 1 = handed over, 0 = the platform could not take it. Returns at once: the
 * application opens on its own time, not the caller's. */
int kira_ui_open_url(const char *url);

#endif
