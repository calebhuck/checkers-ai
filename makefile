CC:=g++
INCLUDE:=./include
CFLAGS:=-c -I$(INCLUDE)
SRC_DIR:=src
OBJ_DIR:=obj
SRC_FILES:=$(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES:=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
EXECUTABLE=CheckersApp

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	`wx-config --cxx` -o $@ $^ `wx-config --libs`

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) `wx-config --cxxflags` $(CFLAGS) -o $@ $< 

run: $(EXECUTABLE)
	./$(EXECUTABLE)
	
clean:
	rm -f $(OBJ_DIR)/*.o CheckersApp