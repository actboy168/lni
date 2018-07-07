#include <lua.hpp> 

namespace lni
{
	int classics(lua_State* L);
	int no_convert(lua_State* L);
}

extern "C" __declspec(dllexport)
int luaopen_lni(lua_State* L)
{
	static luaL_Reg l[] = {
		{ "classics", lni::classics },
		{ "no_convert", lni::no_convert },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
