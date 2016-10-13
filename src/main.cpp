#include <lua.hpp> 

namespace lni
{
	int parse(lua_State* L);
}

extern "C" __declspec(dllexport)
int luaopen_lni(lua_State* L)
{
	luaL_checkversion(L);
	lua_pushcfunction(L, lni::parse);
	return 1;
}

namespace lni
{
	int parse(lua_State* L);

	static int open(lua_State* L)
	{
		lua_pushcfunction(L, lni::parse);
		return 1;
	}

	int init(lua_State* L)
	{
		luaL_checkversion(L);
		luaL_requiref(L, "lni-c", open, 0);
		lua_pop(L, 1);
		return 0;
	}
}
