cmake_minimum_required ( VERSION 2.8.5 )
project ( yaml )

if ( MOAI_YAML )

  set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMOAI_WITH_YAML=1" CACHE INTERNAL "doc string")
  set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMOAI_WITH_YAML=1" CACHE INTERNAL "doc string")

  set ( CMAKE_CURRENT_SOURCE_DIR "${MOAI_ROOT}/3rdparty/libyaml-0.1.4" )
  set ( LUAYAML_SOURCE_DIR       "${MOAI_ROOT}/3rdparty/luayaml-6e68108" )

  set ( YAML_INCLUDES 
      "${CMAKE_CURRENT_SOURCE_DIR}/include"
      "${CMAKE_CURRENT_SOURCE_DIR}/src"
      "${LUAYAML_SOURCE_DIR}"
    CACHE INTERNAL "doc string" )

  # Use all cpp files in directory as source code
  file ( GLOB YAML_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" )
  file ( GLOB LUAYAML_SOURCE_FILES "${LUAYAML_SOURCE_DIR}/*.c" )

  set ( YAML_SOURCES

    ${YAML_SOURCE_FILES}
    ${LUAYAML_SOURCE_FILES}

  CACHE INTERNAL "doc string" )

else ( MOAI_YAML )

  set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMOAI_WITH_YAML=0" CACHE INTERNAL "doc string")
  set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMOAI_WITH_YAML=0" CACHE INTERNAL "doc string")

endif ( MOAI_YAML )