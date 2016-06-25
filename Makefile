SHELL := /bin/bash
PROJECT = gml2c

LOC_INC_DIR = include
LOC_SRC_DIR = src

SOURCES = main.c \
		  $(LOC_SRC_DIR)/*

INCLUDES = $(LOC_INC_DIR)/*

INCLUDES_DIR = -I/usr/local/include/igraph \
			   -I$(LOC_INC_DIR) \
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
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDES_DIR) $(LINK_DIR) $(LINK_FILE) -o $(PROJECT).out
