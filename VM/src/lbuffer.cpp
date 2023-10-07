// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#include "lbuffer.h"

#include "lgc.h"
#include "lmem.h"
#include "lstring.h"

#include <string.h>

// Limit similar to the one we have for strings
#define MAX_BUFFER_SIZE MAXSSIZE

Buffer* luaB_newbuffer(lua_State* L, size_t s)
{
    if (s > MAX_BUFFER_SIZE)
        luaM_toobig(L);

    Buffer* b = luaM_newgco(L, Buffer, sizebuffer(s), L->activememcat);
    luaC_init(L, b, LUA_TBUFFER);
    b->len = int(s);
    return b;
}

void luaB_freebuffer(lua_State* L, Buffer* b, lua_Page* page)
{
    luaM_freegco(L, b, sizebuffer(b->len), b->memcat, page);
}
