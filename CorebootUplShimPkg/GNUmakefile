WORKSPACE=$(shell pwd)

BASE_NAME        = ShimLayer
BUILD_DIR        = $(WORKSPACE)/../Build
SOURCE_DIR       = $(WORKSPACE)/ShimLayer
OUTPUT_DIR 	     = $(WORKSPACE)/../Build/ShimLayer/OUTPUT
DEBUG_DIR        = $(WORKSPACE)/../Build/ShimLayer/DEBUG
MEMBASE          = 0x800000
MEMSIZE          = 0x100000
UEFI_REGION_SIZE = 0x04000000

#
# Module Macro Definition
#
IMAGE_ENTRY_POINT = _ModuleEntryPoint

#
# Shell Command Macro
#
CP = cp -p -f
MV = mv -f
RM = rm -f
MD = mkdir -p
RD = rm -r -f

#
# Tools definitions specific to this module
#
CC_BUILDRULEFAMILY =  CLANGGCC
# CC_FLAGS = -g -Os -fshort-wchar -fno-builtin -fno-strict-aliasing -Wall -Werror -Wno-array-bounds -fno-common -ffunction-sections -fdata-sections -DSTRING_ARRAY_NAME=$(BASE_NAME)Strings -Wno-parentheses-equality -Wno-tautological-compare -Wno-tautological-constant-out-of-range-compare -Wno-empty-body -Wno-unused-const-variable -Wno-varargs -Wno-unknown-warning-option -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-stack-protector -mms-bitfields -Wno-address -Wno-shift-negative-value -Wno-unknown-pragmas -Wno-incompatible-library-redeclaration -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -msoft-float -mno-implicit-float -ftrap-function=undefined_behavior_has_been_optimized_away_by_clang -funsigned-char -fno-ms-extensions -Wno-null-dereference -m32 -Oz -flto -march=i586 -target i686-pc-linux-gnu -g -D DISABLE_NEW_DEPRECATED_INTERFACES
CC_FLAGS = -march=i586 -target i686-pc-linux-gnu
CC = clang

MAKE = make

OBJCOPY_ADDDEBUGFLAG =  --add-gnu-debuglink=$(DEBUG_DIR)/$(MODULE_NAME).debug
OBJCOPY_BUILDRULEFAMILY =  CLANGGCC
OBJCOPY = echo
OBJCOPY_STRIPFLAG =  --strip-unneeded -R .eh_frame

SLINK_BUILDRULEFAMILY =  CLANGGCC
SLINK = llvm-ar

NASM_BUILDRULEFAMILY =  CLANGGCC
NASM_FLAGS = -f elf32
NASM = nasm

DLINK_BUILDRULEFAMILY =  CLANGGCC
# DLINK_FLAGS = -nostdlib -Wl,-q,--gc-sections -z max-page-size=0x40 -Wl,--entry,$(IMAGE_ENTRY_POINT) -u $(IMAGE_ENTRY_POINT) -Wl,-Map,$(DEBUG_DIR)/$(BASE_NAME).map,--whole-archive -flto -Wl,-O3 -Wl,-melf_i386 -Wl,--oformat,elf32-i386
DLINK_FLAGS = -nostdlib -Wl,-q,--gc-sections -z max-page-size=0x40 -Wl,--entry,$(IMAGE_ENTRY_POINT) -u $(IMAGE_ENTRY_POINT) -Wl,--whole-archive -flto -Wl,-O3 -Wl,-melf_i386 -Wl,--oformat,elf32-i386
DLINK = clang

DLINK2_BUILDRULEFAMILY =  CLANGGCC
DLINK2_FLAGS = -Wl,--defsym=PECOFF_HEADER_SIZE=0x800000 -Wl,--script=$(SOURCE_DIR)/ClangBase.lds -O3 -fuse-ld=lld

MAKE_FILE = $(WORKSPACE)/GNUmakefile

#
# Build Macro
#
STATIC_LIBRARY_FILES =  \
  $(BUILD_DIR)/Library/BaseLib/OUTPUT/BaseLib.lib \
  $(BUILD_DIR)/Library/HobLib/OUTPUT/HobLib.lib \
  $(BUILD_DIR)/Library/ParseLib/OUTPUT/ParseLib.lib \
  $(BUILD_DIR)/Library/ElfLoaderLib/OUTPUT/ElfLoaderLib.lib \
  $(BUILD_DIR)/Library/LzmaCustomDecompressLib/OUTPUT/LzmaDecompressLib.lib \
  $(OUTPUT_DIR)/ShimLayer.lib

OBJECT_FILES =  \
  $(OUTPUT_DIR)/CpuId.o \
  $(OUTPUT_DIR)/ShimLayer.o

INC =  \
  -I$(WORKSPACE)/../Include \
  -I$(WORKSPACE)/ShimLayer \
  -I$(WORKSPACE) \
  -I$(WORKSPACE)/Include \
  -I$(WORKSPACE)/Library/ElfLoaderLib \
  -I$(WORKSPACE)/Library/ElfLoaderLib/ElfLib \
  -I$(WORKSPACE)/Library/LzmaCustomDecompressLib \
  -I$(WORKSPACE)/Library/BaseLib \
  -I$(WORKSPACE)/Library/HobLib \
  -I$(WORKSPACE)/Library/ParseLib 

NASM_INC =  \
  -I$(WORKSPACE)/../Include

MAKE_FLAGS = INC="$(INC)" WORKSPACE=$(WORKSPACE)

#
# Overridable Target Macro Definitions
#
INIT_TARGET = init
CODA_TARGET = $(DEBUG_DIR)/ShimLayer.elf \

#
# Default target, which will build dependent libraries in addition to source files
#

all: mbuild

#
# ModuleTarget
#

#mbuild: init gen_libs $(CODA_TARGET)
mbuild: init gen_libs $(CODA_TARGET) install 

#
# Initialization target: print build information and create necessary directories
#
init: info dirs 

info:
	-@echo Building $(BASE_NAME) ...
	-@echo SOURCE_DIR $(SOURCE_DIR)
	-@echo OUTPUT_DIR $(OUTPUT_DIR)
	-@echo DEBUG_DIR $(DEBUG_DIR)

dirs:
	-@$(MD) $(DEBUG_DIR)
	-@$(MD) $(OUTPUT_DIR)

#
# GenLibsTarget
#
gen_libs:
	$(MAKE) $(MAKE_FLAGS) -f $(WORKSPACE)/Library/BaseLib/GNUmakefile
	$(MAKE) $(MAKE_FLAGS) -f $(WORKSPACE)/Library/HobLib/GNUmakefile
	$(MAKE) $(MAKE_FLAGS) -f $(WORKSPACE)/Library/ParseLib/GNUmakefile
	$(MAKE) $(MAKE_FLAGS) -f $(WORKSPACE)/Library/LzmaCustomDecompressLib/GNUmakefile
	$(MAKE) $(MAKE_FLAGS) -f $(WORKSPACE)/Library/ElfLoaderLib/GNUmakefile

#
# Individual Object Build Targets
#
$(DEBUG_DIR)/ShimLayer.elf : $(STATIC_LIBRARY_FILES)
	"$(DLINK)" -o $(DEBUG_DIR)/ShimLayer.elf $(DLINK_FLAGS) -Wl,--start-group,$(STATIC_LIBRARY_FILES) -Wl,--end-group $(CC_FLAGS) $(DLINK2_FLAGS)
	"$(OBJCOPY)" $(OBJCOPY_FLAGS) $(DEBUG_DIR)/ShimLayer.elf
  
$(OUTPUT_DIR)/ShimLayer.o : $(SOURCE_DIR)/ShimLayer.c
	"$(CC)" $(DEPS_FLAGS) $(CC_FLAGS) -c -o $(OUTPUT_DIR)/ShimLayer.o -DMEMBASE=$(MEMBASE) -DMEMSIZE=$(MEMSIZE)  -DUEFI_REGION_SIZE=$(UEFI_REGION_SIZE)  $(INC) $(SOURCE_DIR)/ShimLayer.c

$(OUTPUT_DIR)/CpuId.o : $(SOURCE_DIR)/CpuId.iii
	"$(NASM)" $(NASM_INC) $(NASM_FLAGS) -o $(OUTPUT_DIR)/CpuId.o $(SOURCE_DIR)/CpuId.iii

$(OUTPUT_DIR)/ShimLayer.lib : $(OBJECT_FILES)
	$(RM) $(OUTPUT_DIR)/ShimLayer.lib
	"$(SLINK)" cr $(OUTPUT_DIR)/ShimLayer.lib $(SLINK_FLAGS) $(OBJECT_FILES)

#
# install elf file
#
install:
	cp -f $(DEBUG_DIR)/ShimLayer.elf $(BUILD_DIR)/ShimLayer.elf

#
# clean all intermediate files
#
clean:
	$(RD) $(OUTPUT_DIR)

#
# clean all generated files
#
cleanall:
	$(RD) $(DEBUG_DIR)
	$(RD) $(OUTPUT_DIR)
	$(RD) $(BUILD_DIR)/Library/BaseLib/OUTPUT
	$(RD) $(BUILD_DIR)/Library/HobLib/OUTPUT
	$(RD) $(BUILD_DIR)/Library/ParseLib/OUTPUT
	$(RD) $(BUILD_DIR)/Library/ElfLoaderLib/OUTPUT
	$(RD) $(BUILD_DIR)/Library/LzmaCustomDecompressLib/OUTPUT
