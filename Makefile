CXX = clang++
CXXFLAGS = -std=c++23 -Iinclude -I/usr/include -I/usr/include/GLFW -I/usr/include/GL -O2 -Iimgui
LDLIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

CC = clang
CFLAGS = -Iinclude -O2

SOURCEDIR = src
BUILDDIR = build

SRC := $(wildcard $(SOURCEDIR)/*.cpp) $(wildcard $(SOURCEDIR)/*.c)

OBJ = $(patsubst $(SOURCEDIR)/%.cpp, $(BUILDDIR)/%.o,$(SRC))
OBJ := $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(OBJ))

OUT = testproj

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(LDLIBS) $^ -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)
