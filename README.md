[![Build status](https://ci.appveyor.com/api/projects/status/jf07wn469ghns1ie?svg=true)](https://ci.appveyor.com/project/actboy168/lni)

# lni

一种类似于ini的标记语言。语法上参考了ini和lua，目标是作为lua程序的配置语言。


## build && test

```
3rd\luamake\luamake.exe
build\bin\lua.exe -e "package.path=[[test\?.lua]]"  test\test.lua
```

## 语法

```lua
[ABC]
a = 'Hello'
b = 1.0
c = {'1', '2', '3'}
'd' = {
  x = 2,
  y = 4,
}
10 =
[[ | H
   | e
   | l
   | l
   | o
]]
```

等价的lua为

```lua
{
ABC = {
a = 'Hello',
b = 1.0,
c = {'1', '2', '3'},
d = { x = 2, y = 4 },
[10] =
[[ | H
   | e
   | l
   | l
   | o
]]
}
}
```

## default

default是内置的对象，每个对象初始化时会复制一遍它的值。

```lua
<default>
a = 1
b = 2
[A]
b = 11
[B]
a = 10
```

等价的lua为

```lua
{
  A = { a = 1, b = 11 },
  B = { a = 10, b = 2 },
}
```

## enum

enum是内置对象，其他对象可以直接访问到它的元素。

```lua
<enum>
YES = 1
NO = 0
[A]
a = YES
b = NO
```

等价的lua为

```lua
{
 A = { a = 1, b = 0 }
}
```

## 继承

对象可以继承一个已经定义的对象

```lua
[A]
a = 1
b = 2
[B:A]
c = 3
```

等价的lua为

```lua
{
  A = { a = 1, b = 2 },
  B = { a = 1, b = 2, c = 3 },
}
```
