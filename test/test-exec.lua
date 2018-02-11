package.cpath = package.cpath .. [[;.\..\bin\Debug\?.dll]]

local lni = require 'lni-c'
local env = setmetatable({}, {__index = _G})

local n = 0
local function TEST(script)
	n = n + 1
	local name = 'TEST-' .. n
	local r = lni(script, name, env)
end

TEST [[
--! print 'ok'
]]

function env.test()
	local main = env[1]
	print(main.A.b)
end

TEST [[
[A]
b = 2
--! test()
]]
