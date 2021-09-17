
logger = {}

function logger.logInfo(fmt, ...)
    local msg = string.format(fmt, ...)
    Logger.log_info("[Lua] " .. msg)
end

function logger.logError(fmt, ...)
    local msg = string.format(fmt, ...)
    Logger.log_error("[Lua] " .. msg)
end

function logger.logDebug(fmt, ...)
    local msg = string.format(fmt, ...)
    Logger.log_debug("[Lua] " .. msg)
end

function logger.logWarn(fmt, ...)
    local msg = string.format(fmt, ...)
    Logger.log_warn("[Lua] " .. msg)
end
