SHELL := /bin/bash
PROJECT = graph2c

LOC_INC_DIR = include
LOC_SRC_DIR = src
SIA_LANG_DIR = streamix-sia-lang

SOURCES = main.c \
		  $(LOC_SRC_DIR)/* \
		  $(SIA_LANG_DIR)/$(LOC_SRC_DIR)/*

INCLUDES = $(LOC_INC_DIR)/* \
		   $(SIA_LANG_DIR)/$(LOC_INC_DIR)/*

INCLUDES_DIR = -I/usr/include/igraph \
			   -I$(LOC_INC_DIR) \
			   -I$(SIA_LANG_DIR)/$(LOC_INC_DIR) \
			   -I$(SIA_LANG_DIR)/uthash/src \
			   -I.

LINK_DIR = -L/usr/local/lib

LINK_FILE = -ligraph \
			-lzlog

CFLAGS = -Wall
DEBUG_FLAGS = -g -O0

CC = gcc

all: $(PROJECT)

# compile with dot stuff and debug flags
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(PROJECT)

# compile project
$(PROJECT): $(SOURCES) $(INCLUDES)
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDES_DIR) $(LINK_DIR) $(LINK_FILE) -o $(PROJECT)

.PHONY: clean install

install:
	mkdir -p /usr/local/bin
	cp -a $(PROJECT) /usr/local/bin/.

clean:
	rm -f $(PROJECT)
