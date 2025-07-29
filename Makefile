CC=g++
IN=src/main.cpp
BIN=xrp-botpp
OUT=$(BIN)

CUSTOM_MACROS=-DVERSION=\"v1.0.0-alpha\" -DPROGRAM_NAME=\"$(OUT)\" -DCC=\"$(CC)\"

WARNINGS=-Wall -Wextra -Wformat
LINK=-lTgBot -lcurl -lssl -lcrypto -lfmt #-lboost_system#-lpthread  
INCLUDES=-I/usr/local/include lib/json.hpp
MACROS=-DHAVE_CURL $(CUSTOM_MACROS)
FLAGS=--std=c++14 $(MACROS) $(INCLUDES) $(LINK) $(WARNINGS)

COMP_FLAGS=$(IN) -o $(OUT) $(FLAGS)


compile:
	$(CC) $(COMP_FLAGS)

release: OUT=./pkg/$(BIN)
release:
	@ mkdir -p pkg
	$(CC) $(COMP_FLAGS) -s -Ofast


get:
	@echo retreiving latest copy of 'nlohmann/json'
	wget "https://github.com/nlohmann/json/releases/latest/download/json.hpp" -O lib/json.hpp

clean:
	rm $(OUT) ./pkg/* 2> /dev/null
