
logger = {}

function logger.log_info(fmt, ...)
    local msg = string.format(fmt, ...)
    Logger.log_info("$[Lua] " .. msg)
end

function logger.log_error(fmt, ...)
    local msg = string.format(fmt, ...)
    Logger.log_error("$[Lua] " .. msg)
end

function logger.log_debug(fmt, ...)
    local msg = string.format(fmt, ...)
    Logger.log_debug("$[Lua] " .. msg)
end
