

LIB = -L./lib/ -L/home/erreur/workSpace/workSource/AliYun/arm/libarmOs/lib -ldl -lrt -lpthread -laprutil-1 -lapr-1 -lcurl -lmxml

#INCLUDES =  -I/usr/local/include
INCLUDES =  -I./src\
			-I/home/erreur/workSpace/workSource/AliYun/arm/libarmOs/include\
			-I/home/erreur/workSpace/workSource/AliYun/arm/libarmOs/include/apr-1\


FLAGS = -O3 -Wall -Wno-unused -Wno-format -fstrength-reduce

GXX = arm-hisiv100nptl-linux-gcc $(INCLUDES) $(FLAGS)
GCC = arm-hisiv100nptl-linux-gcc $(INCLUDES) $(FLAGS)


CSRCS := $(wildcard ./src/*.c)
#CSRCS += $(wildcard ./Con2Server/*.c)


VPATH = ./src

OBJDIR := ./objs
DEPDIR := ./deps

COBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(CSRCS))))
CPPOBJS = $(addprefix $(OBJDIR)/,$(notdir $(patsubst %.cpp,%.o,$(CPPSRCS))))

DEPS = $(patsubst %.c,%.d,$(COBJS))
DEPS += $(patsubst %.cpp,%.d,$(CPPOBJS))  


ALL_OBJ = $(CS_O)

CS_P = libaliyun.so

ALL_OUTPUT = $(CS_P)

all: $(ALL_OUTPUT) # 编译主入�?

#$(CDEPS) $(CPPDEPS)	
$(CS_P):  $(COBJS) $(CPPOBJS) 
	$(GXX) -shared -o $(CS_P) $(CPPOBJS) $(COBJS) $(LIB) -D EXECINFO
	
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

