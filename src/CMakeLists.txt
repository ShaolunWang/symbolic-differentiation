file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
	"${AutomaticDifferentiation_SOURCE_DIR}/include/*.h"
)

add_library(ad_lib operation.cpp  parser.cpp ${HEADER_LIST})
target_link_libraries(ad_lib PUBLIC fmt::fmt )
target_include_directories(ad_lib PUBLIC ../include)

target_compile_features(ad_lib PUBLIC cxx_std_23)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
