
require("util.logger")

Mem = {}

function Mem.usage()
    local mem_count = collectgarbage("count")
    local msg = string.format("mem size: %fKB",  mem_count)
    logger.logInfo(msg)
    return msg
end

function Mem.gc()
    local mem_count = collectgarbage("count")
    collectgarbage("collect")
    local new_mem_count = collectgarbage("count")
    local msg = string.format("gc, before gc mem size: %fKB, after gc mem size: %fKB", mem_count, new_mem_count)
    logger.logInfo(msg)
    return msg
end