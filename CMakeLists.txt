project(libcutils)

enable_language(ASM)

add_definitions(-DANDROID_SMP=1 -DHAVE_PTHREADS -DHAVE_SYS_SOCKET_H)

set(LIBCUTILS_SRC_FILES
    hashmap.c
    atomic.c
    native_handle.c
    socket_inaddr_any_server.c
    socket_local_client.c
    socket_local_server.c
    socket_loopback_client.c
    socket_loopback_server.c
    socket_network_client.c
    sockets.c
    config_utils.c
    cpu_info.c
    load_file.c
    list.c
    open_memstream.c
    strdup16to8.c
    strdup8to16.c
    record_stream.c
    process_name.c
    threads.c
    sched_policy.c
    iosched_policy.c
    str_parms.c
    ashmem-dev.c
    debugger.c
    klog.c
    partition_utils.c
    properties.c
    qtaguid.c
    trace.c
    uevent.c
    arch-arm/memset32.S
)

add_library(cutils SHARED ${LIBCUTILS_SRC_FILES})
install(TARGETS cutils DESTINATION lib)
install(DIRECTORY cutils DESTINATION include)

target_include_directories(cutils
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)