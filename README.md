## KVMKit, a UART --> HID Keyboard/Mouse Adapter

### Project:
Half of a diy USB-KVM solution for laptops, pairing up with a cheap HDMI-USB capture card. This project implements a USB keyboard & mouse interface, controlled by a second USB host (USB-UART). A desktop application running on the Controller (laptop) shows the video feed from the capture card and sends clicks & keystrokes to the HID bridge over UART to be forwarded to the Target as simulated keyboard/mouse.

From the Target's perspective, the bridge is an HID device supporting keyboard and mouse profiles, so it should work with default OS HID drivers.

From the Controller's perspective, the bridge takes HID keystrokes/mouse states over UART console endpoints, or takes text and converts to HID keycodes.

### Implementation Notes:
#### Component Status:
#### USB-HID Bridge:
STM32f7 nucleo board dev environment
USB-UART adapter breadboarded to UART4
No HID mouse support yet
#### HDMI-USB:
OTS (for now?)
#### Hardware:
Dedicated board planned, likely stm32l4
Dual USB-C/micro-B, power from Controller USB
Oboard hub option for integrating HDMI capture later?
#### Desktop App:
TBD, want to nail down and test UART console API first


### Notes/credits:
Console adapted from https://github.com/eleciawhite/reusable
	Note that spaces and newlines can't be escaped or quoted within the console. Spaces will need a different escape char (like
	modifier keys) and newlines will just have to each be their own command (escape at EOL will mean newline)
