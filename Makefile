#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment.")
endif

TARGET		:= MusicMaker
BUILD		:= build
SOURCES		:= source
INCLUDES	:= include

PREFIX		:= arm-none-eabi-
CC			:= $(PREFIX)gcc
CXX			:= $(PREFIX)g++

export CC
export CXX

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:= $(CURDIR)/$(TARGET)

export VPATH	:= $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:= $(CURDIR)/$(BUILD)

LIBDIRS		:= $(DEVKITPRO)/libnds $(DEVKITPRO)/calico

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:= $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export ARCH		:= -mthumb -mthumb-interwork

export CFLAGS	:= -g -Wall -O2 \
					-march=armv5te -mtune=arm946e-s \
					-fomit-frame-pointer -ffast-math \
					$(ARCH) $(INCLUDE) \
					-DARM9 -D__ARM9__ -D__NDS__

export CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

export ASFLAGS	:= -g $(ARCH)
export LDFLAGS	:= -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
export LIBS		:= -Wl,--start-group -lnds9 -lcalico_ds9 -Wl,--end-group

CPPFILES	:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))

export OFILES	:= $(CPPFILES:.cpp=.o) $(CFILES:.c=.o)

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile all

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds

#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------

DEPENDS := $(OFILES:.o=.d)

all: $(OUTPUT).nds

$(OUTPUT).nds: $(OUTPUT).elf
	@echo packaging $(notdir $@)
	@ndstool -c $@ -9 $<

$(OUTPUT).elf: $(OFILES)
	@echo linking $(notdir $@)
	@$(CXX) $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

%.o: %.cpp
	@echo $(notdir $<)
	@$(CXX) -MMD -MP -MF $*.d $(CXXFLAGS) -c $< -o $@

%.o: %.c
	@echo $(notdir $<)
	@$(CC) -MMD -MP -MF $*.d $(CFLAGS) -c $< -o $@

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
