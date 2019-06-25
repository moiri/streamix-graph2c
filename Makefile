SHELL := /bin/bash
PROJECT = smxrtsp

LOC_INC_DIR = include
LOC_SRC_DIR = src
SIA_LANG_DIR = streamix-sia-lang

TARGET_PATH = /opt/smx

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

LINK_FILE = -ligraph

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
	mkdir -p $(TARGET_PATH)/bin
	mkdir -p $(TARGET_PATH)/tpl
	cp -a $(PROJECT) $(TARGET_PATH)/bin/.
	cp -a tpl $(TARGET_PATH)/

clean:
	rm -f $(PROJECT)
