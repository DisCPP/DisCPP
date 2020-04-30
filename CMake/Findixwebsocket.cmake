find_path(IXWEBSOCKET_HEADERS ixwebsocket/IXWebSocket.h)

find_library(IXWEBSOCKET_LIBRARY ixwebsocket)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IXWEBSOCKET DEFAULT_MSG
        IXWEBSOCKET_HEADERS IXWEBSOCKET_LIBRARY)

mark_as_advanced(IXWEBSOCKET_HEADERS IXWEBSOCKET_LIBRARY)