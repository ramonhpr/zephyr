include(ExternalProject)

set(ep_base ${PROJECT_BINARY_DIR}/ext_proj)
set_property(DIRECTORY PROPERTY "EP_BASE" ${ep_base})

# Construct a list of commands to give to ExternalProject_Add()
#
# See https://cmake.org/cmake/help/latest/module/ExternalProject.html
# for documentation on ExternalProject_Add
set(cmd "")

set(knot_name knot-protocol)
list(APPEND cmd
  ${knot_name}
  )

set(knot_STAMP_DIR   ${ep_base}/Stamp/${knot_name})
set(knot_INSTALL_DIR ${ep_base}/Install/${knot_name})


#--Download step--------------
if(NOT EXTERNAL_PROJECT_PATH_KNOT_PROTOCOL)
  # TODO: Point to a Zephyr fork
  # Nov. 7
  set_ifndef(knot_GIT_REPOSITORY "https://github.com/cesarbr/knot-protocol-source.git")
  set_ifndef(knot_GIT_TAG da475057a24e0c3e9cb0d7135b4d089f9075650f)
  set_ifndef(knot_GIT_PROGRESS 1)

  list(APPEND cmd
    GIT_REPOSITORY ${knot_GIT_REPOSITORY}
    GIT_TAG        ${knot_GIT_TAG}
    GIT_PROGRESS   ${knot_GIT_PROGRESS}
    )
endif()
#--Update/Patch step-------------

# An update step is not necessary because we are using a commit hash
# as a tag, and the code behind a hash cannot change.

# UPDATE_DISCONNECTED should be used when updates aren't needed, but
# for some reason we were not able to get this to work, so we use a
# dummy step to fake updating. This ensures that the git repo is not
# downloaded on every 'make' invocation.
list(APPEND cmd
  UPDATE_COMMAND
  ${CMAKE_COMMAND} -E touch ${knot_STAMP_DIR}/${knot_name}-update
  )


#--Configure step-------------
if(EXTERNAL_PROJECT_PATH_KNOT_PROTOCOL)
  set(knot_SOURCE_DIR ${EXTERNAL_PROJECT_PATH_KNOT_PROTOCOL})

  list(APPEND cmd
    SOURCE_DIR ${knot_SOURCE_DIR}
    )

else()
  set(knot_SOURCE_DIR ${ep_base}/Source/${knot_name}) # TODO: Download dir?

  # "If SOURCE_DIR is explicitly set to an existing directory the
  # project will be built from it. Otherwise a download step must be
  # specified using one of the DOWNLOAD_COMMAND, CVS_*, SVN_*, or URL
  # options." -- CMake docs
endif()

find_program(INSTALL install)

set(exec_prefix zephyr)
set(configure_flags
  "INSTALL=${INSTALL} -p"
  "CFLAGS=-I${knot_SOURCE_DIR}/src"
  "LDFLAGS=-lc -lrdimon -specs=rdimon.specs"

  --host=arm-none-eabi
  --prefix=/
  --exec-prefix=/${exec_prefix}
  --target=arm-none-eabi # TODO: Is Kconfig expressing that OT is ARM-only?
)

list(APPEND cmd
  CONFIGURE_COMMAND ./configure ${configure_flags}
)

#--Build step-----------------
set(knot_BINARY_DIR  ${knot_SOURCE_DIR})
list(APPEND cmd
  BINARY_DIR  ${knot_BINARY_DIR}
  INSTALL_DIR ${knot_INSTALL_DIR}
  )

set(make_flags
  --no-print-directory
  )

list(APPEND cmd
  BUILD_COMMAND   make ${make_flags}                             all
  INSTALL_COMMAND make ${make_flags} DESTDIR=${knot_INSTALL_DIR} install
  )

set(knot_source_dir ${knot_SOURCE_DIR}/src)

zephyr_system_include_directories(${knot_source_dir})

target_include_directories(app SYSTEM PRIVATE ${ot_source_dir})

zephyr_include_directories(${knot_source_dir})


set(knot_lib knotprotocol)
set(build_byproducts ${build_byproducts} ${knot_INSTALL_DIR}/${exec_prefix}/libs/lib${knot_lib}.a)

list(APPEND cmd
  BUILD_BYPRODUCTS ${build_byproducts}
  )

ExternalProject_Add(${cmd})

ExternalProject_Add_Step(
  ${knot_name} bootstrap # Names of project and custom step
  COMMAND ./bootstrap     # Command line invoked by this step
  COMMENT "bootstrapping..." # Text printed when step executes
  DEPENDEES download        # Steps on which this step depends
  DEPENDERS configure     # Steps that depend on this step
  WORKING_DIRECTORY ${knot_SOURCE_DIR}
  )


# Create wrapper CMake libraries
add_library(${knot_lib} STATIC IMPORTED GLOBAL)
add_dependencies(
    ${knot_lib}
    ${knot_name}
    )
set_target_properties(${knot_lib} PROPERTIES IMPORTED_LOCATION
    ${knot_INSTALL_DIR}/${exec_prefix}/lib/lib${knot_lib}.a
    )

zephyr_append_cmake_library(${knot_lib})

