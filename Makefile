# lazygitcpp - C++ git TUI (line-by-line port of lazygit)
#
# One C++ translation unit per upstream Go file: src/<pkg>/... mirrors
# upstream pkg/<pkg>/..., so sources are collected recursively.
#
# Builds run in GitHub Actions only (.github/workflows/build.yml).

CXX ?= g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -pthread
TARGET := lazygitcpp
SRCDIR := src
OBJDIR := build

SRCS := $(shell find $(SRCDIR) -name '*.cpp' | sort)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
