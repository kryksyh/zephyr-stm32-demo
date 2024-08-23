Output bin-mode.mp4

Require echo

Set Shell "bash"
Set FontSize 26
Set Width 1200
Set Height 600
Set Theme "Dracula+"

Type "# Hi there!" Sleep 2s Enter
Type "# 'app toggle' switches the device to binary mode" Sleep 500ms Enter
Type "# It uses Google Protobuf for communications" Sleep 1s Enter
Type "# The message format is defined in 'proto/message.proto' " Sleep 1s Enter
Sleep 3s Enter Sleep 1s
Type "# To properly test binary comms," Sleep 500ms Enter
Type "# you would need a dedicated program" Sleep 500ms Type "." Sleep 500ms Type "." Sleep 500ms Type "." Sleep 500ms Enter
Type "# But let's try some command-line tricks" Sleep 500ms Enter

Hide
  Type "tmux -f /dev/null -L test new-session -- bash" Enter
  Type "tmux split-window -d -v -- bash && \" Enter
  Type "tmux set status && \" Enter
  Type 'tmux setw pane-border-style "fg=0" && \' Enter
  Type 'tmux setw pane-active-border-style "fg=0"' Enter
  Type `export PS1="> "` Enter
  Sleep 0.5
  Ctrl+L
  Ctrl+B Down
  Type `export PS1="> "` Enter
  Ctrl+L
  Ctrl+B Up
  Sleep 1
Show

Sleep 2s
Type "# At the top, we will send a message" Sleep 500ms Enter Sleep 2s
Ctrl+B Down
Type "# And at the, bottom we will receive the response" Sleep 500ms Enter Sleep 2s
Sleep 2s

Ctrl+B Up
Type "# Let's read the temperature from sensor 0" Sleep .5 Enter
Type `echo -e "type: ReadData read_data:{sensor_id: 0}" |` Ctrl+V Ctrl+J Sleep 0.5
Type "  protoc --encode=Request message.proto > /dev/ttyACM0"

Ctrl+B Down
Sleep .5

Type "dd if=/dev/ttyACM0 bs=1 count=9 status=none |"  Ctrl+V Ctrl+J Sleep 0.5
Type "  protoc --decode Response message.proto"

Sleep 2
Enter
Ctrl+B Up
Sleep .5
Enter

Sleep 6s

Ctrl+L

Type "# Now, let's configure and enable sensor 1" Sleep .5 Enter

Type `echo -e "type: ConfigSensor config: ` Ctrl+V Ctrl+J Sleep 0.5
Type `        {sensor_id: 1 command: SetConfig interval: 100 }" |`  Ctrl+V Ctrl+J Sleep 0.5
Type "  protoc --encode=Request message.proto > /dev/ttyACM0" Sleep .5 Enter

Type `echo -e "type: ConfigSensor config: ` Ctrl+V Ctrl+J Sleep 0.5
Type `        {sensor_id: 1 command: Enable }" | ` Ctrl+V Ctrl+J Sleep 0.5
Type "  protoc --encode=Request message.proto > /dev/ttyACM0"  Sleep .5 Enter

Sleep 2s

Ctrl+L

Type "# Now, let's read its data" Sleep .5 Enter
Type `echo -e "type: ReadData read_data:{sensor_id: 1}" |` Ctrl+V Ctrl+J Sleep 0.5
Type "  protoc --encode=Request message.proto > /dev/ttyACM0"

Ctrl+B Down
Sleep .5

Type "dd if=/dev/ttyACM0 bs=1 count=11 status=none |"  Ctrl+V Ctrl+J Sleep 0.5
Type "protoc --decode Response message.proto"

Sleep 2
Enter
Ctrl+B Up
Sleep .5
Enter
Sleep 6

Type "# And to return back to text mode," Sleep .5 Enter
Type `echo -e "type: Toggle" | ` Ctrl+V Ctrl+J Sleep 0.5
Type "protoc --encode=Request message.proto > /dev/ttyACM0" Sleep .5

Ctrl+B Down

Type "cat /dev/ttyACM0" Sleep .5 Enter

Ctrl+B Up

Sleep .5 Enter
Sleep 20s
