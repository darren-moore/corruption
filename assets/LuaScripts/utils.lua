-- metatable for Vec2
local vectorMT = {}
function MakeVec2(x,y)
    local v = {x=x, y=y}
    setmetatable(v, vectorMT)
    return v
end
function vectorMT.__add(a, b)
    return MakeVec2(a.x+b.x, a.y+b.y)
end
function vectorMT.__sub(a, b)
    return MakeVec2(a.x-b.x, a.y-b.y)
end
function vectorMT.__mul(a, b)
    if type(a) == "number" then
        return MakeVec2(b.x * a, b.y * a)
    elseif type(b) == "number" then
        return MakeVec2(a.x * b, a.y * b)
    end
    return a.x * b.x + a.y * b.y
end
function vectorMT.__eq(a, b)
    return a.x == b.x and a.y == b.y
end
function vectorMT.__tostring(a)
    return "(" .. a.x .. ", " .. a.y .. ")"
end

-- LuaList implementation. A two-sided queue
LuaList = {}
function LuaList.new ()
    return {first = 0, last = -1}
end
function LuaList.pushtop (list, value)
    local first = list.first - 1
    list.first = first
    list[first] = value
end
function LuaList.pushbottom (list, value)
    local last = list.last + 1
    list.last = last
    list[last] = value
end
function LuaList.poptop (list)
    local first = list.first
    if first > list.last then error("list is empty") end
    local value = list[first]
    list[first] = nil        -- to allow garbage collection
    list.first = first + 1
    return value
end
function LuaList.popbottom (list)
    local last = list.last
    if list.first > last then error("list is empty") end
    local value = list[last]
    list[last] = nil         -- to allow garbage collection
    list.last = last - 1
    return value
end
function LuaList.peektop (list)
    local first = list.first
    if first > list.last then error("list is empty") end
    local value = list[first]
    return value
end
function LuaList.peekbottom (list)
    local last = list.last
    if list.first > last then error("list is empty") end
    local value = list[last]
    return value
end
function LuaList.empty (list)
    return list.first > list.last
end