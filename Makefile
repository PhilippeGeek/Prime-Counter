# Copyright (C) 2015 Gabriel Augendre <gabriel@augendre.info>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Defining the custom targets.
# If we don't specify these special targets using .PHONY, make won't run
# the clean target if a file named "clean" already exists,
# because it will be considered up to date.
.PHONY: clean, cleanall, run, create_build_path

# Deactivating implicit rules.
# If we don't put this directive, make will search itself how to create
# the files he doesn't know (.o for example).
# This directive sort of activates a manual mode :
# Do only what I'm telling you, nothing from your initiative.
.SUFFIXES:

# Variables definition
CC = gcc
DEBUG = no
BUILD_PATH = build/
SOURCE_PATH = src/
EXEC = $(BUILD_PATH)main.out
STD = -std=gnu89

OBJECTS = $(BUILD_PATH)main.o $(BUILD_PATH)functions.o

# Conditionnal definition
# syntax : ifeq ($(VAR),value)
ifeq ($(DEBUG),yes)
	CFLAGS = $(STD) -g -W -Wall
	MESSAGE = Génération en mode debug
else
	CFLAGS = $(STD)
	MESSAGE = Génération en mode release
endif

# all
# $(EXEC) is a dependency of all, as well as create_build_path.
# make will so search for some rules mathing the dependencies names and will run them before.
# The @ before the command line mutes it. It is still ran but the command itself isn't printed.
all: create_build_path $(EXEC)
	@echo $(MESSAGE)

# EXEC rule says it has .o files in dependency.
# make searches for a matching rule, then runs it.
# $^ is a variable corresponding to all dependencies of the target.
$(EXEC): $(OBJECTS)
	@$(CC) $^ -o $(EXEC)
	@echo Generating executable file $@
	@echo

# "Make me .o with corresponding .c"
# $< is the first dependency name.
# $@ is the target name.
$(BUILD_PATH)%.o: $(SOURCE_PATH)%.c
	@$(CC) -c $< -o $@ $(CFLAGS)
	@echo Generating object file $@

# Cleans generated files
clean:
	rm -rf $(BUILD_PATH)*.o

# cleanall is clean + remove executable
cleanall: clean
	rm -rf $(EXEC)

# Creates build path
create_build_path:
	@mkdir -p $(BUILD_PATH)

# run will compile and run the project
run: all
	@echo
	@echo Running
	@echo ===================================
	@$(EXEC)
