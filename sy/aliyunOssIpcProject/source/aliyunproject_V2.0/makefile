LIB = -L./liball/libarm -lNetLib -loss_c_sdk -lvo-aacenc -laprutil-1 -lapr-1 -lcurl -lmxml -lmp4v2 -ldl -lrt -lpthread  -lm -Wl,--rpath=./liball/libarm
#LIB = -L./liball/libx86 -lNetLib -loss_c_sdk -lvo-aacenc -laprutil-1 -lapr-1 -lcurl -lmxml -lmp4v2 -ldl -lrt -lpthread -lm -Wl,--rpath=./liball/libx86

INCLUDES = -I./include\
		   -I./include/apr-1\
		   -I./include/ossSDK\
		   -I./include/mp4v2\
		   -I./include/curl\
		   -I./include/openssl\
		   -I./include/vo-aacenc
#关闭所有 未使用变量、printf格式化、警告  yehxm9xt
#FLAGS = -m32 -O2 -Wall -Wno-unused -Wno-format -fstrength-reduce -DHI_OS_LINUX
FLAGS = -g -O3 -Wall -Wno-unused -Wno-format -fstrength-reduce -DHI_OS_LINUX -DCPU_ARM
FLAGS += -g -rdynamic
ifeq ($(DECODEMP3TOOL),y)
LIB += -lmad
FLAGS += -DDECODEMP3TOOL
endif

#GXX = g++ $(INCLUDES) $(FLAGS)
#GCC = gcc $(INCLUDES) $(FLAGS)

#export PATH=$PATH:/opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin
GXX = arm-hisiv100nptl-linux-g++ $(INCLUDES) $(FLAGS) $(LIB)
GCC = arm-hisiv100nptl-linux-gcc $(INCLUDES) $(FLAGS) $(LIB)
//GXX = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc $(INCLUDES) $(FLAGS) $(LIB)
//GCC = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc $(INCLUDES) $(FLAGS) $(LIB)

CSRCS := $(wildcard ./*.c)
CSRCS += $(wildcard ./Common/*.c)
CSRCS += $(wildcard ./LogOut/*.c)
CSRCS += $(wildcard ./hisdk/*.c)
CSRCS += $(wildcard ./toolComm/*.c)
CSRCS += $(wildcard ./mp4v2/*.c)
CSRCS += $(wildcard ./oss/*.c)
CSRCS += $(wildcard ./mxml/*.c)
CSRCS += $(wildcard ./controlServer/*.c)
CSRCS += $(wildcard ./json/*.c)
#CSRCS += $(wildcard ./pollingCheck/*.c)
VPATH = ./Common:./LogOut:./hisdk:./toolComm:./mxml:./mp4v2:./oss:./controlServer:./json

ifeq ($(DECODEMP3TOOL),y)
CSRCS += $(wildcard ./mp3ToPcm/*.c)
VPATH +=:./mp3ToPcm
endif

OBJDIR := ./objs
DEPDIR := ./deps
COBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(CSRCS))))
CPPOBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.cpp,%.o,$(CPPSRCS))))
DEPS = $(patsubst %.c,%.d,$(COBJS))
DEPS += $(patsubst %.cpp,%.d,$(CPPOBJS))  


CS_P = aliyunOss.bin
ALL_OUTPUT = $(CS_P)
all: $(ALL_OUTPUT)

#$(CDEPS) $(CPPDEPS)
$(CS_P):  $(COBJS) $(CPPOBJS) 
	$(GXX) -o $(CS_P) $(CPPOBJS) $(COBJS) $(LIB) -D EXECINFO
	#$(GXX) -shared -o $(CS_P) $(CPPOBJS) $(COBJS)  -D EXECINFO
sinclude $(wildcard $(DEPDIR)/*.d)
	
$(OBJDIR)/%.o : %.c
	@mkdir -p $(OBJDIR) $(DEPDIR)
	$(GCC) -MM -MT$(OBJDIR)/$(notdir $@) $< -o $(DEPDIR)/$(notdir $(basename $<).d )
	$(GCC) -o $(OBJDIR)/$(notdir $@) -c $<

$(OBJDIR)/%.o : %.cpp
	@mkdir -p $(OBJDIR) $(DEPDIR)
	$(GXX) -MM -MT$(OBJDIR)/$(notdir $@) $< -o $(DEPDIR)/$(notdir $(basename $<).d )
	$(GXX) -o $(OBJDIR)/$(notdir $@) -c $<
	
clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(DEPDIR)/*.d
	rm -f $(CS_P)

