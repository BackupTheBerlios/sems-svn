set (libbinrpc_SRCS
call.c
cb.c
dissector.c
errno.c
lock.c
log.c
mem.c
net.c
print.c
time.c
value.c
)

###
# Used flags
#
# NDEBUG				:	release build
# INT_AS_ID				:	allow integer records as attribute identifes 
# 							(besides str)
# DFIX_FALSE_GCC_WARNS	:	initialize some vars falsely signaled by GCC
# 							(4.1.1) as potentially used without.
# 							(some may be triggered by pointer usage without
# 							'const' or 'restrict' keyword)
# BINRPC_REENTRANT		:	enable thread local storage variables (set them 
# 							global)
# NOP_DFL_LOCKS			:	hollow locking; if not set and reentrant,
# 							realtime's locking system will be used
# BINRPC_VER1_COMPAT	:	compatibility with version 1 (TODO: :) )
# USE_DEFAULT_SYSLOG	:	use syslog as default logger, instead of black
# 							hole
#
ADD_DEFINITIONS(-DBINRPC_LIB_VER="1.0-1" -D_LIBBINRPC_BUILD -DNDEBUG)

ADD_LIBRARY(libbinrpc STATIC ${libbinrpc_SRCS})
SET_TARGET_PROPERTIES(libbinrpc PROPERTIES OUTPUT_NAME "libbinrpc")

