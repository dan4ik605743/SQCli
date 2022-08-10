find_package(Boost REQUIRED COMPONENTS program_options REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

set(
    SRC_LIST
    main.cpp 
    sqcli/sqcli.cpp
    options/options.cpp
    io_controller/io_controller.cpp
)

set(
    LIB_LIST
    PRIVATE fmt
    ${CMAKE_THREAD_LIBS_INIT} 
    ${SQLite3_LIBRARIES}
    ${Boost_LIBRARIES} 
)

set(
    FLAG_LIST
    -Wall 
    -Werror 
    -Wextra 
    -Winit-self 
    -Wredundant-decls
    -Wpedantic 
    -flto=auto -O3
)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${SQLite3_INCLUDE_DIRS}
)

add_executable(
    ${PROJECT_NAME} 
    ${SRC_LIST}
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    ${FLAG_LIST}
)

target_link_libraries(
    ${PROJECT_NAME} 
    ${LIB_LIST}
)

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}        
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
