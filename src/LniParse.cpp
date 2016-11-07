#include "LniParse.h" 

namespace lni
{
	int parse(lua_State* L)
	{
		lex l(luaL_checkstring(L, 1));
		const char* file = luaL_optstring(L, 2, "...");
		switch (lua_gettop(L))
		{
		case 1:
			lua_pushstring(L, file);
		case 2:
			lua_newtable(L); // main
			lua_newtable(L); // default
			lua_newtable(L); // enum
			break;
		case 3:
			luaL_checktype(L, 3, LUA_TTABLE); // main
			lua_newtable(L); // default
			lua_newtable(L); // enum
			break;
		case 4:
			luaL_checktype(L, 3, LUA_TTABLE); // main
			lua_newtable(L); // default
			if (lua_type(L, 4) == LUA_TTABLE) {
				lua_copytable(L, 4, -1);
			}
			lua_replace(L, 4);
			lua_newtable(L); // enum
			break;
		default:
			lua_settop(L, 5);
		case 5:
			luaL_checktype(L, 3, LUA_TTABLE); // main
			lua_newtable(L); // default
			if (lua_type(L, 4) == LUA_TTABLE) {
				lua_copytable(L, 4, -1);
			}
			lua_replace(L, 4);
			lua_newtable(L); // enum
			if (lua_type(L, 5) == LUA_TTABLE) {
				lua_copytable(L, 5, -1);
			}
			lua_replace(L, 5);
			break;
		}
		handler h(L);
		if (!l.parse(h)) {
			return luaL_error(L, "\n%s:%d: %s", file, (int)lua_tointeger(L, -2), lua_tostring(L, -1));
		}
		return 3;
	}
}
