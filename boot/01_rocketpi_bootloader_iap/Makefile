.PHONY: all build bootloader app1 app2 upload upload-seq upload-seq-app1 upload-seq-bootloader upload-seq-app2 clean

all: build upload

build:
	pio run -e bootloader
	pio run -e app1
	pio run -e app2

bootloader:
	pio run -e bootloader

app1:
	pio run -e app1

app2:
	pio run -e app2

upload-seq: build
	pio run -e bootloader -t upload
	sleep 1
	pio run -e app1 -t upload
	sleep 1
	pio run -e app2 -t upload

upload-seq-app1: build
	pio run -e bootloader -t upload
	sleep 1
	pio run -e app1 -t upload

upload-seq-bootloader:
	pio run -e bootloader -t upload


upload-seq-app2: build
	pio run -e bootloader -t upload
	sleep 1
	pio run -e app2 -t upload

upload: upload-seq

clean:
	pio run -e bootloader -t clean
	pio run -e app1 -t clean
	pio run -e app2 -t clean
	pio run -t clean