// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "lualib.h"

#include "lcommon.h"
#include "lmem.h"
#include "lstring.h"
#include "ludata.h"

#include <string>

// TODO: error messages mention the type, making the reuse of iN for uN have the wrong message

// Limit similar to the one we have for strings
#define MAX_BUFFER_SIZE MAXSSIZE

static const char* kinvalidsize = "invalid buffer size";
static const char* koutofbounds = "access out of bounds of the buffer";
static const char* knegativesize = "size cannot be negative";

#define isoutofbounds(offset, len, accessize) ((len) < (accessize) || unsigned(offset) > (len) - (accessize))

static int buffer_create(lua_State* L)
{
    double dsize = luaL_checknumber(L, 1);
    int size = int(dsize);

    if (double(size) != dsize || size < 0)
        luaL_error(L, kinvalidsize);

    if (size > MAX_BUFFER_SIZE)
        luaM_toobig(L);

    void* buf = lua_newuserdatatagged(L, size, UTAG_BUF);
    memset(buf, 0, size);

    return 1;
}

static int buffer_readi8(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    if (unsigned(offset) >= len)
        luaL_error(L, koutofbounds);

    int8_t val = ((int8_t*)buf)[offset];
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_readu8(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    if (unsigned(offset) >= len)
        luaL_error(L, koutofbounds);

    uint8_t val = ((uint8_t*)buf)[offset];
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writei8(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int value = luaL_checkinteger(L, 3);

    if (unsigned(offset) >= len)
        luaL_error(L, koutofbounds);

    ((int8_t*)buf)[offset] = int8_t(value);
    return 0;
}

static int buffer_readi16(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    int16_t val;

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_readu16(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    uint16_t val;

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writei16(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int value = luaL_checkinteger(L, 3);

    int16_t val = int16_t(value);

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readi32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    int32_t val;

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_readu32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    uint32_t val;

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writei32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int val = luaL_checkinteger(L, 3);

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buffer_writeu32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    unsigned val = luaL_checkunsigned(L, 3);

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readf32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    float val;

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writef32(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    double value = luaL_checknumber(L, 3);

    float val = float(value);

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readf64(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);

    double val;

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy(&val, (char*)buf + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writef64(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    double val = luaL_checknumber(L, 3);

    if (isoutofbounds(offset, len, sizeof(val)))
        luaL_error(L, koutofbounds);

    memcpy((char*)buf + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readstring(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int size = luaL_checkinteger(L, 3);

    if (size < 0)
        luaL_error(L, knegativesize);

    if (isoutofbounds(offset, len, unsigned(size)))
        luaL_error(L, koutofbounds);

    lua_pushlstring(L, (char*)buf + offset, size);
    return 1;
}

static int buffer_writestring(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    unsigned len = lua_objlen(L, 1);
    int offset = luaL_checkinteger(L, 2);
    size_t size = 0;
    const char* val = luaL_checklstring(L, 3, &size);

    if (isoutofbounds(offset, len, unsigned(size)))
        luaL_error(L, koutofbounds);

    memcpy((char*)buf + offset, val, size);
    return 0;
}

static int buffer_len(lua_State* L)
{
    void* buf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!buf)
        luaL_typeerrorL(L, 1, "buffer");

    lua_pushnumber(L, double(lua_objlen(L, 1)));
    return 1;
}

static int buffer_copy(lua_State* L)
{
    void* sbuf = lua_touserdatatagged(L, 1, UTAG_BUF);
    if (!sbuf)
        luaL_typeerrorL(L, 1, "buffer");
    unsigned slen = lua_objlen(L, 1);

    int soffset = luaL_checkinteger(L, 2);
    int size = luaL_checkinteger(L, 3);
    int toffset = luaL_checkinteger(L, 4);
    int tu = !lua_isnoneornil(L, 5) ? 5 : 1; // destination userdata

    void* tbuf = lua_touserdatatagged(L, tu, UTAG_BUF);
    if (!tbuf)
        luaL_typeerrorL(L, tu, "buffer");
    unsigned tlen = lua_objlen(L, tu);

    if (size < 0)
        luaL_error(L, knegativesize);

    if (isoutofbounds(soffset, slen, unsigned(size)))
        luaL_error(L, koutofbounds);

    if (isoutofbounds(toffset, tlen, unsigned(size)))
        luaL_error(L, koutofbounds);

    memcpy((char*)tbuf + toffset, (char*)sbuf + soffset, size);
    return 0;
}

static const luaL_Reg bufferlib[] = {
    {"create", buffer_create},
    {"readi8", buffer_readi8},
    {"readu8", buffer_readu8},
    {"writei8", buffer_writei8},
    {"writeu8", buffer_writei8}, // reuse the i8 function as it has the same result
    {"readi16", buffer_readi16},
    {"readu16", buffer_readu16},
    {"writei16", buffer_writei16},
    {"writeu16", buffer_writei16}, // reuse the i16 function as it has the same result
    {"readi32", buffer_readi32},
    {"readu32", buffer_readu32},
    {"writei32", buffer_writei32},
    {"writeu32", buffer_writeu32}, // can't reuse writei32, because double -> unsigned conversion is required
    {"readf32", buffer_readf32},
    {"writef32", buffer_writef32},
    {"readf64", buffer_readf64},
    {"writef64", buffer_writef64},
    {"readstring", buffer_readstring},
    {"writestring", buffer_writestring},
    {"len", buffer_len},
    {"copy", buffer_copy},
    {NULL, NULL},
};

int luaopen_buffer(lua_State* L)
{
    luaL_register(L, LUA_BUFFERLIBNAME, bufferlib);

    return 1;
}