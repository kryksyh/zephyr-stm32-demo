Output text-mode.mp4

Require echo

Set Shell "bash"
Set FontSize 26
Set Width 1200
Set Height 600
Set Theme "Dracula+"


Type "# Hi there!" Sleep 2s Enter
Type "# You can connect to the device using any terminal app," Sleep 500ms Enter
Type "# although minicom works best" Sleep 1s Enter
Type "# Let's use minicom to log into the device" Sleep 1s Enter
Type "minicom -c on -D /dev/ttyACM0" Sleep 2s Enter
Sleep 1s
Ctrl+L
Sleep 3s
Type "Zephyr's shell supports a lot of useful commands" Sleep 2s Ctrl+C
Type "Feel free to check them out with `help`" Sleep 2s Ctrl+C Ctrl+C
Type "All custom commands for this demo" Sleep 500ms Ctrl+C
Type "are under the 'app' command" Sleep 1s Ctrl+C Sleep 500ms

Type "app" Sleep 1s Enter
Sleep 8s Ctrl+L

Type "To read sensor data, use 'app read <sensor id>'" Sleep 2s Ctrl+C  Sleep 500ms
Type "app read 0" Sleep 2s Enter
Type "You can read multiple sensors at once" Ctrl+C  Sleep 500ms
Type "app read 0 1 2" Sleep 2s Enter
Type "Wait, why sensors are offline? Let's enable them" Sleep 2s Ctrl+C  Sleep 500ms
Type "app enable 1 2" Sleep 2s Enter
Type "If the sensor has an incorrect config it can't be enabled" Sleep 2s Ctrl+C  Sleep 500ms
Type "Let's configure it" Sleep 2s Ctrl+C  Sleep 500ms
Type "The command is 'app config <id> <interval> <off> <cf>" Sleep 2s Ctrl+C  Sleep 500ms
Type "Offset and coefficient are calibrations 't = T*k + b'" Sleep 2s Ctrl+C  Sleep 500ms
Type "app config 1 100 0 1" Sleep 2s Enter  Sleep 500ms
Type "app enable 1" Sleep 2s Enter Sleep 500ms
Type "app read 1" Sleep 2s Enter Sleep 500ms

Sleep 5s Ctrl+L

Type "And the last one, to switch to binary mode"  Sleep 2s Ctrl+C  Sleep 500ms
Type "Use 'app toggle' command"  Sleep 2s Ctrl+C  Sleep 500ms
Type "app toggle"  Sleep 2s Enter Sleep 500ms
Sleep 8s

Hide
Ctrl+A Type "q"
Sleep 1s
Enter
Show

Type "# That's it for the text mode showcase" Sleep 15s Enter
