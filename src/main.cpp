#include "lni_parser.h"
#include "lni_handler.h"

namespace lni {
    static void copy(lua_State* L, int t, int from, int to, bool clone) {
        lua_rawgeti(L, t, from);
        if (clone) {
            if (lua_type(L, -1) != LUA_TTABLE) {
                lua_pop(L, 1);
                lua_newtable(L);
                lua_pushvalue(L, -1);
                lua_rawseti(L, t, from);
            }
            lua_newtable(L);
            copytable(L, -2, -1);
            lua_rawseti(L, t, from);
        }
        lua_rawseti(L, t, to);
    }

    static int parse(lua_State* L) {
        bool clean = false;
        lex l(luaL_checkstring(L, 1));
        const char* file = luaL_optstring(L, 2, "...");
        switch (lua_gettop(L)) {
        case 1:
            lua_pushstring(L, file);
        case 2:
            lua_newtable(L); // env
            lua_newtable(L); lua_rawseti(L, -2, 1); // root
            lua_newtable(L); lua_rawseti(L, -2, 2); // default
            lua_rawgeti(L, 3, 1);
            lua_rawgeti(L, 3, 2);
            break;
        default:
            lua_settop(L, 3);
        case 3:
            // env
            luaL_checktype(L, 3, LUA_TTABLE);
            // root
            if (lua_rawgeti(L, 3, 1) != LUA_TTABLE) {
                lua_pop(L, 1);
                lua_newtable(L);
                lua_pushvalue(L, -1);
                lua_rawseti(L, 3, 1);
            }
            // default
            copy(L, 3, 2, 3, true);
            lua_rawgeti(L, 3, 2);
            clean = true;
            break;
        }
        handler h(L);
        bool ok = l.parse(h);
        if (clean) {
            copy(L, 3, 3, 2, false);
        }
        if (!ok) {
            return luaL_error(L, "\n%s:%d: %s", file, (int)lua_tointeger(L, -2), lua_tostring(L, -1));
        }
        return 2;
    }
}

extern "C" __declspec(dllexport)
int luaopen_lni(lua_State* L) {
    lua_pushcfunction(L, lni::parse);
    return 1;
}
