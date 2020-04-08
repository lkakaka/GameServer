#
# Nicholas Yue nicholas_yue@users.sourceforge.net
#
# Note:
# (1) A dummy file include/config.h is required (remance from configure)
# (2) Default installation directory is /usr/local, override with -DCMAKE_INSTALL_PREFIX="" during cmake
#     invocation
# (3) Do the usual "make clean all" to build the library
# (4) To install either "make install" or "make install DESTDIR=<your directory>"
# (5) Need to include changes in include/log4cpp/Portability.hh for OSX to build

# PROJECT ( LOG4CPP )
#set(CMAKE_DEBUG_POSTFIX "_d")

INCLUDE_DIRECTORIES ( include )
INCLUDE_DIRECTORIES ( . )

IF (WIN32)
  ADD_DEFINITIONS ( -D_CRT_SECURE_NO_WARNINGS )
ELSE (WIN32)
  IF (APPLE)
    ADD_DEFINITIONS ( -DNDEBUG -DLOG4CPP_HAVE_SSTREAM )
  ELSE (APPLE)
    ADD_DEFINITIONS ( -pthread -DNDEBUG -DLOG4CPP_HAVE_SSTREAM )
  ENDIF (APPLE)
ENDIF (WIN32)

IF (WIN32)
IF ( CMAKE_BUILD_TYPE MATCHES "Debug" )
  SET ( LOG4CPP_LIBRARY_NAME "log4cppD" )
ELSE ( CMAKE_BUILD_TYPE MATCHES "Debug" )
  SET ( LOG4CPP_LIBRARY_NAME "log4cpp" )
ENDIF ( CMAKE_BUILD_TYPE MATCHES "Debug" )
ELSE (WIN32)
  set(CMAKE_DEBUG_POSTFIX "_d")
ENDIF (WIN32)


if (CMAKE_COMPILER_IS_GNUCXX)
 	set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
 	MESSAGE(STATUS ${CMAKE_CXX_FLAGS})
endif (CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY ( ${LOG4CPP_LIBRARY_NAME}
  src/Appender.cpp
  src/AppenderSkeleton.cpp
  src/AppendersFactory.cpp
  src/BufferingAppender.cpp
  src/FactoryParams.cpp
  src/LayoutsFactory.cpp
  src/LevelEvaluator.cpp
  src/Localtime.cpp
  src/PassThroughLayout.cpp
  src/TriggeringEventEvaluatorFactory.cpp
  src/LayoutAppender.cpp
  src/FileAppender.cpp
  src/DailyRollingFileAppender.cpp
  src/RollingFileAppender.cpp
  src/FixedContextCategory.cpp
  src/IdsaAppender.cpp
  src/OstreamAppender.cpp
  src/StringQueueAppender.cpp
  src/SyslogAppender.cpp
  src/RemoteSyslogAppender.cpp
  src/SimpleLayout.cpp
  src/BasicLayout.cpp
  src/PatternLayout.cpp
  src/Category.cpp
  src/CategoryStream.cpp
  src/HierarchyMaintainer.cpp
  src/Configurator.cpp
  src/BasicConfigurator.cpp
  src/SimpleConfigurator.cpp
  src/PropertyConfigurator.cpp
  src/PropertyConfiguratorImpl.cpp
  src/LoggingEvent.cpp
  src/Priority.cpp
  src/NDC.cpp
  src/Filter.cpp
  src/TimeStamp.cpp
  src/StringUtil.cpp
  src/Properties.cpp
  src/Win32DebugAppender.cpp
  src/NTEventLogAppender.cpp
  src/DllMain.cpp
  src/DummyThreads.cpp
  src/MSThreads.cpp
  src/OmniThreads.cpp
  src/PThreads.cpp
  src/PortabilityImpl.cpp
  src/AbortAppender.cpp
  logger/Logger.cpp
)

IF (WIN32)
  TARGET_LINK_LIBRARIES (${LOG4CPP_LIBRARY_NAME} kernel32 user32 ws2_32 advapi32 )
  SET_TARGET_PROPERTIES(${LOG4CPP_LIBRARY_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:msvcrt )
ENDIF (WIN32)

# INSTALL (
#   DIRECTORY include/log4cpp
#   DESTINATION include
#   PATTERN "config.h" EXCLUDE
#   PATTERN ".svn" EXCLUDE
#   PATTERN "*.am" EXCLUDE
#   PATTERN "*.in" EXCLUDE
#   )

# INSTALL (
#   TARGETS ${LOG4CPP_LIBRARY_NAME}
#   ARCHIVE DESTINATION lib
#   )
