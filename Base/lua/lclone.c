/*
** $Id: lauxlib.c,v 1.248.1.1 2013/04/12 18:48:47 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/


#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* This file uses only the official API of Lua.
** Any function declared here could be written as an application function.
*/

#define lauxlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lclone.h"

// use clonefunction
struct codecache {
	volatile unsigned long lock;
	lua_State *L;
};

static struct codecache CC = { 0 , NULL };

#define LOCK(q) while ( ::InterlockedCompareExchange(&((q)->lock),1,0)!=0 ){};
#define UNLOCK(q)  (q)->lock=0;


static void
clearcache() {
	if (CC.L == NULL)
		return;
	LOCK(&CC)
		lua_close(CC.L);
		CC.L = luaL_newstate();
	UNLOCK(&CC)
}

static void
init() {
	CC.lock = 0;
	CC.L = luaL_newstate();
}

static const void *
load(const char *key) {
  if (CC.L == NULL)
    return NULL;
  LOCK(&CC)
    lua_State *L = CC.L;
    lua_pushstring(L, key);
    lua_rawget(L, LUA_REGISTRYINDEX);
    const void * result = lua_touserdata(L, -1);
    lua_pop(L, 1);
  UNLOCK(&CC)

  return result;
}

static const void *
save(const char *key, const void * proto) {
  lua_State *L;
  const void * result = NULL;

  LOCK(&CC)
    if (CC.L == NULL) {
      init();
      L = CC.L;
    } else {
      L = CC.L;
      lua_pushstring(L, key);
      lua_pushvalue(L, -1);
      lua_rawget(L, LUA_REGISTRYINDEX);
      result = lua_touserdata(L, -1); /* stack: key oldvalue */
      if (result == NULL) {
        lua_pop(L,1);
        lua_pushlightuserdata(L, (void *)proto);
        lua_rawset(L, LUA_REGISTRYINDEX);
      } else {
        lua_pop(L,2);
      }
    }
  UNLOCK(&CC)
  return result;
}

LUALIB_API int luaL_loadfilex (lua_State *L, const char *filename,
                                             const char *mode) {
  const void * proto = load(filename);
  if (proto) {
    lua_clonefunction(L, proto);
    return LUA_OK;
  }
  lua_State * eL = luaL_newstate();
  if (eL == NULL) {
    lua_pushliteral(L, "New state failed");
    return LUA_ERRMEM;
  }
  int err = luaL_loadfilex_(eL, filename, mode);
  if (err != LUA_OK) {
    size_t sz = 0;
    const char * msg = lua_tolstring(eL, -1, &sz);
    lua_pushlstring(L, msg, sz);
    lua_close(eL);
    return err;
  }
  proto = lua_topointer(eL, -1);
  const void * oldv = save(filename, proto);
  if (oldv) {
    lua_close(eL);
    lua_clonefunction(L, oldv);
  } else {
    lua_clonefunction(L, proto);
    /* Never close it. notice: memory leak */
  }
  return LUA_OK;
}

static int
cache_clear(lua_State *L) {
	clearcache();
	return 0;
}

LUAMOD_API int luaopen_cache(lua_State *L) {
	luaL_Reg l[] = {
		{ "clear", cache_clear },
		{ NULL, NULL },
	};
	luaL_newlib(L,l);
  lua_getglobal(L, "loadfile");
  lua_setfield(L, -2, "loadfile");
	return 1;
}
