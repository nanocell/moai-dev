cmake_minimum_required ( VERSION 2.8.5 )
project ( tlsf )

set ( CMAKE_CURRENT_SOURCE_DIR "${MOAI_ROOT}/3rdparty/tlsf-2.0/" )

set ( TLSF_INCLUDES
  "${MOAI_ROOT}/3rdparty/tlsf-2.0/"
CACHE INTERNAL "doc string" )

set ( TLSF_SOURCES

  ${CMAKE_CURRENT_SOURCE_DIR}/tlsf.c

CACHE INTERNAL "doc string" )