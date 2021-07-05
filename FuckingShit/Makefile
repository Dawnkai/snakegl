LIBS=-lGL -lglfw -lGLEW
HEADERS=shaderprogram.h libs.h lodepng.h
FILES=main_file.cpp libs.cpp shaderprogram.cpp lodepng.cpp

main_file: $(FILES) $(HEADERS)
	g++ -o main_file $(FILES)  $(LIBS) -I.
