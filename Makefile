upload:
	platformio run --target upload

monitor:
	platformio device monitor -b 115200 -f colorize
