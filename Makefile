include ../lightframe/Makefile_public

ROOMSERVER_DIR = $(BASE_DIR)/roomserver

ROOMSERVER_INCLUDE_DIR = $(ROOMSERVER_DIR)

BLL_DIR = bll
REQ_SONG_DIR = requestsong
CTL_DIR = ctl
DAL_DIR = dal
DEF_DIR = def
CONFIG_DIR = config

ROOMSERVER_OBJ_DIR = $(ROOMSERVER_DIR)/.objs

ROOMSERVER_LDFLAGS = $(LDFLAGS) -ldl -lpthread

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(BIN_DIR)/vdc_roomserver$(BIN_SUFFIX)

ROOMSERVER_SRC = $(wildcard *.cpp)
DAL_SRC = $(wildcard $(DAL_DIR)/*.cpp)
DEF_SRC = $(wildcard $(DEF_DIR)/*.cpp)
CTL_SRC = $(wildcard $(CTL_DIR)/*.cpp)
REQ_SONG_SRC = $(wildcard $(REQ_SONG_DIR)/*.cpp)
BLL_SRC = $(wildcard $(BLL_DIR)/*.cpp)
CONFIG_SRC = $(wildcard $(CONFIG_DIR)/*.cpp)
TEST_SRC = $(wildcard $(TEST_DIR)/*.cpp)

ROOMSERVER_OBJS = $(addprefix $(ROOMSERVER_OBJ_DIR)/, $(subst .cpp,.o,$(ROOMSERVER_SRC)))

OBJS = $(wildcard $(ROOMSERVER_OBJ_DIR)/*.o)

INC = -I$(LIGHTFRAME_INCLUDE_DIR) -I$(ROOMSERVER_INCLUDE_DIR) -I$(PUBLIC_INCLUDE_DIR)

all : $(TARGET)

$(TARGET) : $(ROOMSERVER_OBJS) COMMON LIGHTFRAME DAL DEF CTL REQ_SONG BLL CONFIG
	$(CXX)  -o $@ $(OBJS) $(ROOMSERVER_LDFLAGS)
$(ROOMSERVER_OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@
COMMON:
	cd $(COMMON_DIR); make
DAL:
	cd $(DAL_DIR); make
DEF:
	cd $(DEF_DIR); make
CTL:
	cd $(CTL_DIR); make
REQ_SONG:
	cd $(REQ_SONG_DIR); make
BLL:
	cd $(BLL_DIR); make
CONFIG:
	cd $(CONFIG_DIR); make
LIGHTFRAME:
	cd $(LIGHTFRAME_DIR); make

clean: 
	cd $(COMMON_DIR); make clean;
	cd $(LIGHTFRAME_DIR); make clean;
	cd $(CONFIG_DIR); make clean;
	cd $(DAL_DIR); make clean;
	cd $(DEF_DIR); make clean;
	cd $(CTL_DIR); make clean;
	cd $(REQ_SONG_DIR); make clean;
	cd $(BLL_DIR); make clean;
	rm -f $(OBJS) $(TARGET)


