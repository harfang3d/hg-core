set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_SYSROOT ${SYSROOT})
set(TARGET_PREFIX "aarch64-linux-gnu")

set(CMAKE_C_COMPILER ${TOOLCHAIN}/bin/${TARGET_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/bin/${TARGET_PREFIX}-g++)

set(ENV{CFLAGS} "--sysroot=${SYSROOT} $ENV{CFLAGS}")
set(ENV{LDFLAGS} "--sysroot=${SYSROOT} -Wl,-rpath,${SYSROOT}/lib/${TARGET_PREFIX} $ENV{LDFLAGS}")


if(IS_DIRECTORY "${SYSROOT}/usr/lib/pkgconfig")
    set(ENV{PKG_CONFIG_PATH} $ENV{SYSROOT}/usr/lib/pkgconfig)
endif()
if(IS_DIRECTORY "${SYSROOT}/usr/lib/${TARGTE_PREFIX}/pkgconfig")
    set(ENV{PKG_CONFIG_PATH} $ENV{SYSROOT}/usr/lib/${TARGTE_PREFIX}/pkgconfig)
endif()
if (NOT "$ENV{PKG_CONFIG_PATH}" STREQUAL "")
    set(ENV{PKG_CONFIG_SYSROOT_DIR} $ENV{SYSROOT})
    # Don't strip -I/usr/include out of cflags
    set(ENV{PKG_CONFIG_ALLOW_SYSTEM_CFLAGS} 1)
    # Don't strip -L/usr/lib out of libs
    set(ENV{PKG_CONFIG_ALLOW_SYSTEM_LIBS} 1)
endif()

add_compile_options("-march=armv8-a")

set(CMAKE_FIND_ROOT_PATH ${SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
