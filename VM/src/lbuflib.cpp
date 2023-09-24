// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "lualib.h"

#include "lcommon.h"
#include "lmem.h"
#include "lstring.h"
#include "ludata.h"

#include <string>

// TODO: luaL_checkunsigned overflow check is not compatible with NUM_TO_INDEX and this 64bit cast is pretty awful
// Limit similar to the one we have for strings
#define MAX_BUF_SIZE MAXSSIZE

static int buf_create(lua_State* L)
{
    double dsize = luaL_checknumber(L, 1);
    int size = int(dsize);

    if (double(size) != dsize || size < 0)
        luaL_error(L, "invalid buffer size");

    if (size > MAX_BUF_SIZE)
        luaM_toobig(L);

    void* buf = lua_newuserdatatagged(L, size, UTAG_BUF);
    memset(buf, 0, size);

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
        luaL_error(L, "attempt to read i8 past the length of a buffer");

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
        luaL_error(L, "attempt to write i8 past the length of a buffer");

    ((int8_t*)buf)[offset] = int8_t(value);
    return 0;
}

static int buf_readi16(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);

    int16_t val;

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to read i16 past the length of a buffer");

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buf_writei16(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);
    int value = luaL_checkinteger(L, 3);

    int16_t val = int16_t(value);

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to write i16 past the length of a buffer");

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buf_readi32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);

    int32_t val;

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to read i32 past the length of a buffer");

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buf_writei32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);
    int value = luaL_checkinteger(L, 3);

    int32_t val = int32_t(value);

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to write i32 past the length of a buffer");

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buf_readf32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);

    float val;

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to read f32 past the length of a buffer");

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buf_writef32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);
    double value = luaL_checknumber(L, 3);

    float val = float(value);

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to write f32 past the length of a buffer");

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buf_readf64(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);

    double val;

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to read f64 past the length of a buffer");

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buf_writef64(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);
    double val = luaL_checknumber(L, 3);

    if(uint64_t(offset) + sizeof(val) > uint64_t(len))
        luaL_error(L, "attempt to write f64 past the length of a buffer");

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buf_readstring(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);
    unsigned size = luaL_checkunsigned(L, 3);

    if(uint64_t(offset) + size > uint64_t(len))
        luaL_error(L, "attempt to read string past the length of a buffer");

    lua_pushlstring(L, (char*)buf + offset, size);
    return 1;
}

static int buf_writestring(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    unsigned offset = luaL_checkunsigned(L, 2);
    size_t stringlen = 0;
    const char* val = luaL_checklstring(L, 3, &stringlen);

    if(uint64_t(offset) + stringlen > uint64_t(len))
        luaL_error(L, "attempt to write string past the length of a buffer");

    memcpy((char*)buf + offset, val, stringlen);
    return 0;
}

static const luaL_Reg buflib[] = {
    {"create", buf_create},
    {"readi8", buf_readi8},
    {"writei8", buf_writei8},
    {"readi16", buf_readi16},
    {"writei16", buf_writei16},
    {"readi32", buf_readi32},
    {"writei32", buf_writei32},
    {"readf32", buf_readf32},
    {"writef32", buf_writef32},
    {"readf64", buf_readf64},
    {"writef64", buf_writef64},
    {"readstring", buf_readstring},
    {"writestring", buf_writestring},
    // TODO: unsigned integers
    {NULL, NULL},
};

int luaopen_buf(lua_State* L)
{
    luaL_register(L, LUA_BUFLIBNAME, buflib);

    return 1;
}