#LIB = -L./ -lrtmp -lssl -lcrypto -lgnutls -lgcrypt -ldl -lm
LIB = -L../libarm -lrtmp -lssl -lcrypto -ldl -lm
INCLUDES = -I./
#-lrtmp
#关闭所有 未使用变量、printf格式化、警告 
FLAGS = -O3 -Wall -Wno-unused -Wno-format -O -fstrength-reduce -fPIC -fpermissive
#FLAGS = -g -m32 -Ddebug -O3 -Wall -fPIC -fpermissive 
#FLAGS += -g -rdynamic  -fpermissive

GXX = /opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin/arm-hisiv100nptl-linux-g++ $(INCLUDES) $(FLAGS) 
GCC = /opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin/arm-hisiv100nptl-linux-gcc $(INCLUDES) $(FLAGS)

#GXX = gcc $(INCLUDES) $(FLAGS) 
#GCC = gcc $(INCLUDES) $(FLAGS)

CSRCS := $(wildcard ./*.c)
CSRCS += $(wildcard ./librtmp/*.c)
CSRCS += $(wildcard ./rtmpWrapper/*.c)


CPPSRCS := $(wildcard ./*.cpp)
CPPSRCS += $(wildcard ./librtmp/*.cpp)
CPPSRCS += $(wildcard ./rtmpWrapper/*.cpp)


VPATH = ./librtmp:./rtmpWrapper:./

OBJDIR := ./objs
DEPDIR := ./deps

COBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(CSRCS))))
CPPOBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.cpp,%.o,$(CPPSRCS))))

DEPS = $(patsubst %.c,%.d,$(COBJS))
DEPS += $(patsubst %.cpp,%.d,$(CPPOBJS))  


ALL_OBJ = $(CS_O)

CS_P = librtmpTest.so

ALL_OUTPUT = $(CS_P)

all: $(ALL_OUTPUT)

#$(CDEPS) $(CPPDEPS)	
$(CS_P):  $(COBJS) $(CPPOBJS) 
	$(GXX) -shared  -o $(CS_P) $(CPPOBJS) $(COBJS) $(LIB)
	
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
install:
	cp -f $(CS_P) /usr/lib64
	cp -f $(CS_P) /usr/lib


