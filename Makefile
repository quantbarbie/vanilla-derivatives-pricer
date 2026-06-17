CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
SRC       = main.cpp Date.cpp Market.cpp RateCurve.cpp VolCurve.cpp
TARGET    = pricer

$(TARGET): $(SRC) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
