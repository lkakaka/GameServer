
timer = {}

function timer.addTimer(loop_time, loop_cnt, func)
    loop_cnt = loop_cnt or 1
    loop_time = loop_time * 1000
    return Timer.addTimer(loop_time, loop_time, loop_cnt, func)
end

function timer.removeTimer(timerId)
    return Timer.removeTimer(timerId)
end

function timer.test()
    timer.addTimer(1, 10, function(timerId) print("on timer " .. timerId) end)
end

