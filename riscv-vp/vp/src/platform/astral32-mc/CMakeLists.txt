file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(astral32-mc
        mc_main.cpp
        CTBSearcherL.cpp
        CTBSearcherR.cpp
        CTSearcherB.cpp
        CTSearcher.cpp
        ${HEADERS})

target_link_libraries(astral32-mc rv32 platform-common gdb-mc
	${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS astral32-mc RUNTIME DESTINATION bin)
