CPPFLAGS=-g
LINKER_FLAGS=-lSDL2
PROG=gb


default:
	g++ $(CPP_FLAGS) -o $(PROG) src/*.cpp $(LINKER_FLAGS)
clean:
	rm gb
