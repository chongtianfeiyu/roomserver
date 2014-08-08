include ../../lightframe/Makefile_public

ROOMSERVER_DIR = $(BASE_DIR)/roomserver

ROOMSERVER_INCLUDE_DIR = $(ROOMSERVER_DIR)

ROOMSERVER_OBJ_DIR = $(ROOMSERVER_DIR)/.objs

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(ROOMSERVER_OBJ_DIR)/, $(subst .cpp,.o,$(SRC)))

INC = -I$(LIGHTFRAME_INCLUDE_DIR) -I$(ROOMSERVER_INCLUDE_DIR) -I$(MYSQL_INCLUDE_DIR) -I$(PUBLIC_INCLUDE_DIR)

all : $(OBJS)

$(ROOMSERVER_OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS)
