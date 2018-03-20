LIB      = -lglfw3 -pthread -lGLEW -lGLU -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lXcursor -lrt -ldl
STANDARD = -std=c++11
LIBLOCATION = -L /usr/local/lib/ -L /usr/lib/x86_64-linux-gnu/
CPP = ./main.cpp ./World.cpp ./ShaderLoader.cpp  ./FileLoader.cpp ./Structure.cpp ./Model.cpp ./PointCloud.cpp ./Controls.cpp 
	
all: ./main.cpp
	g++ -Wall $(LIBLOCATION) $(STANDARD) -g -o Driver $(CPP) $(LIB)