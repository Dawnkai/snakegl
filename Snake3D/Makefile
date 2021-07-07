LIBS=-lGL -lglfw -lGLEW
HEADERS=lodepng.h  myCube.h  myTeapot.h  shaderprogram.h  libs.h  objloader.h  floor.h
FILES=lodepng.cpp  main_file.cpp  shaderprogram.cpp  libs.cpp  objloader.cpp

main_file: $(FILES) $(HEADERS)
	g++ -o main_file $(FILES)  $(LIBS) -I.
