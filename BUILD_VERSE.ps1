$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
$qt='C:/Qt/6.12.0/mingw_64'
$mingw='C:/Qt/Tools/mingw1310_64/bin'
$env:PATH="$qt/bin;$mingw;$env:PATH"
cmake -S . -B build-verse -G 'MinGW Makefiles' "-DCMAKE_PREFIX_PATH=$qt" "-DCMAKE_CXX_COMPILER=$mingw/g++.exe"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build build-verse --target ChessVerse -j4
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& "$qt/bin/windeployqt.exe" --qmldir . build-verse/ChessVerse.exe
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& .\build-verse\ChessVerse.exe
exit $LASTEXITCODE
