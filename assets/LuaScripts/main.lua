require "utils"
require "behaviours"

enemyQueue = LuaList.new()
enemies = {}
nilpat = {pattern = nil, delay = -1, freq = -1}
startTime = 0

backgroundData = {velocity = 75, acceleration = 0, accelerating = true}

function BindOptions(f, options)
    return function(enemy, dt)
        return f(options, enemy, dt)
    end
end

function MakePatternData(pattern, delay, freq)
    return {pattern = pattern, delay = delay, freq = freq}
end

function AddEnemy(spawnTime, SB, SB_length, EB, spawnPosition, patternData, health)
    local enemy = {}
    enemy.spawnTime = spawnTime
    enemy.SB = SB
    enemy.transitionTime = spawnTime + SB_length
    enemy.EB = EB
    enemy.pos = spawnPosition
    enemy.patternData = patternData
    enemy.fireTime = patternData.delay
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

    if (time < enemy.transitionTime) then
        enemy.SB(enemy, dt)
    else
        enemy.EB(enemy, dt)
    end

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
        AddEnemy(st, SB, 10, EB, MakeVec2(100, -100), pat)

        st = st + 1
        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(300, 125), speed = 25})
        AddEnemy(st, SB, 10, EB, MakeVec2(300, -150), pat)

        st = st + .5
        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(500, 150), speed = 25})
        AddEnemy(st, SB, 10, EB, MakeVec2(500, -175), pat)
    end

    st = st + 1.5

    -- separated simple
    do
        local SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(100, 100), speed = 15})
        local EB = BindOptions(Behaviour_Nil)
        local pat = MakePatternData("enemy1.xml", .5, 3)
        AddEnemy(st, SB, 10, EB, MakeVec2(-50, 250), pat)

        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(700, 100), speed = 15})
        AddEnemy(st, SB, 10, EB, MakeVec2(850, 250), pat)
    end

    st = st + 4

    -- nil group 1
    do
        local SB = BindOptions(Behaviour_MoveAcross, {direction = 100, speed = 250})
        local EB = BindOptions(Behaviour_Kill)
        AddEnemy(st, SB, 15, EB, MakeVec2(600, -50), nilpat)
        AddEnemy(st, SB, 15, EB, MakeVec2(650, -50), nilpat)
        local SB = BindOptions(Behaviour_MoveAcross, {direction = 110, speed = 245})
        AddEnemy(st, SB, 15, EB, MakeVec2(600, -50), nilpat)
        AddEnemy(st, SB, 15, EB, MakeVec2(700, -50), nilpat)
        local SB = BindOptions(Behaviour_MoveAcross, {direction = 107, speed = 255})
        AddEnemy(st, SB, 15, EB, MakeVec2(600, -50), nilpat)
    end

    st = st + 1

    -- nil group 2
    do
        local SB = BindOptions(Behaviour_MoveAcross, {direction = 80, speed = 250})
        local EB = BindOptions(Behaviour_Kill)
        AddEnemy(st, SB, 15, EB, MakeVec2(200, -50), nilpat)
        AddEnemy(st, SB, 15, EB, MakeVec2(250, -50), nilpat)
        local SB = BindOptions(Behaviour_MoveAcross, {direction = 70, speed = 245})
        AddEnemy(st, SB, 15, EB, MakeVec2(200, -50), nilpat)
        AddEnemy(st, SB, 15, EB, MakeVec2(300, -50), nilpat)
        local SB = BindOptions(Behaviour_MoveAcross, {direction = 75, speed = 255})
        AddEnemy(st, SB, 15, EB, MakeVec2(200, -50), nilpat)
    end

    st = st + 3

    -- big guy + "distractions" on side
    do
        
        local EB = BindOptions(Behaviour_MoveAcross, {direction = 200, speed = 100})
        local SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(400, 150), speed = 15})
        local pat = MakePatternData("Waves.xml", 1, 2)
        AddEnemy(st, SB, 5, EB, MakeVec2(400, -100), pat, 170)  
        
        st = st + .5

        local SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(100, 100), speed = 15})
        local pat = MakePatternData("enemy1.xml", .5, 3)
        AddEnemy(st, SB, 10, EB, MakeVec2(-50, 250), pat)

        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(700, 100), speed = 15})
        AddEnemy(st, SB, 10, EB, MakeVec2(850, 250), pat)
    end

    st = st + 20

    -- speed up enemies
    -- "1.. 1.2.. .. 1.2.3."
    do
        local SB = BindOptions(Behaviour_MoveAcross, {direction = -90, speed = 1200})
        local EB = BindOptions(Behaviour_Kill)
        AddEnemy(st, SB, 1, EB, MakeVec2(300, 700), nilpat, 500)
        st = st + .5

        AddEnemy(st, SB, 1, EB, MakeVec2(300, 700), nilpat, 500)
        st = st + .1
        AddEnemy(st, SB, 1, EB, MakeVec2(250, 700), nilpat, 500)
        st = st + 1

        AddEnemy(st, SB, 1, EB, MakeVec2(500, 700), nilpat, 500)
        st = st + .075
        AddEnemy(st, SB, 1, EB, MakeVec2(550, 700), nilpat, 500)
        st = st + .1
        AddEnemy(st, SB, 1, EB, MakeVec2(520, 700), nilpat, 500)
        st = st + 1
        SB = BindOptions(Behaviour_SpeedUpPace)
        AddEnemy(st, SB, 10, EB, MakeVec2(-100, -100), nilpat, 500)
    end

    st = st + 5
    
    -- reintroduce enemies
    do
        local SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(100, 100), speed = 25})
        local EB = BindOptions(Behaviour_Nil)
        local pat = MakePatternData("enemy1.xml", .5, 2)
        AddEnemy(st, SB, 10, EB, MakeVec2(100, -100), pat)

        st = st + 1
        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(300, 125), speed = 25})
        AddEnemy(st, SB, 10, EB, MakeVec2(300, -150), pat)

        st = st + .5
        SB = BindOptions(Behaviour_DampedMoveToTarget, {target = MakeVec2(500, 150), speed = 25})
        AddEnemy(st, SB, 10, EB, MakeVec2(500, -175), pat)
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