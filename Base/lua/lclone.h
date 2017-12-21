/*
** $Id: lauxlib.h,v 1.120.1.1 2013/04/12 18:48:47 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/


#ifndef lclone_h
#define lclone_h


#include <stddef.h>
#include <stdio.h>

#include "lua.h"


LUALIB_API int luaL_loadfilex (lua_State *L, const char *filename,
	const char *mode);

LUAMOD_API int luaopen_cache(lua_State *L);
LUAMOD_API int luaL_loadfilex_ (lua_State *L, const char *filename,
	const char *mode);

#endif


