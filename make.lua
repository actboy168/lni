local lm = require 'luamake'

lm.rootdir = 'src'
lm:lua_library 'lni' {
   sources = "*.cpp"
}

lm:build "test" {
   "$luamake", "lua", "test.lua",
   deps = { "lni" }
}
