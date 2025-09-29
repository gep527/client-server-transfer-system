OPENSSL_DIR = /opt/homebrew/opt/openssl@3

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude -I$(OPENSSL_DIR)/include
LDFLAGS = -pthread -L$(OPENSSL_DIR)/lib -lcrypto

# Source files
CLIENT_SRC = src/client.cpp src/pack109lib.cpp src/linkedlistlib.cpp src/lib.cpp
SERVER_SRC = src/server.cpp src/pack109lib.cpp src/linkedlistlib.cpp src/lib.cpp
TEST_SRC   = src/test.cpp   src/pack109lib.cpp src/linkedlistlib.cpp src/lib.cpp
EXTRA_SRC  = src/extra.cpp  src/pack109lib.cpp src/linkedlistlib.cpp src/lib.cpp

# Object files
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
TEST_OBJ   = $(TEST_SRC:.cpp=.o)
EXTRA_OBJ  = $(EXTRA_SRC:.cpp=.o)

# Targets
TARGETS = bin/client bin/server bin/test bin/extra

all: $(TARGETS)

bin/client: $(CLIENT_OBJ)
	@mkdir -p bin
	$(CXX) -o $@ $^ $(LDFLAGS)

bin/server: $(SERVER_OBJ)
	@mkdir -p bin
	$(CXX) -o $@ $^ $(LDFLAGS)

bin/test: $(TEST_OBJ)
	@mkdir -p bin
	$(CXX) -o $@ $^ $(LDFLAGS)

bin/extra: $(EXTRA_OBJ)
	@mkdir -p bin
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o $(TARGETS)

.PHONY: all clean
