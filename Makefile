CXX = g++
CXXFLAGS = -g -std=c++11
SERVER_FLAGS = -L/opt/lib -lncurses
SOURCES = main.cpp reversicompetitionagent.cpp reversiboard.cpp coordinate.cpp
SERVER_SOURCES = server.cpp reversicompetitionagent.cpp reversiboard.cpp coordinate.cpp

OBJECTS=$(SOURCES:%.cpp=$(OBJ)%.o)
SERVER_OBJECTS=$(SERVER_SOURCES:%.cpp=$(S_OBJ)%.o)

$(OBJ)/%.o: %.cpp
	@echo Building Objects
	$(CXX) $(CXXFLAGS) -c $@ $<

$(S_OBJ)/%.o: %.cpp
	@echo Building Server Objects
	$(CXX) $(CXXFLAGS) $(SERVER_FLAGS) -c $@ $<

EXECUTABLE=agent

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo "Agent built. Ready for action."

SERVER_EXECUTABLE=server

$(SERVER_EXECUTABLE): $(SERVER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJECTS) -o $@ $(SERVER_FLAGS)
	@echo "Server built. Ready to play games."

newgame:
	rm numberofmoves*

run:
	./$(EXECUTABLE)

play:
	./server

clean:
	rm *.o $(EXECUTABLE) $(SERVER_EXECUTABLE)
