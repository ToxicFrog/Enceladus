#ifndef ENCELADUS_TOC_H
#define ENCELADUS_TOC_H

int readTOC(lua_State *, const char *);
int writeTOC(lua_State *, const char *, size_t, char **);

#endif