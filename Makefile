CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Find all .cpp files in current and subdirectories
SRCS = $(shell find . -name "*.cpp")

# Generate corresponding .o files in the same folder
OBJS = $(SRCS:.cpp=.o)

# Name of the final binary
TARGET = server

# Default target: build everything
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	@echo "Linking executable: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp into a .o
%.o: %.cpp
	@echo "Compiling: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning..."
	rm -f $(OBJS) $(TARGET)