LIB = -L ./libarm -L./lib/  -L /usr/lib64/ -lcurl -ldl -lrt -lpthread  -lm 

INCLUDES = -I./curl -I./tmlibghttp/ -I /usr/include/mysql

#关闭所有 未使用变量、printf格式化、警告 
#FLAGS = -O3 -g -rdynamic -Wall -Wno-unused -Wno-format -O -fstrength-reduce -fPIC -fpermissive
#FLAGS = -g -Ddebug -O3 -Wall -fpermissive
#FLAGS += -g -rdynamic  -fpermissive
FLAGS = -g -O3 -Wall 

#GXX = g++ $(INCLUDES) $(FLAGS)
#GCC = gcc $(INCLUDES) $(FLAGS)
GXX = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc $(INCLUDES) $(FLAGS) $(LIB)
GCC = /opt/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc $(INCLUDES) $(FLAGS) $(LIB)

CSRCS := $(wildcard ./*.c)
CSRCS += $(wildcard ./Common/*.c)
CSRCS += $(wildcard ./LogOut/*.c)

CPPSRCS := $(wildcard ./*.cpp)
CPPSRCS += $(wildcard ./Common/*.cpp)
CPPSRCS += $(wildcard ./LogOut/*.cpp)


#VPATH = ./serialAlarm/:./Con2Server/:./mxml/:./GlobalShare/:./UdpSearch:./TcpServer:./tmlibghttp:./communication:./tmMsgProc:./rtpsend:./LogOut:./update:./UploadLog:./
VPATH = ./LogOut/:./Common/:./
#./http/:

OBJDIR := ./objs
DEPDIR := ./deps

COBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(CSRCS))))
CPPOBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.cpp,%.o,$(CPPSRCS))))

DEPS = $(patsubst %.c,%.d,$(COBJS))
DEPS += $(patsubst %.cpp,%.d,$(CPPOBJS))  


ALL_OBJ = $(CS_O)

CS_P = randTest

ALL_OUTPUT = $(CS_P)

all: $(ALL_OUTPUT)

#$(CDEPS) $(CPPDEPS)	
$(CS_P):  $(COBJS) $(CPPOBJS) 
	$(GXX) -o $(CS_P) $(CPPOBJS) $(COBJS) $(LIB)
	
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

