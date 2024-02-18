PATH_SRC   := src
PATH_BUILD := build
PATH_BIN   := $(PATH_BUILD)/bin
PATH_OBJ   := $(PATH_BUILD)/obj
PATH_DEP   := $(PATH_OBJ)/__dep__

CC        = g++
CXXFLAGS += -Wall -pedantic -g
CC        = $(CXX) $(CXXFLAGS)
CPPFLAGS += -std=c++17

MKDIR  = mkdir -p
RM     = rm -f
RMDIR  = rm -rf
COPY   = cp
MOVE   = mv -f
RENAME = mv -f

NULL_DEVICE = /dev/null

EXE_COMPANY   := company.out
EXE_FINANCIAL := financial.out
EXE_BUILDING  := building.out
EXE_METER     := meter.out

#----------------------------------------

VPATH = $(PATH_SRC)

FILES   = $(patsubst src/%, %, $(shell find $(PATH_SRC) -name "*.cpp" -type f))
FOLDERS = $(patsubst src/%, %, $(shell find $(PATH_SRC) -mindepth 1 -type d))

FILES_NOMAIN = $(filter-out company_entry.cpp financial_entry.cpp building_entry.cpp meter_entry.cpp, $(FILES))

FILES_DEP = $(patsubst %, $(PATH_DEP)/%.d, $(basename $(FILES)))
FILES_OBJ = $(patsubst %, $(PATH_OBJ)/%.o, $(basename $(FILES_NOMAIN)))

#----------------------------------------

all: $(PATH_BIN)/$(EXE_COMPANY) $(PATH_BIN)/$(EXE_FINANCIAL) $(PATH_BIN)/$(EXE_BUILDING) $(PATH_BIN)/$(EXE_METER)

$(PATH_BIN)/$(EXE_COMPANY): $(PATH_OBJ)/company_entry.o $(FILES_OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(PATH_BIN)/$(EXE_FINANCIAL): $(PATH_OBJ)/financial_entry.o $(FILES_OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(PATH_BIN)/$(EXE_BUILDING): $(PATH_OBJ)/building_entry.o $(FILES_OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(PATH_BIN)/$(EXE_METER): $(PATH_OBJ)/meter_entry.o $(FILES_OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@


DEPFLAGS    = -MT $@ -MMD -MP -MF $(PATH_DEP)/$*.dTMP
POSTCOMPILE = @$(MOVE) $(PATH_DEP)/$*.dTMP $(PATH_DEP)/$*.d > $(NULL_DEVICE) && touch $@

$(PATH_OBJ)/%.o: %.cpp
$(PATH_OBJ)/%.o: %.cpp $(PATH_DEP)/%.d | directories
	$(CC) $(CPPFLAGS) -c $(DEPFLAGS) $< -o $@
	$(POSTCOMPILE)

.PRECIOUS: $(FILES_DEP)
$(FILES_DEP): ;
-include $(FILES_DEP)

#----------------------------------------

directories: $(PATH_BUILD) $(PATH_BIN) $(PATH_OBJ) $(PATH_DEP) nested-folders
nested-folders: $(addprefix $(PATH_OBJ)/, $(FOLDERS)) $(addprefix $(PATH_DEP)/, $(FOLDERS))

$(PATH_BUILD): ; $(MKDIR) $@
$(PATH_BIN)  : ; $(MKDIR) $@
$(PATH_OBJ)  : ; $(MKDIR) $@
$(PATH_DEP)  : ; $(MKDIR) $@

$(addprefix $(PATH_OBJ)/, $(FOLDERS)): ; @$(MKDIR) $@
$(addprefix $(PATH_DEP)/, $(FOLDERS)): ; @$(MKDIR) $@

#----------------------------------------

.PHONY: all directories nested-folders \
		clean clean-obj clean-dep clean-exe delete-build \
		run help

clean: clean-obj clean-dep clean-exe
clean-obj   : ; $(RMDIR) $(PATH_OBJ)/*
clean-dep   : ; $(RMDIR) $(PATH_DEP)/* $(PATH_BIN)/fifos/* $(PATH_BIN)/logs/*
clean-exe   : ; $(RMDIR) $(PATH_BIN)/$(EXE_COMPANY) $(PATH_BIN)/$(EXE_FINANCIAL) $(PATH_BIN)/$(EXE_BUILDING) $(PATH_BIN)/$(EXE_METER)

ARGS ?=
run: ; @cd $(PATH_BIN) && ./$(EXE_COMPANY) $(ARGS)

help:
	@echo Targets: all clean clean-obj clean-dep clean-exe delete-build run
	@echo '(make run ARGS="arg1 arg2...")'