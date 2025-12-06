SRC = src/*.cpp
END = build/tworm
END_D = build/tworm_debug

COMP = g++
FLAGS = -O3 -std=c++23

all: $(END)

$(END): $(SRC)
	$(COMP) $(SRC) -o $(END) $(FLAGS)

debug: $(END_D)

$(END_D): $(SRC)
	$(COMP) $(SRC) -o $(END_D) $(FLAGS) -g

clean:
	rm $(END) $(END_D)

.PHONY: all clean
