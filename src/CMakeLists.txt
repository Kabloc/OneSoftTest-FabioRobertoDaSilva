include_directories ( ${opensoft_test_SOURCE_DIR}/include/ )

if( WIN32 )
   set( BOOST_ROOT "C:/Boost" )
   if( MSVC )
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
   endif( MSVC )
endif( WIN32  )

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package( Boost 1.48 REQUIRED COMPONENTS system thread )
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable (neg_file_server market_data.cpp
                                neg_server.cpp
                                server.cpp)

add_executable (client_test client.cpp)