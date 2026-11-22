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
    performancechart.h
    performancechart.cpp
    soundeffects.h
    soundeffects.cpp

    ai.h
    ai.cpp
    playerstats.h
    playerstats.cpp
)

target_link_libraries(ChessPBL PRIVATE Qt6::Widgets)

include(CTest)
if(BUILD_TESTING)
    add_executable(engine_smoke tests/engine_smoke.cpp ai.cpp movehistory.cpp chessgame.cpp chessgame_moves.cpp chessgame_check.cpp chessgame_history.cpp)
    target_include_directories(engine_smoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(engine_smoke PRIVATE Qt6::Core)
    add_test(NAME engine_smoke COMMAND engine_smoke)
endif()
option(BUILD_CHESSVERSE "Build the ChessVerse Quick dashboard" ON)
if(BUILD_CHESSVERSE)
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Quick3D Concurrent Multimedia)
add_executable(ChessVerse verse_main.cpp verse.cpp verse_history.cpp verse_training.cpp verse.h ai.cpp movehistory.cpp chessgame.cpp chessgame_moves.cpp chessgame_check.cpp chessgame_history.cpp)
qt_add_resources(ChessVerse "verse_ui" PREFIX "/" FILES Main.qml Board3D.qml LiveChart.qml MatchRecap.qml MemeDance.qml art/orbit.png audio/button.wav audio/move.wav audio/capture.wav)
target_link_libraries(ChessVerse PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Quick3D Qt6::Concurrent Qt6::Multimedia)
endif()
