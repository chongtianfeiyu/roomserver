include ../../Makefile_public_test
include ../Makefile_public_test

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(ROOMSERVER_OBJ_DIR)/, $(subst .cpp,.o,$(SRC)))

INC = -I$(FRAME_INCLUDE_DIR) -I$(ROOMSERVER_INCLUDE_DIR) -I$(MYSQL_INCLUDE_DIR) -I$(PUBLIC_INCLUDE_DIR)

all : $(OBJS)

$(ROOMSERVER_OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS)
