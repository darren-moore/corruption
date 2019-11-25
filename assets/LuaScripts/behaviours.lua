
function Behaviour_Nil(options, enemy, dt)
end

function Behaviour_Kill(options, enemy, dt)
    enemy.health = 0
end

function Behaviour_SpeedUpPace(options, enemy, dt)
    local target = 500
    local overTime = 7
    -- use triangular accel profile.
    local peakAccel = 2*target/overTime
    local accelSlope = peakAccel/(overTime/2)

    if (backgroundData.accelerating) then
        backgroundData.acceleration = backgroundData.acceleration + accelSlope*dt
    else
        backgroundData.acceleration = backgroundData.acceleration - accelSlope*dt
    end

    if (backgroundData.acceleration > peakAccel) then
        backgroundData.accelerating = false
    end

    if (backgroundData.velocity-75 >= target or backgroundData.acceleration < 0) then
        enemy.health = 0
        backgroundData.accelerating = 0
    end

    backgroundData.velocity = backgroundData.velocity + backgroundData.acceleration*dt
end

function Behaviour_MoveAcross(options, enemy, dt)
    local direction = options.direction
    local speed = options.speed

    local velocity = speed * MakeVec2(math.cos(direction*(3.14/180)), math.sin(direction*(3.14/180)))
    enemy.pos = enemy.pos + velocity * dt
end


function Behaviour_DampedMoveToTarget(options, enemy, dt)
    local target = options.target
    local speedMin = options.speed

    local K_SPEED_DAMP = 1

    local heading = target - enemy.pos
    local dist = math.sqrt(heading * heading)

    -- so we don't normalize a near zero vector
    if (dist < .001) then
        return
    end

    local speed = dist*K_SPEED_DAMP
    heading = 1/dist * heading --normalize
    local velocity = math.max(speed, speedMin) * heading
    enemy.pos = enemy.pos + velocity * dt
end
