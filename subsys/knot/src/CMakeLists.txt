zephyr_library_named(knot_platform)
zephyr_library_sources(protocol.c states.c)

# The source files here use header files from the KNoT platform
# target_link_libraries(knot_platform knotprotocol)
add_dependencies(knot_platform knotprotocol)
