file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${SymbolicDifferentiation_SOURCE_DIR}/include/*.h"
)

add_library(sd_lib operation.cpp  parser.cpp ${HEADER_LIST})
target_link_libraries(sd_lib PUBLIC fmt::fmt )
target_include_directories(sd_lib PUBLIC ../include)

target_compile_features(sd_lib PUBLIC cxx_std_23)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
