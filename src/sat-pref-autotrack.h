#ifndef SAT_PREF_AUTOTRACK_H
#define SAT_PREF_AUTOTRACK_H 1

GtkWidget      *sat_pref_autotrack_create(GKeyFile * cfg);
void            sat_pref_autotrack_cancel(GKeyFile * cfg);
void            sat_pref_autotrack_ok(GKeyFile * cfg);

#endif
