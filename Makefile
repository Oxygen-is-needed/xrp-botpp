CC=g++
IN=src/main.cpp
OUT=xrp-botpp

CUSTOM_MACROS=-DVERSION=\"v0.1.0\" -DPROGRAM_NAME=\"$(OUT)\" -DCC=\"$(CC)\"

WARNINGS=-Wall -Wextra
LINK=-lTgBot -lcurl -lssl -lcrypto -lfmt #-lboost_system#-lpthread  
INCLUDES=-I/usr/local/include lib/json.hpp
MACROS=-DHAVE_CURL $(CUSTOM_MACROS)
FLAGS= --std=c++14 $(MACROS) $(INCLUDES) $(LINK) $(WARNINGS)

COMP_FLAGS=$(IN) -o $(OUT) $(FLAGS)


compile:
	$(CC) $(COMP_FLAGS)

get:
	@echo retreiving latest copy of 'nlohmann/json'
	wget "https://github.com/nlohmann/json/releases/latest/download/json.hpp" -O lib/json.hpp

clean:
	rm $(OUT)
