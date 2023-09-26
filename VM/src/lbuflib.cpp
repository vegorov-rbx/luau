// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "lualib.h"

#include "lcommon.h"
#include "lmem.h"
#include "lstring.h"
#include "ludata.h"

#include <string.h>

// Limit similar to the one we have for strings
#define MAX_BUFFER_SIZE MAXSSIZE

#define isoutofbounds(offset, len, accessize) ((len) < (accessize) || unsigned(offset) > (len) - (accessize))

#define sizebuffer(size) (offsetof(LuauBuffer, data) + size)

LuauBuffer* getbuffer(lua_State* L, int idx)
{
    void* buf = lua_touserdatatagged(L, idx, UTAG_BUF);
    if(!buf)
        luaL_typeerrorL(L, idx, "buffer");

    return (LuauBuffer*)buf;
}

static int buffer_create(lua_State* L)
{
    double dsize = luaL_checknumber(L, 1);
    int size = int(dsize);

    if (double(size) != dsize || size < 0)
        luaL_error(L, "invalid buffer size");

    if (size > MAX_BUFFER_SIZE)
        luaM_toobig(L);

    void* buf = lua_newuserdatatagged(L, sizebuffer(size), UTAG_BUF);
    
    LuauBuffer* buffer = (LuauBuffer*)buf;
    buffer->pos = 0;
    buffer->len = size;
    memset(buffer->data, 0, size);

    return 1;
}

static int buffer_readi8(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    if (unsigned(offset) >= buffer->len)
        luaL_error(L, "access out of bounds of the buffer");

    int8_t val = ((int8_t*)buffer->data)[offset];
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_readu8(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    if (unsigned(offset) >= buffer->len)
        luaL_error(L, "access out of bounds of the buffer");

    uint8_t val = ((uint8_t*)buffer->data)[offset];
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writei8(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int value = luaL_checkinteger(L, 3);

    if (unsigned(offset) >= buffer->len)
        luaL_error(L, "access out of bounds of the buffer");

    ((int8_t*)buffer->data)[offset] = int8_t(value);
    return 0;
}

static int buffer_readi16(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    int16_t val;

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy(&val, (char*)buffer->data + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_readu16(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    uint16_t val;

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy(&val, (char*)buffer->data + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writei16(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int value = luaL_checkinteger(L, 3);

    int16_t val = int16_t(value);

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy((char*)buffer->data + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readi32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    int32_t val;

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy(&val, (char*)buffer->data + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_readu32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    uint32_t val;

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy(&val, (char*)buffer->data + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writei32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int val = luaL_checkinteger(L, 3);

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy((char*)buffer->data + offset, &val, sizeof(val));
    return 0;
}

static int buffer_writeu32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    unsigned val = luaL_checkunsigned(L, 3);

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy((char*)buffer->data + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readf32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    float val;

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy(&val, (char*)buffer->data + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writef32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    double value = luaL_checknumber(L, 3);

    float val = float(value);

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy((char*)buffer->data + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readf64(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);

    double val;

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy(&val, (char*)buffer->data + offset, sizeof(val));
    lua_pushnumber(L, double(val));
    return 1;
}

static int buffer_writef64(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    double val = luaL_checknumber(L, 3);

    if (isoutofbounds(offset, buffer->len, sizeof(val)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy((char*)buffer->data + offset, &val, sizeof(val));
    return 0;
}

static int buffer_readstring(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    int size = luaL_checkinteger(L, 3);

    if (size < 0)
        luaL_error(L, "size cannot be negative");

    if (isoutofbounds(offset, buffer->len, unsigned(size)))
        luaL_error(L, "access out of bounds of the buffer");

    lua_pushlstring(L, (char*)buffer->data + offset, size);
    return 1;
}

static int buffer_writestring(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int offset = luaL_checkinteger(L, 2);
    size_t size = 0;
    const char* val = luaL_checklstring(L, 3, &size);

    if (isoutofbounds(offset, buffer->len, unsigned(size)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy((char*)buffer->data + offset, val, size);
    return 0;
}

static int buffer_len(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);

    lua_pushnumber(L, double(buffer->len));
    return 1;
}

static int buffer_copy(lua_State* L)
{
    LuauBuffer* sbuffer = getbuffer(L, 1);
    int soffset = luaL_checkinteger(L, 2);
    int size = luaL_checkinteger(L, 3);
    int toffset = luaL_checkinteger(L, 4);
    int tu = !lua_isnoneornil(L, 5) ? 5 : 1; // destination userdata

    LuauBuffer* tbuffer = getbuffer(L, tu);

    if (size < 0)
        luaL_error(L, "size cannot be negative");

    if (isoutofbounds(soffset, sbuffer->len, unsigned(size)))
        luaL_error(L, "access out of bounds of the buffer");

    if (isoutofbounds(toffset, tbuffer->len, unsigned(size)))
        luaL_error(L, "access out of bounds of the buffer");

    memcpy((char*)tbuffer->data + toffset, (char*)sbuffer->data + soffset, size);
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