import Timer


class _Timer(object):

    @staticmethod
    def add_timer(loop_time, func, loop_cnt=1):
        loop_time = int(loop_time * 1000)
        return Timer.addTimer(loop_time, loop_time, loop_cnt, func)

    @staticmethod
    def add_custom_timer(first_time, interval, func, loop_cnt=1):
        first_time = int(first_time * 1000)
        interval = int(interval * 1000)
        return Timer.addTimer(first_time, interval, loop_cnt, func)

    @staticmethod
    def remove_timer(timer_id):
        Timer.removeTimer(timer_id)


add_timer = _Timer.add_timer
add_custom_timer = _Timer.add_custom_timer
remove_timer = _Timer.remove_timer
