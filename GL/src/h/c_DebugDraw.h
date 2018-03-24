// Helper class; draws the world as seen by Bullet.
// This is very handy to see it Bullet's world matches yours
// How to use this class :
// Declare an instance of the class :
// 
// dynamicsWorld->setDebugDrawer(&mydebugdrawer);
// Each frame, call it :
// mydebugdrawer.SetMatrices(ViewMatrix, ProjectionMatrix);
// dynamicsWorld->debugDrawWorld();
#pragma once

// TODO: Make this my own
#include "glHeader.h"
#include "defs.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

using namespace std;

class BulletDebugDrawer_OpenGL : public btIDebugDraw {
public:
    void init(Controls * control, GLuint shade)
    {
        shader = loadShaders("./GL/src/shaders/SimpleVertexShader.vertexshader", "./GL/src/shaders/SimpleFragmentShader.fragmentshader");
        controls = control;

        glGenBuffers(1, &verticies);

        glGenBuffers(1, &colors);

        MatrixID      = glGetUniformLocation(shader, "MVP");
        ViewMatrixID  = glGetUniformLocation(shader,   "V");
        ModelMatrixID = glGetUniformLocation(shader,   "M");
    }

    void draw()
    {
        mat4 viewMatrix = controls->getViewMatrix();

        mat4 modelMatrix = mat4(1);
        
        mat4 projectionMatrix = controls->getProjectionMatrix();

        mat4 MVP = projectionMatrix * viewMatrix;

        glUseProgram(shader);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);


        //glUseProgram(shader);
        glBindBuffer(GL_ARRAY_BUFFER, verticies);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, colors);
        glBufferData(GL_ARRAY_BUFFER, colorList.size() * sizeof(GLfloat), &colorList[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, verticies); 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colors);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_LINES, 0, sizeof(GLfloat) * points.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);


        points.clear();
        colorList.clear();
    }

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) 
    {
        // For each point
        colorList.push_back(color.getX());
        colorList.push_back(color.getY());
        colorList.push_back(color.getZ());
        colorList.push_back(color.getX());
        colorList.push_back(color.getY());
        colorList.push_back(color.getZ());

        points.push_back(from.getX());
        points.push_back(from.getY());
        points.push_back(from.getZ());
        points.push_back(to.getX());
        points.push_back(to.getY());
        points.push_back(to.getZ());        
    }

    // TODO: Implement drawContactPoint();
    virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
    virtual void reportErrorWarning(const char *) {}
    virtual void draw3dText(const btVector3 &, const char *) {}
    virtual void setDebugMode(int p) {
        m = p;
    }
    int getDebugMode(void) const { return 3; }
    int m;

private:
    GLuint VBO, VAO;
    Controls * controls;
    GLuint shader;

    std::vector <GLfloat> points;
    std::vector <GLfloat> colorList;

    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;

    GLuint colors;
    GLuint verticies;

};






/*#pragma once

#include "glHeader.h"
//#include <btIDebugDraw>

class CustomDebugDraw: public btIDebugDraw
{
  public:
    int m_debugMode;

    virtual void drawLine(const btVector3& from,const btVector3& to ,const btVector3& color);
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB,btScalar distance, int lifeTime,const btVector3& color);
    virtual void reportErrorWarning(const char* warningString);
    virtual void draw3dText(const btVector3& location ,const char* textString);
    virtual void setDebugMode(int debugMode);
    virtual int  getDebugMode() const {return m_debugMode};
};

void CustomDebugDraw::drawLine(const btVector3& from,const btVector3& to ,const btVector3& color)
{
    glLineWidth(2.5f); 
    glColor3f(color.getX(), color.getY(), color.getZ());
    glBegin(GL_LINES);
    glVertex3f(from.getX(), from.getY(), from.getZ());
    glVertex3f(to.getX(), to.getY(), to.getZ());
    glEnd();
}

void CustomDebugDraw::drawContactPoint(const btVector3 &PointOnB , const btVector3 &normalOnB, btScalar distance ,int lifeTime, const btVector3 &color)
{

}

void CustomDebugDraw::reportErrorWarning(const char *warningString)
{

}

void CustomDebugDraw::draw3dText(const btVector3 &location, const char *textString)
{

}

void CustomDebugDraw::setDebugMode(int debugMode)
{

}

int CustomDebugDraw::getDebugMode() const
{
    return DBG_DrawWireframe;
}

mat4 viewMatrix = controls->getViewMatrix();

        mat4 modelMatrix = mat4(1);
        
        mat4 projectionMatrix = controls->getProjectionMatrix();

        mat4 MVP = projectionMatrix * viewMatrix;

        glUseProgram(shader);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

        // Vertex data
        GLfloat points[6];
        points[0] = from.getX();
        points[1] = from.getY();    
        points[2] = from.getZ();    
        points[3] = to.getX();    
        points[4] = to.getY();            
        points[5] = to.getZ();

        GLfloat colorInfo[6];
        colorInfo[0] = color.getX();
        colorInfo[1] = color.getY();
        colorInfo[2] = color.getZ();
        colorInfo[3] = color.getX();
        colorInfo[4] = color.getY();
        colorInfo[5] = color.getZ();

        //glUseProgram(shader);
        glBindBuffer(GL_ARRAY_BUFFER, verticies);
        glBufferData(GL_ARRAY_BUFFER, 6, points, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, colors);
        glBufferData(GL_ARRAY_BUFFER, 6, colorInfo, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, verticies); 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colors);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_LINES, 0, 6);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
*/