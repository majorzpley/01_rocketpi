.PHONY: all build bootloader app upload upload-seq upload-merge clean

all: build upload

build:
	pio run -e bootloader
	pio run -e app

bootloader:
	pio run -e bootloader

app:
	pio run -e app

upload-seq: build
	pio run -e bootloader -t upload
	sleep 1
	pio run -e app -t upload

upload: upload-seq

clean:
	pio run -e bootloader -t clean
	pio run -e app -t clean
	pio run -t clean