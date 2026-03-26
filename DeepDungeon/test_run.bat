@echo off
chcp 65001 > nul
cd /d %~dp0
echo.
echo === Deep Dungeon Test Run ===
echo.
echo Running with simulated input: d d d s s (move right 3x, down 2x)
echo.
echo d
echo d
echo d
echo s
echo s
echo q
) | DeepDungeon.exe
