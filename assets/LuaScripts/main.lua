require "utils"
require "behaviours"
local machine = require('statemachine')

enemyQueue = LuaList.new()
enemies = {}
nilpat = {pattern = nil, delay = -1, freq = -1}
startTime = 0

backgroundData = {velocity = 75, acceleration = 0, accelerating = true}

function FSM(SB, EB)
    local fsm = machine.create({
        initial = 'sb',
        events = {
          { name = 'toEB',  from = 'sb',  to = 'eb' }}, 
        update_functions = {
          duringsb = SB, 
          duringeb = EB,
        }
    })

    return fsm
end

function BindOptions(f, options)
    return function(enemy, dt)
        return f(options, enemy, dt)
    end
end

function MakePatternData(pattern, delay, freq)
    return {pattern = pattern, delay = delay, freq = freq}
end

function AddEnemy(spawnTime, fsm, SB_length, spawnPosition, patternData, health)
    local enemy = {}
    enemy.spawnTime = spawnTime
    enemy.transitionTime = spawnTime + SB_length
    enemy.pos = spawnPosition
    enemy.patternData = patternData
    enemy.fireTime = patternData.delay
    enemy.fsm = fsm

    if health == nil then
        health = 5
    end
     enemy.health = health

    LuaList.pushtop(enemyQueue, enemy)
end

function KillEnemy(enemy)
    Game_DestroyCollisionAgent(enemy.collisionAgent)

    if (enemy.bulletCommand ~= nil) then
        Game_DestroyPattern(enemy.bulletCommand)
    end
end

function UpdateEnemy(enemy, dt, time)
    -- TODO pass and utilize aim direction
    if (time > enemy.fireTime and enemy.fireTime ~= -1) then
        enemy.bulletCommand = Game_SpawnPattern(enemy.patternData.pattern, enemy.pos.x, enemy.pos.y)
        enemy.fireTime = time + enemy.patternData.freq
    end

    if (enemy.bulletCommand ~= nil) then
        Game_UpdatePattern(enemy.bulletCommand, enemy.pos.x, enemy.pos.y)
    end

    if (time >= enemy.transitionTime) then
        enemy.fsm:toEB()
    end

    enemy.fsm:runupdate(enemy, dt)

    Game_UpdateCollisionAgent(enemy.collisionAgent, enemy.pos.x, enemy.pos.y)
    if (Game_QueryCollisionAgent(enemy.collisionAgent)) then
        enemy.health = enemy.health -1
    end
end

function _Init()
    Game_AddTexture("back.png", "background")
    local st = 0
    
    -- simple, flow on top
    do
        local SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(100, 100), speed = 25})
        local EB = BindOptions(Behaviour_Nil)
        local pat = MakePatternData("enemy1.xml", .5, 2)
        AddEnemy(st, FSM(SB, EB), 10, MakeVec2(100, -100), pat)

        st = st + 1
        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(300, 125), speed = 25})
        AddEnemy(st, FSM(SB, EB), 10, MakeVec2(300, -150), pat)

        st = st + .5
        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(500, 150), speed = 25})
        AddEnemy(st, FSM(SB, EB), 10, MakeVec2(500, -175), pat)
    end

end

function _Cleanup()
    for idx,enemy in pairs(enemies) do
        KillEnemy(enemy)
        enemies[idx] = nil
    end
end

function _Update(dt, time)

    -- draw background
    local backgroundCoord = (time * backgroundData.velocity) % 640
    Game_DrawTexture("background", 0, backgroundCoord, 480, 640, 0)
    Game_DrawTexture("background", 0, backgroundCoord - 640, 480, 640, 0)
    
    -- process enemyQueue
    while (not LuaList.empty(enemyQueue) and LuaList.peekbottom(enemyQueue).spawnTime < startTime) do
        KillEnemy(LuaList.popbottom(enemyQueue))
    end
    while (not LuaList.empty(enemyQueue) and LuaList.peekbottom(enemyQueue).spawnTime < time + startTime) do
        enemy = LuaList.popbottom(enemyQueue)
        enemy.collisionAgent = Game_CreateCollisionAgent();
        table.insert(enemies, enemy)
    end


    -- updateEnemies
    --for idx,enemy in pairs(enemies) do
    for i=#enemies,1,-1 do
        UpdateEnemy(enemies[i], dt, time + startTime)
        if (enemies[i].health > 0) then
            Game_DrawTexture("enemy", enemies[i].pos.x-32, enemies[i].pos.y-32, 64, 64, 0)
        else
            KillEnemy(enemies[i])
            table.remove(enemies, i)
        end
        
    end

end

_Init()