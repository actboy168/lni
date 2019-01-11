#pragma once

#include <lua.hpp>

namespace lni {
	inline void lua_copytable(lua_State* L, int src, int dst) {
		src = lua_absindex(L, src);
		dst = lua_absindex(L, dst);
		lua_pushnil(L);
		while (lua_next(L, src)) {
			lua_pushvalue(L, -2);
			lua_insert(L, -2);
			lua_rawset(L, dst);
		}
	}

	inline void lua_cleartable(lua_State* L, int t) {
		t = lua_absindex(L, t);
		lua_pushnil(L);
		while (lua_next(L, t)) {
			lua_pop(L, 1);
			lua_pushvalue(L, -1);
			lua_pushnil(L);
			lua_rawset(L, t);
		}
	}

	struct handler {
		lua_State* L;
		int t_root = 0;
		int t_default = 0;

		handler(lua_State* L)
			: L(L)
		{}

		void accept_boolean(bool b) {
			lua_pushboolean(L, b ? 1 : 0);
		}
		void accept_nil() {
			lua_pushnil(L);
		}
		void accept_integer(int64_t i) {
			lua_pushinteger(L, i);
		}
		void accept_double(double d) {
			lua_pushnumber(L, d);
		}
		size_t accept_number(const char* str, size_t len) {
			if (len > 200) {
				return 0;
			}
			char tmp[200 + 1];
			memcpy(tmp, str, len);
			tmp[len] = 0;
			return lua_stringtonumber(L, tmp);
		}
		void accept_string(const char* str, size_t len) {
			lua_pushlstring(L, str, len);
		}
		void accept_table(int& ary) {
			lua_newtable(L);
			ary = 1;
		}
		void accept_table_hash() {
			lua_rawset(L, -3);
		}
		void accept_table_array(int& ary) {
			lua_rawseti(L, -2, ary++);
		}
		void accept_set() {
			lua_settable(L, -3);
		}
		void accept_object() {
			lua_pop(L, 1);
		}
		template <class ... Args>
		void accept_error(int line, const char* fmt, const Args& ... args) {
			lua_pushinteger(L, line);
			lua_pushfstring(L, fmt, args...);
		}
		void accept_root() {
			// env
			// root
			// default
			t_default = lua_gettop(L);
			t_root = t_default - 1;
			lua_pushvalue(L, t_root);
		}
		void accept_root_end() {
			lua_pop(L, 1);
		}
        bool accept_internal_section() {
            lua_remove(L, -2);
            const char* name = luaL_checkstring(L, -1);
            if (0 == strcmp(name, "default")) {
                lua_pop(L, 1);
                lua_pushvalue(L, t_default);
                lua_pushvalue(L, -1);
                return true;
            }
            if (0 == strcmp(name, "root")) {
                lua_pop(L, 1);
                lua_pushvalue(L, t_root);
                lua_pushvalue(L, -1);
                return true;
            }
            return false;
        }
		void accept_section(bool has_root) {
			if (!has_root) {
				lua_pushvalue(L, -1);
				return;
			}
			lua_remove(L, -2);
			lua_pushvalue(L, -1);
			if (lua_gettable(L, t_root) != LUA_TTABLE) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_insert(L, -3);
				lua_settable(L, t_root);
			}
			else {
				lua_remove(L, -2);
			}
			lua_pushvalue(L, -1);
		}
		void accept_section_child() {
			lua_pushvalue(L, -1);
			if (lua_gettable(L, -3) != LUA_TTABLE) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_insert(L, -3);
				lua_settable(L, -4);
			}
			else {
				lua_remove(L, -2);
			}
			lua_remove(L, -2);
		}
		void accept_section_inherited() {
			lua_pushvalue(L, -1);
			if (LUA_TTABLE != lua_gettable(L, t_root)) {
				lua_pop(L, 1);
				lua_pushvalue(L, -1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_insert(L, -3);
				lua_settable(L, t_root);
			}
			lua_remove(L, -2);
		}
		void accept_section_end(bool inherited, bool top) {
			if (inherited) {
				lua_cleartable(L, -2);
				lua_copytable(L, -1, -2);
				lua_pop(L, 1);
				return;
			}
			else {
				lua_cleartable(L, -1);
				if (top) {
					lua_copytable(L, t_default, -1);
				}
			}
		}
	};
}
