## Raspberry Pi Control

This is just a Tkinter-based GUI for controlling multiple Raspberry Pis which are running our usual experimental optogenetic protocols. It includes timers for each well and a history of commands issued. It's ugly but it works. Soon to be added: log of experiments and commands, video streaming. 

------
You can run it on OS X by just clicking the file in "dist"
------


Description of commands:

**Flashing Lights**: Specify a frequency and pulse width for regular and repeated pulses of light *ad infinitum*.

**Paired Pulse**: A protocol for producing two pulses of light of pre-determined duration and relative timing.

**Blocks**: (we don't really use this protocol anymore), but runs Flashing Lights but with a slower "envelope", i.e. there are periods in which it will periodically flash the lights like Flashing Lights and periods in which it won't turn the lights on at all


