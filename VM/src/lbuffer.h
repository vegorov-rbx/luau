// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "lobject.h"

#define sizebuffer(len) (offsetof(Buffer, data) + (len < 8 ? 8 : len))

LUAI_FUNC Buffer* luaB_newbuffer(lua_State* L, size_t s);
LUAI_FUNC void luaB_freebuffer(lua_State* L, Buffer* u, struct lua_Page* page);
