local lm = require 'luamake'

lm.rootdir = '3rd/lua53/src'
lm:shared_library 'lua53' {
   sources = {
      "*.c",
      "!lua.c",
      "!luac.c",
      "!testes/*.c",
   },
   defines = "LUA_BUILD_AS_DLL"
}
lm:executable 'lua' {
   deps = "lua53",
   sources = "lua.c"
}

lm.rootdir = 'src'
lm:lua_library 'lni' {
   luaversion = 'lua53',
   sources = "*.cpp"
}

lm:build "test" {
   "$luamake", "lua", "test.lua",
   deps = { "lni", "lua" }
}
