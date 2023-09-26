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

static int buffer_pushi8(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int value = luaL_checkinteger(L, 2);

    if (buffer->pos + 1 > buffer->len)
        luaL_error(L, "buffer is full");

    ((int8_t*)buffer->data)[buffer->pos] = int8_t(value);
    buffer->pos += 1;
    return 0;
}

static int buffer_pushi16(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int value = luaL_checkinteger(L, 2);

    int16_t val = int16_t(value);

    if(buffer->pos + sizeof(val) > buffer->len)
        luaL_error(L, "buffer is full");

    memcpy((char*)buffer->data + buffer->pos, &val, sizeof(val));
    buffer->pos += sizeof(val);
    return 0;
}

static int buffer_pushi32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int32_t val = luaL_checkinteger(L, 2);

    if(buffer->pos + sizeof(val) > buffer->len)
        luaL_error(L, "buffer is full");

    memcpy((char*)buffer->data + buffer->pos, &val, sizeof(val));
    buffer->pos += sizeof(val);
    return 0;
}

static int buffer_pushu32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    uint32_t val = luaL_checkunsigned(L, 2);

    if(buffer->pos + sizeof(val) > buffer->len)
        luaL_error(L, "buffer is full");

    memcpy((char*)buffer->data + buffer->pos, &val, sizeof(val));
    buffer->pos += sizeof(val);
    return 0;
}

static int buffer_pushf32(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    float val = float(luaL_checknumber(L, 2));

    if(buffer->pos + sizeof(val) > buffer->len)
        luaL_error(L, "buffer is full");

    memcpy((char*)buffer->data + buffer->pos, &val, sizeof(val));
    buffer->pos += sizeof(val);
    return 0;
}

static int buffer_pushf64(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    double val = luaL_checknumber(L, 2);

    if(buffer->pos + sizeof(val) > buffer->len)
        luaL_error(L, "buffer is full");

    memcpy((char*)buffer->data + buffer->pos, &val, sizeof(val));
    buffer->pos += sizeof(val);
    return 0;
}

static int buffer_pushstring(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    size_t size = 0;
    const char* val = luaL_checklstring(L, 2, &size);

    if(buffer->pos + size > buffer->len)
        luaL_error(L, "buffer is full");

    memcpy((char*)buffer->data + buffer->pos, val, size);
    buffer->pos += unsigned(size);
    return 0;
}

static int buffer_skip(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);
    int val = luaL_checkinteger(L, 2);

    if (val < 0)
        luaL_error(L, "can't skip back"); // TODO: maybe?

    // TODO: recheck this bound check
    if(buffer->pos + unsigned(val) > buffer->len)
        luaL_error(L, "buffer is full");

    buffer->pos += unsigned(val);
    return 0;
}

static int buffer_pos(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);

    lua_pushnumber(L, double(buffer->pos));
    return 1;
}

static int buffer_rewind(lua_State* L)
{
    LuauBuffer* buffer = getbuffer(L, 1);

    buffer->pos = 0;
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
    {"pushi8", buffer_pushi8},
    {"pushu8", buffer_pushi8}, // reuse the i16 function as it has the same result
    {"pushi16", buffer_pushi16},
    {"pushu16", buffer_pushi16}, // reuse the i16 function as it has the same result
    {"pushi32", buffer_pushi32},
    {"pushu32", buffer_pushu32}, // can't reuse pushi32, because double -> unsigned conversion is required
    {"pushf32", buffer_pushf32},
    {"pushf64", buffer_pushf64},
    {"pushstring", buffer_pushstring},
    {"skip", buffer_skip},
    {"pos", buffer_pos},
    {"rewind", buffer_rewind},
    {NULL, NULL},
};

int luaopen_buffer(lua_State* L)
{
    luaL_register(L, LUA_BUFFERLIBNAME, bufferlib);

    return 1;
}