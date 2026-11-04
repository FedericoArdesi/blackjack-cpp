# Makefile per Blackjack. Uso:
#   make                       compila (crea ./blackjack, su Windows blackjack.exe)
#   make run                   compila e avvia il gioco
#   make test                  compila ed esegue i test della logica
#   make demo SCENARIO=split   avvia il gioco con carte decise (vedi tools/demo.cpp)
#   make clean                 cancella i file generati
#
# Funziona con qualsiasi GNU make (anche quello di w64devkit su Windows).

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
# -Isrc: gli #include sono relativi a src/
# -MMD -MP: genera i file .d, cosi' make ricompila anche quando cambia un .h
CPPFLAGS += -Isrc -MMD -MP

ifeq ($(OS),Windows_NT)
  EXE := .exe
endif

TARGET := blackjack$(EXE)
BUILD  := build

SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS := $(patsubst src/%.cpp,$(BUILD)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Tutto il gioco tranne main.cpp: lo condividono test e demo, che hanno un loro main().
LIB_OBJS := $(filter-out $(BUILD)/main.o,$(OBJS))

TEST_BIN := $(BUILD)/test_logica$(EXE)
DEMO_BIN := $(BUILD)/demo$(EXE)
SCENARIO ?= split

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

$(BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

# --- Test e demo ---

$(TEST_BIN): tests/test_logica.cpp $(LIB_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIB_OBJS) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(DEMO_BIN): tools/demo.cpp $(LIB_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIB_OBJS) -o $@

demo: $(DEMO_BIN)
	./$(DEMO_BIN) $(SCENARIO)

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all run test demo clean

-include $(DEPS)
