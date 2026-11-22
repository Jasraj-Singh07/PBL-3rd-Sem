cmake_minimum_required(VERSION 3.16)

project(ChessPBL VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(ChessPBL
    main.cpp

    piece.h
    move.h
    movehistory.h

    movehistory.cpp

    chessgame.h
    chessgame.cpp
    chessgame_moves.cpp
    chessgame_check.cpp
    chessgame_history.cpp

    chessboard.h
    chessboard.cpp
    chessboard_layout.cpp
    chessboard_render.cpp
    chessboard_actions.cpp
    chessboard_clock.cpp

    dashboard.h
    dashboard.cpp

    ai.h
    ai.cpp
    playerstats.h
    playerstats.cpp
)

target_link_libraries(ChessPBL PRIVATE Qt6::Widgets)
