
require("math")

timer = {}

function timer.add_timer(loop_time, loop_cnt, func)
    loop_cnt = loop_cnt or 1
    loop_time = math.floor(loop_time * 1000)
    return Timer.addTimer(loop_time, loop_time, loop_cnt, func)
end

function timer.remove_timer(timerId)
    return Timer.removeTimer(timerId)
end

function timer.test()
    timer.add_timer(1, 10, function(timerId) print("on timer " .. timerId) end)
end

