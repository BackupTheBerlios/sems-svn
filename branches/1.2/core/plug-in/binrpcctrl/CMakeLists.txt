set (binrpcctrl_SRCS
BrpcCtrlInterface.cpp
ConnPool.cpp
CtrlServer.cpp
)

ADD_SUBDIRECTORY (libbinrpc)
INCLUDE_DIRECTORIES (libbinrpc/include)

SET(sems_module_name binrpcctrl)
SET(sems_module_libs libbinrpc)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/module.rules.txt)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/config.rules.txt)
