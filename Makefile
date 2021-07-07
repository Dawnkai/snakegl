LIBS=-lGL -lglfw -lGLEW
HEADERS=*.h
FILES=*.cpp

main_file: $(FILES) $(HEADERS)
	g++ -o main_file $(FILES)  $(LIBS) -I.
