#pragma once

#include <lua.hpp>
#include <stack>
#include <limits>

namespace lni {
    inline void copytable(lua_State* L, int src, int dst) {
        src = lua_absindex(L, src);
        dst = lua_absindex(L, dst);
        lua_pushnil(L);
        while (lua_next(L, src)) {
            lua_pushvalue(L, -2);
            lua_insert(L, -2);
            lua_rawset(L, dst);
        }
    }

    struct handler {
        lua_State* L;
        int t_root = 0;
        int t_default = 0;
        bool split = false;
        std::stack<unsigned int> levels;

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
            levels.push(0);
        }
        void accept_root_end() {
            accept_split_end();
            for (;!levels.empty();) {
                lua_pop(L, 1);
                levels.pop();
            }
        }
        bool accept_internal_section() {
            const char* name = luaL_checkstring(L, -1);
            if (0 == strcmp(name, "default")) {
                lua_pop(L, 1);
                lua_pushvalue(L, t_default);
                lua_pushvalue(L, -1);
                levels.push(std::numeric_limits<unsigned int>::max());
                return true;
            }
            return false;
        }
        void accept_split_end() {
            if (split) {
                lua_pop(L, 1);
                split = false;
            }
        }
        void accept_section_sub() {
            lua_pushvalue(L, -1);
        }
        bool accept_section(unsigned int level, unsigned int& last) {
            for (;;) {
                if (level <= levels.top()) {
                    lua_remove(L, -2);
                    levels.pop();
                    continue;
                }
                if (level > levels.top() + 1) {
                    last = levels.top();
                    return false;
                }
                levels.push(level);
                break;
            }
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_insert(L, -3);
            lua_settable(L, -4);
            lua_pushvalue(L, -1);
            return true;
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
                lua_newtable(L);
                lua_pushvalue(L, -1);
                lua_insert(L, -3);
                lua_settable(L, t_root);
            }
            else {
                lua_remove(L, -2);
            }
        }
        void accept_section_end(bool inherited, bool top) {
            if (inherited) {
                copytable(L, -1, -2);
                lua_pop(L, 1);
            }
            else if (top) {
                copytable(L, t_default, -1);
            }
        }
        void accept_split() {
            if (split) {
                lua_pop(L, 1);
            }
            split = true;
            lua_Integer n = luaL_len(L, -1);
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_seti(L, -3, n + 1);
        }
    };
}
