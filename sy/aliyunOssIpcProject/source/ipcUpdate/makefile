LIB = -L../aliyunproject_V2.0/liball/libarm -lcurl -ldl -lrt -lpthread -lm -Wl,--rpath=../aliyunproject_V2.0/liball/libarm
#LIB = -L../aliyunproject_V2.0/liball/libarm -lNetLib -loss_c_sdk -lutf8proc  -laprutil-1 -lapr-1 -lcurl -lxml2 -lfaac -ldl -lrt -lpthread -lm -Wl,--rpath=./liball/libarm

INCLUDES =  -I../aliyunproject_V2.0/include \
			-I./ \
			-I../aliyunproject_V2.0
#关闭所有 未使用变量、printf格式化、警告 
#FLAGS = -m32 -O2 -Wall -Wno-unused -Wno-format -fstrength-reduce -DHI_OS_LINUX
FLAGS = -O2 -Wall -Wno-unused -Wno-format -fstrength-reduce -DHI_OS_LINUX -DCPU_ARM
#FLAGS += -g -rdynamic

#GXX = g++ $(INCLUDES) $(FLAGS)
#GCC = gcc $(INCLUDES) $(FLAGS)

#export PATH=$PATH:/opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin
GXX = arm-hisiv100nptl-linux-g++ $(INCLUDES) $(FLAGS) $(LIB)
GCC = arm-hisiv100nptl-linux-gcc $(INCLUDES) $(FLAGS) $(LIB)
//GXX = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc $(INCLUDES) $(FLAGS) $(LIB)
//GCC = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc $(INCLUDES) $(FLAGS) $(LIB)

CSRCS := $(wildcard ./*.c)
CSRCS += $(wildcard ../aliyunproject_V2.0/Common/*.c)
CSRCS += $(wildcard ../aliyunproject_V2.0/json/*.c)
CSRCS += $(wildcard ../aliyunproject_V2.0/Logout/*.c)
CSRCS += $(wildcard ./update/*.c)

VPATH = ../aliyunproject_V2.0/Common:../aliyunproject_V2.0/LogOut:../aliyunproject_V2.0/json:./update

OBJDIR := ./objs
DEPDIR := ./deps
COBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(CSRCS))))
CPPOBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.cpp,%.o,$(CPPSRCS))))
DEPS = $(patsubst %.c,%.d,$(COBJS))
DEPS += $(patsubst %.cpp,%.d,$(CPPOBJS))  


CS_P = ipcUpdate.bin
ALL_OUTPUT = $(CS_P)
all: $(ALL_OUTPUT)

#$(CDEPS) $(CPPDEPS)
$(CS_P):  $(COBJS) $(CPPOBJS) 
	$(GXX) -o $(CS_P) $(CPPOBJS) $(COBJS)  -D EXECINFO
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

