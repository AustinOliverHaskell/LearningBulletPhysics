LIB      = -lglfw3 -pthread -lGLEW -lGLU -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lXcursor -lrt -ldl -lBulletDynamics -lBulletCollision -lBulletSoftBody -lLinearMath 
STANDARD = -std=c++11
LIBLOCATION = -L /usr/local/lib/ -L /usr/lib/x86_64-linux-gnu/
INCLOCATION = -I /usr/local/include/bullet 
CPP = ./main.cpp ./GL/src/World.cpp ./GL/src/ShaderLoader.cpp ./GL/src/FileLoader.cpp ./GL/src/PointCloud.cpp ./GL/src/Model.cpp ./GL/src/Controls.cpp ./GL/src/Structure.cpp 
CPP2= ./GL/src/btFractureBody.cpp ./GL/src/btFractureDynamicsWorld.cpp

all: ./main.cpp
	g++ -Wall $(LIBLOCATION) $(INCLOCATION) $(STANDARD) -g -o Driver $(CPP) $(CPP2) $(LIB)