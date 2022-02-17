#####
CFLAGS = -std=c++17 -m64 -Wall -Wextra -Wpedantic
LIBS = $(patsubst cpp/source/%.cpp,obj/%.o,$(filter-out cpp/source/main.cpp,$(wildcard cpp/source/*.cpp)))

main: cpp/source/main.cpp dirs $(LIBS)
	g++ $(CFLAGS) -Icpp/include -obin/unbsh $< $(LIBS)

tests/%: cpp/tests/%.cpp $(LIBS)
	g++ $(CFLAGS) -DUNIT_TEST -Icpp/include -obin/test $< $(LIBS)

obj/%.o: cpp/source/%.cpp cpp/include/%.h
	g++ $(CFLAGS) -Icpp/include -c -o$@ $<

.PHONY: env dirs clean cleanse

dirs:
	mkdir -p bin
	mkdir -p obj

# CUIDADO AO UTILIZAR AS RECEITAS ABAIXO POIS ELAS PODEM REMOVER OU SUBSTITUIR ARQUIVOS IMPORTANTES
env:
	mkdir -vp ./env/bin
	cp -v ./bin/unbsh ./env/bin
	rsync -av ./env/ ~

clean:
	rm -vfr obj

cleanse: clean
	rm -vfr bin
	rm -vfr env/bin

