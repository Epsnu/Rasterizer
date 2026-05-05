# Complier and flags
CXX = g++
CXXFLAGS = -Wall -g
# 			-I"C:/Program Files/vcpkg/installed/x64-mingw-static/include" \
# 			-L"C:/Program Files/vcpkg/installed/x64-mingw-static/lib" \
# 			-DUSE_LIBPNG -lpng

# Source files
SOURCES = main.cpp utilities.cpp classes.cpp string_utils.cpp rasterize.cpp PNG.cpp lodepng.cpp RGBPixel.cpp
TEST_SOURCES = test.cpp utilities.cpp classes.cpp string_utils.cpp rasterize.cpp PNG.cpp lodepng.cpp RGBPixel.cpp
# uselibpng.c
HEADERS = classes.h utilities.h string_utils.h rasterize.h PNG.h lodepng.h RGBPixel.h
# uselibpng.h

# Arguments (can be supplied in terminal)
ARGS = .\core-files\rast-checkers.txt
# ARGS = .\anylang-files\warmup-messy2.txt .\anylang-files\warmup-messy2-result.png .\anylang-files\warmup_messy2.png
# ARGS = .\anylang-files\warmup-simple.txt .\anylang-files\warmup-simple-result.png .\anylang-files\warmup_simple.png

# Executable name
TARGET = main
TEST = test

# Build exec and test
all: $(SOURCES) $(TEST_SOURCES) $(HEADERS)
	@echo "Building Main..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
	@echo "Building Test..."
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST)

# build main exec
build: $(SOURCES) $(HEADERS)
	@echo "Building Main..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

# build test exec
build-test: $(TEST_SOURCES) $(HEADERS)
	@echo "Building Test..."
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST)

run:
	./$(TARGET) $(file)

run-test:
	./$(TEST) $(file)

clean:
	rm -f $(TARGET)
	rm -f $(TEST)