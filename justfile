DEFAULT_BUILD_DIR := "build"
DEFAULT_SERIAL_PORT := "/dev/ttyACM0"
DEFAULT_EXTRA_CMAKE_ARGS := "-j 8"

build *extra_args:
	cmake -S . -B "{{DEFAULT_BUILD_DIR}}" && cmake --build "{{DEFAULT_BUILD_DIR}}" {{DEFAULT_EXTRA_CMAKE_ARGS}} {{extra_args}}

[doc('Upload UF2 to pico. Takes optional directory parameter')]
upload directory="{{DEFAULT_BUILD_DIR}}":
	picotool load -f "./{{directory}}/demo_bot.uf2"

monitor port="{{DEFAULT_SERIAL_PORT}}":
	picocom "{{port}}"
