# Chess PBL (refactored source layout)

This version keeps the existing Qt/C++ implementation but separates large source files by responsibility.

## Source layout

- `main.cpp`: starts the application and opens the dashboard.
- `dashboard.cpp/.h`: game-mode selection and dashboard actions.
- `chessboard.cpp/.h`: initializes the board window and its state.
- `chessboard_layout.cpp`: builds the header, clocks, coordinate grid, and controls.
- `chessboard_render.cpp`: square styling, piece display, and board refresh.
- `chessboard_actions.cpp`: square clicks, undo, dashboard navigation, and settings.
- `chessboard_clock.cpp`: clock display and timeout handling.
- `chessgame.cpp/.h`: board setup, getters, piece names, and symbols.
- `chessgame_moves.cpp`: piece movement rules and path checking.
- `chessgame_check.cpp`: king safety, legal-move checks, checkmate, and stalemate.
- `chessgame_history.cpp`: executing moves, undoing moves, and move count.
- `movehistory.cpp/.h`: fixed-size stack used for move history.
- `move.h` and `piece.h`: simple data structures.

## Build on Windows

Use the same Qt 6 MinGW kit that was used for the original project. Open a terminal in this folder and run the project's existing CMake configuration. For example, if Qt is installed at `C:\Qt\6.12.0\mingw_64` and its MinGW compiler is at `C:\Qt\Tools\mingw1310_64\bin`:

```powershell
cmake -S . -B build -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER="C:\Qt\Tools\mingw1310_64\bin\gcc.exe" `
  -DCMAKE_CXX_COMPILER="C:\Qt\Tools\mingw1310_64\bin\g++.exe" `
  -DCMAKE_MAKE_PROGRAM="C:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe" `
  -DCMAKE_PREFIX_PATH="C:\Qt\6.12.0\mingw_64"

cmake --build build
```

If you already have a `build` folder from the old layout, delete that generated folder before configuring so CMake doesn't reuse stale build files.

## Team workflow

Each member should commit the changes they actually worked on. Coordinate changes to shared headers and `CMakeLists.txt`, pull the latest main branch before starting, and build/test before opening a pull request.
