#include <lua.hpp> 

namespace lni
{
	int parse(lua_State* L);
}

extern "C" __declspec(dllexport)
int luaopen_lni(lua_State* L)
{
	lua_pushcfunction(L, lni::parse);
	return 1;
}
