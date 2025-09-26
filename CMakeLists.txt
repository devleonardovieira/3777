cmake_minimum_required(VERSION 3.10)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(theforgottenserver CXX)

add_subdirectory(src)
add_executable(theforgottenserver ${theforgottenserver_SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_definitions(-D__USE_MYSQL__ -D__USE_SQLITE__ -D__ROOT_PERMISSION__ -D__ENABLE_SERVER_DIAGNOSTIC__)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "-Ofast -DNDEBUG")
    include(FindCXX11)
endif()

find_package(GMP REQUIRED)
find_package(MySQL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem regex thread)

# Use LuaJIT from vcpkg - manual configuration
if(EXISTS "E:/vcpkg/installed/x64-windows/include/luajit")
  set(LUA_INCLUDE_DIR "E:/vcpkg/installed/x64-windows/include/luajit")
  set(LUA_LIBRARIES "E:/vcpkg/installed/x64-windows/lib/lua51.lib")
  message(STATUS "Using LuaJIT from vcpkg: ${LUA_LIBRARIES}")
else()
  find_package(Lua 5.1 REQUIRED)
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${SQLITE3_INCLUDE_DIRS}
  ${GMP_INCLUDE_DIR}
)
target_link_libraries(theforgottenserver PRIVATE
  Boost::filesystem
  Boost::regex
  Boost::thread
  OpenSSL::SSL
  LibXml2::LibXml2
  ZLIB::ZLIB
  SQLite::SQLite3
  ${LUA_LIBRARIES}
  ${MYSQL_CLIENT_LIBS}
  ${SQLITE3_LIBRARIES}
  ${GMP_LIBRARY}
)
