
add_library(osm2pgsql_lib STATIC)

# Set the minimum required C++ version for the library and hence for all
# binaries that use it.
target_compile_features(osm2pgsql_lib PUBLIC cxx_std_17)

target_sources(osm2pgsql_lib PRIVATE
  command-line-parser.cpp
  db-copy.cpp
  debug-output.cpp
  dependency-manager.cpp
  expire-tiles.cpp
  expire-output.cpp
  gazetteer-style.cpp
  geom.cpp
  geom-box.cpp
  geom-from-osm.cpp
  geom-functions.cpp
  geom-pole-of-inaccessibility.cpp
  input.cpp
  logging.cpp
  middle.cpp
  middle-pgsql.cpp
  middle-ram.cpp
  node-locations.cpp
  node-persistent-cache.cpp
  ordered-index.cpp
  osmdata.cpp
  output-gazetteer.cpp
  output-null.cpp
  output-pgsql.cpp
  output.cpp
  pgsql.cpp
  pgsql-capabilities.cpp
  pgsql-helper.cpp
  progress-display.cpp
  properties.cpp
  reprojection.cpp
  table.cpp
  taginfo.cpp
  tagtransform-c.cpp
  tagtransform.cpp
  tile.cpp
  thread-pool.cpp
  util.cpp
  wildcmp.cpp
  wkb.cpp
)

if (WITH_LUA)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
                 "${CMAKE_CURRENT_SOURCE_DIR}/init.lua")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/init.lua" LUA_INIT_CODE)
    configure_file(lua-init.cpp.in lua-init.cpp @ONLY)
    target_sources(osm2pgsql_lib PRIVATE
        flex-index.cpp
        flex-table.cpp
        flex-table-column.cpp
        flex-lua-expire-output.cpp
        flex-lua-geom.cpp
        flex-lua-index.cpp
        flex-lua-table.cpp
        flex-write.cpp
        geom-transform.cpp
        lua-setup.cpp
        lua-utils.cpp
        output-flex.cpp
        tagtransform-lua.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/lua-init.cpp)
endif()

target_sources(osm2pgsql_lib PRIVATE ${PROJECT_BINARY_DIR}/src/version.cpp)

if (HAVE_PROJ4)
    target_sources(osm2pgsql_lib PRIVATE reprojection-generic-proj4.cpp)
    if (NOT MSVC)
        set_source_files_properties(reprojection-generic-proj4.cpp
            PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)
    endif()
elseif(HAVE_PROJ6)
    target_sources(osm2pgsql_lib PRIVATE reprojection-generic-proj6.cpp)
else()
    target_sources(osm2pgsql_lib PRIVATE reprojection-generic-none.cpp)
endif()

set_target_properties(osm2pgsql_lib PROPERTIES OUTPUT_NAME osm2pgsql)
target_link_libraries(osm2pgsql_lib ${LIBS})

