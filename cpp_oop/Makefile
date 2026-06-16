# Makefile for C++ OOP Tutorial
# Demonstrates header files, source files, enum, and extern

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++14 -I./include
TARGET = vehicle_demo

# Source files
SRCS = src/Vehicle.cpp src/Car.cpp src/Airplane.cpp src/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete!"

# Rebuild everything
rebuild: clean all

.PHONY: all run clean rebuild
