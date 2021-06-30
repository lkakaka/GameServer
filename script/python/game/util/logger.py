
import Logger


def log_info(fmt, *p):
    Logger.logInfo("$[PyLog] " + fmt.format(*p))


def log_error(fmt, *p):
    Logger.logError("$[PyLog] " + fmt.format(*p))


def log_debug(fmt, *p):
    Logger.logDebug("$[PyLog] " + fmt.format(*p))


def log_warn(fmt, *p):
    Logger.logWarn("$[PyLog] " + fmt.format(*p))
