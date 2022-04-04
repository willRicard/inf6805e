all: build/main.bo build/main.bdb

build/main.bo build/main.bdb:
	bzzc src/main.bzz -b build/main.bo -d build/main.bdb
