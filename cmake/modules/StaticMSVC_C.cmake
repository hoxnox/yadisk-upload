if (MSVC)
	set(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
	set(CMAKE_C_FLAGS_MINSIZE_REL_INIT "/MT /Os /Ob1 /D NDEBUG")
	set(CMAKE_C_FLAGS_RELEASE_INIT "/MT /Ox /D NDEBUG")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /Ox /Ob1 /D NDEBUG")
endif()
