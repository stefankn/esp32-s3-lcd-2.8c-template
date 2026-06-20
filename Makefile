.PHONY: build compiledb upload monitor run clean

BAUD ?= 115200

build:
	pio run

compiledb:
	pio run -t compiledb

upload:
	pio run -t upload

monitor:
	pio device monitor --baud $(BAUD)

run:
	pio run -t upload && pio device monitor --baud $(BAUD)

clean:
	pio run -t clean
