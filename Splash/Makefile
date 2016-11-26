TARGET = Splash
OBJS = mp3player.o graphics.o framebuffer.o main.o

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = -lpspgu -lpng -lz -lm -lmad -lpspaudiolib -lpspaudio -lpsppower 
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Title Screen.
PSP_EBOOT_ICON = bsg_eboot_icon.png
PSP_EBOOT_PIC1 = bsg_eboot_bg.png

PSPSDK=/usr/local/pspdev/psp/sdk
include $(PSPSDK)/lib/build.mak
