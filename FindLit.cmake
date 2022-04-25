find_path(LIT_INCLUDE_DIR lit/lit.h
	$ENV{LIT_DIR}/include
	$ENV{LIT_DIR}
	/usr/local/include
	/usr/include
)

find_library(LIT_LIBRARIES
	NAMES liblit lit
	PATHS
	$ENV{LIT_DIR}/lib
	$ENV{LIT_DIR}
	/usr/local/lib
	/usr/lib
)

set(LIT_FOUND "NO")

if(LIT_LIBRARIES AND LIT_INCLUDE_DIR)
	set(LIT_FOUND "YES")

	add_library(lit INTERFACE IMPORTED)
	set_target_properties(lit PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${LIT_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES "${LIT_LIBRARIES}"
	)
endif()