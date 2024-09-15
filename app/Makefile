CXX= g++
CC = $(CXX)

CXXFLAGS  = -pedantic -Wall -Werror 
CXXFLAGS += -g3
CXXFLAGS += -std=c++17

CPPFLAGS = -DDEBUG
CPPFLAGS += -I$(INCLUDES_DIR)

LDFLAGS = -lgumbo -lcurlpp -lcurl -pthread

INCLUDES_DIR = ../inc/
SOURCES_DIR = ../src/

OBJECTS = $(SOURCES_DIR)configuration.o $(SOURCES_DIR)downloader.o $(SOURCES_DIR)pageParser.o $(SOURCES_DIR)query.o $(SOURCES_DIR)client.o
OBJECTS += $(SOURCES_DIR)BFSCrawler.o $(SOURCES_DIR)DFSCrawler.o $(app/)searchEngine.o 
TARGET = app

$(TARGET) : $(OBJECTS) configuration.json
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

all: $(TARGET)

check : $(TARGET)
	@./$(TARGET) -v


clean:
	@$(RM) ./$(TARGET) $(OBJECTS)

valgrind : $(TARGET)
	valgrind ./$(TARGET) 

.PHONY : all clean check valgrind