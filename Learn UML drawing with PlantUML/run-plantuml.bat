@echo off
rem The line above prevents the commands themselves from being displayed.

echo.
rem This adds a blank line for better readability.

set /p user_input="Please enter path to the PUML file: "
rem The `set /p` command prompts for input and stores it in the `user_input` variable.

java -jar plantuml.jar %user_input%

echo.
pause
rem The `pause` command keeps the command prompt window open until you press a key.
