
CC				= $(CROSS_COMPILER)g++

CODE_DIR 			= .
SRC_DIR 			= $(CODE_DIR)
INC_DIR 			= ./

TARGET_LIB			= main.exe


FLAGS			= -O2


FLAGS 			+= -DLINUX

IFLAGS    =	-I$(INC_DIR)

CFLAGS   		= $(IFLAGS) $(FLAGS) -Wall  -fPIC -fsigned-char 


vpath  			%.cpp		$(SRC_DIR)

vpath			%.h		$(INC_DIR)

SRCS 				= $(wildcard $(SRC_DIR)/*.cpp)
INCS				= $(wildcard $(INC_DIR)/*.h)

OBJS		= $(notdir $(patsubst %.cpp, %.o, $(SRCS)))


#packet
$(TARGET_LIB):	$(OBJS)
	$(CC) -o $(TARGET_LIB) $(OBJS) $(LD_FLAGS)
		
%.o: %.cpp $(INCS)
	@echo $@
	@echo $(CFLAGS)
	$(CC) -c $< $(CFLAGS)

.PHONY: clean
clean:
	-rm $(TARGET_LIB)
	-rm $(OBJS)






