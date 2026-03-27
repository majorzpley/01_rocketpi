.PHONY: all build ota app upload upload-seq upload-merge clean

all: build upload

build:
	pio run -e ota
	pio run -e app

ota:
	pio run -e ota

app:
	pio run -e app

upload-seq: build
	pio run -e ota -t upload
	sleep 1
	pio run -e app -t upload

upload: upload-seq

clean:
	pio run -e ota -t clean
	pio run -e app -t clean
	pio run -t clean