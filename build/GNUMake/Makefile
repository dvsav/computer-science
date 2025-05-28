# To build the project successfully, when launching GNU Make,
# you may need to specify manually the following arguments:
# path to the compiler:  CXX="/path/to/compiler".
#
# For target 'test' you can specify the name(s) of a specific test(s) to run with target 'test' like this:
# make test RUN_TEST=TestCaseName
# https://github.com/catchorg/Catch2/blob/v2.x/docs/command-line.md#specifying-which-tests-to-run

PROJDIR = $(CURDIR)/../..
SRC_DIR = $(PROJDIR)/src
BIN_DIR = $(CURDIR)/bin
TEST_DIR = $(PROJDIR)/test

# Determine environment in which this makefile is being executed
# (e.g. Cygwin, MSYS, Linux, etc.)
UNAME = $(shell uname)
$(info Environment: $(UNAME))

# installation directory (used by 'install' target, can be specified in make command line)
# example: sudo make install DESTDIR=/usr/local/bin
DESTDIR ?= /usr/local/bin

# You can pass variable to GNU make like this: make VERBOSE=yes
ifndef VERBOSE
    QUIET:=@
endif

# You can pass variable to GNU make like this: make DEBUG=yes
ifdef DEBUG
    # add debug information
    CXXFLAGS+="-ggdb"
    # disable optimizations at compile time
    CXXFLAGS+="-O0"
    # specify the debugging tool
    DEBUGGER="gdb --args"
endif

# '-Wall' enables all the warnings about constructions that some users consider questionable, and that are easy to avoid
# '-Wextra' enables some extra warning flags that are not enabled by '-Wall'
CXXFLAGS+=-Wall -Wextra

# Enable exception handling. Generates extra code needed to propagate exceptions.
CXXFLAGS+=-fexceptions

# Cygwin specific part
ifeq ($(findstring CYGWIN,$(UNAME)),CYGWIN)
	# This is to overcome compiler issue:
	# too many sections
	# Assembler messages: can't write bytes to section .text of test.o: 'file too big'
	CXXFLAGS+=-Wa,-mbig-obj
endif

# Linking to libstdc++fs is required for use of the Filesystem library extensions in <experimental/filesystem>
STDLIBS= \
    -lstdc++fs

# extract GCC/CLANG (whichever compiler you use) standard libraries' search paths (may be necessary to launch the built executable)
STDLIBDIRS = $(shell $(CXX) -print-search-dirs | sed -n 's/\(^libraries: =\)\(.*\)/\2/p')

TEST= \
    $(BIN_DIR)/test.exe

OBJECTS= \
    $(BIN_DIR)/test.o \
    $(BIN_DIR)/utility.o

INSTALL_FILES= \
    $(SRC_DIR)/*.h \
	$(SRC_DIR)/*.cpp

# Create directory if the directory part of the file name of the target, with the trailing slash removed
DIR_GUARD=@mkdir -p $(@D)

# GNU Make automatic variables (https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html)
# $@ - target
# $(@D) - The directory part of the file name of the target, with the trailing slash removed. If the value of $@ is dir/foo.o then $(@D) is dir
# $^ - prerequisites
# $< - first prerequisite
# $* - the text matching the % in the pattern in the target name
# %  - matches any sequence of characters

# 1st target is the default one (which is built if no targets are explicitly specified for make)
all : $(TEST)

# pattern rule (matches multiple possible targets)
# compiles all source files in the source directory
$(BIN_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(DIR_GUARD)
	@echo -e "\nCompiling... $<"
	$(QUIET)$(CXX) -c $(CXXFLAGS) $< -o $@

# links object files into the unit test executable (-Wl,-rpath embeds library search paths into the executable)
$(TEST) : $(OBJECTS)
	@echo -e "\nLinking... $^"
	$(QUIET)$(CXX) -o $@ $(CXXFLAGS) $^ $(STDLIBS) -Wl,-rpath,"$(STDLIBDIRS)"

# runs unit tests
# cd $(BIN_DIR) is done to use the same relative path to 'tests' directory as in Visual Studio
test : $(TEST)
	@echo -e "\n(You may specify test case name using RUN_TEST parameter)"
	$(QUIET)cd $(TEST_DIR) && $(DEBUGGER) $(BIN_DIR)/test.exe $(RUN_TEST)

# 'install' target copies specified INSTALL_FILES to DESTDIR directory.
# 'install' command is mostly similar to 'cp' but provides additionally the option -m to
# set the destination file properties directly without having to use chmod separately.
install :
	@echo -e "\n(You may specify installation directory using DESTDIR parameter)"
	@echo -e "\nInstalling $^ to $(DESTDIR)"
	$(QUIET)[ -d $(DESTDIR) ] || mkdir -p $(DESTDIR)
	$(QUIET)install $(INSTALL_FILES) $(DESTDIR)

# cleans up output directory
clean :
	cd $(BIN_DIR) && rm -f *.o *.exe *.sh

# a phony target is one that is not really the name of a file
# a target can be marked as phony to avoid a conflict with a file of the same name
.PHONY : all clean test install
