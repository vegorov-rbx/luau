// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "lualib.h"

#include "lcommon.h"
#include "lmem.h"
#include "ludata.h"

#include <string>

// TODO: luaL_checkunsigned overflow check is not compatible with NUM_TO_INDEX and this 64bit cast is pretty awful
// TODO: what's a good limit?
#define MAX_BUF_SIZE (1 << 20)

static int buf_create(lua_State* L)
{
    unsigned v = luaL_checkunsigned(L, 1);

    if (v > MAX_BUF_SIZE)
        luaM_toobig(L);

    void* buf = lua_newuserdatatagged(L, v, UTAG_BUF);
    memset(buf, 0, v);

    return 1;
}

static int buf_readi8(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);

    if (uint64_t(offset) + 1 > uint64_t(len))
        luaL_error(L, "attempt to read i8 from the buffer past the length of %u", len);

    int8_t val = ((int8_t*)buf)[offset];
    lua_pushnumber(L, double(val));
    return 1;
}

static int buf_writei8(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);
    int value = luaL_checkinteger(L, 3);

    if(uint64_t(offset) + 1 > uint64_t(len))
        luaL_error(L, "attempt to write i8 from the buffer past the length of %u", len);

    ((int8_t*)buf)[offset] = int8_t(value);
    return 0;
}

static const luaL_Reg buflib[] = {
    {"create", buf_create},
    {"readi8", buf_readi8},
    {"writei8", buf_writei8},
    /*{"readu8", buf_create},
    {"readi16", buf_create},
    {"readu16", buf_create},
    {"readi32", buf_create},
    {"readu32", buf_create},
    {"readf32", buf_create},
    {"readf64", buf_create},*/
    {NULL, NULL},
};

int luaopen_buf(lua_State* L)
{
    luaL_register(L, LUA_BUFLIBNAME, buflib);

    return 1;
}