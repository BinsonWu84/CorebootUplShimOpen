BASE_NAME = ParseLib
SOURCE_DIR = $(WORKSPACE)/Library/ParseLib
OUTPUT_DIR = $(WORKSPACE)/../Build/Library/ParseLib/OUTPUT
DEBUG_DIR = $(WORKSPACE)/../Build/Library/ParseLib/DEBUG

#
# Shell Command Macro
#
CP = cp -p -f
MV = mv -f
RM = rm -f
MD = mkdir -p
RD = rm -r -f

CC_BUILDRULEFAMILY =  CLANGGCC
CC_FLAGS = -g -Os -fshort-wchar -fno-builtin -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -fno-common -ffunction-sections -fdata-sections -Wno-parentheses-equality -Wno-tautological-compare -Wno-tautological-constant-out-of-range-compare -Wno-empty-body -Wno-unused-const-variable -Wno-varargs -Wno-unknown-warning-option -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-protector -mms-bitfields -Wno-address -Wno-shift-negative-value -Wno-unknown-pragmas -Wno-incompatible-library-redeclaration -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -msoft-float -mno-implicit-float -ftrap-function=undefined_behavior_has_been_optimized_away_by_clang -funsigned-char -fno-ms-extensions -Wno-null-dereference -m32 -Oz -flto -march=i586 -target i686-pc-linux-gnu -g -D DISABLE_NEW_DEPRECATED_INTERFACES
CC = clang

MAKE = make

OBJCOPY_ADDDEBUGFLAG =  --add-gnu-debuglink=$(DEBUG_DIR)/$(MODULE_NAME).debug
OBJCOPY_BUILDRULEFAMILY =  CLANGGCC
OBJCOPY_FLAGS = 
OBJCOPY = echo
OBJCOPY_STRIPFLAG =  --strip-unneeded -R .eh_frame

SLINK_BUILDRULEFAMILY =  CLANGGCC
SLINK = llvm-ar

MAKE_FILE = $(WORKSPACE)/GNUmakefile

#
# Build Macro
#
OBJECT_FILES =  \
    $(OUTPUT_DIR)/ParseLib.o

#
# Overridable Target Macro Definitions
#
INIT_TARGET = init
CODA_TARGET = $(OUTPUT_DIR)/ParseLib.lib \
              

#
# Default target, which will build dependent libraries in addition to source files
#

all: mbuild

#
# ModuleTarget
#

mbuild: $(INIT_TARGET) $(CODA_TARGET)

#
# Initialization target: print build information and create necessary directories
#
init: info dirs

info:
	-@echo Building $(BASE_NAME) ...
	-@echo SOURCE_DIR $(SOURCE_DIR)
	-@echo OUTPUT_DIR $(OUTPUT_DIR)
	-@echo DEBUG_DIR $(DEBUG_DIR)
	-@echo INC $(INC)

dirs:
	-@$(MD) $(DEBUG_DIR)
	-@$(MD) $(OUTPUT_DIR)

#
# Individual Object Build Targets
#
$(OUTPUT_DIR)/ParseLib.o : $(SOURCE_DIR)/ParseLib.c
	"$(CC)" $(DEPS_FLAGS) $(CC_FLAGS) -c -o $(OUTPUT_DIR)/ParseLib.o $(INC) $(SOURCE_DIR)/ParseLib.c

$(OUTPUT_DIR)/ParseLib.lib : $(OBJECT_FILES)
	$(RM) $(OUTPUT_DIR)/ParseLib.lib
	-@echo "$(SLINK)" cr $(OUTPUT_DIR)/ParseLib.lib $(OBJECT_FILES)
	"$(SLINK)" cr $(OUTPUT_DIR)/ParseLib.lib $(OBJECT_FILES)



#
# clean all intermediate files
#
clean:
	$(RD) $(OUTPUT_DIR)
		$(RM) AutoGenTimeStamp

#
# clean all generated files
#
cleanall:
	$(RD) $(DEBUG_DIR)
	$(RD) $(OUTPUT_DIR)
	$(RM) *.pdb *.idb > NUL 2>&1
	$(RM) $(BIN_DIR)/$(MODULE_NAME).efi
	$(RM) AutoGenTimeStamp


