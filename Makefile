upload:
	platformio run --target upload

monitor:
	platformio device monitor -b 115200 -f send_on_enter -f printable --echo
