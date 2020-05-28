find_path(MBEDTLS_HEADERS mbedtls/ssl.h)

find_library(MBEDTLS_LIBRARY mbedtls)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(mbedtls DEFAULT_MSG
        MBEDTLS_HEADERS MBEDTLS_LIBRARY)

mark_as_advanced(MBEDTLS_HEADERS MBEDTLS_LIBRARY)