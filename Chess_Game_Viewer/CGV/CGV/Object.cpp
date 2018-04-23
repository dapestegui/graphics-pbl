//
//  Object.cpp
//  tutorial07_model_loading
//
//  Created by Elias Piovani on 21/04/2018.
//

// Include GLEW
#include <GL/glew.h>
#include <vector>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include "Object.hpp"
#include "objloader.hpp"
#include "controls.hpp"

class Object
{
private:
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> n;
    
public:
//    char path[10];
    std::vector<glm::vec3> v;
    GLuint vb;
    GLuint uvb;
    GLuint nb;
    GLuint texture;
    GLuint textureID;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    
    void load(const char *path, const GLuint &Texture, const GLuint &TextureID)
    {
        bool wParts = loadOBJ(path, v, uvs, n);
        glGenBuffers(1, &vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(glm::vec3), &v[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &uvb);
        glBindBuffer(GL_ARRAY_BUFFER, uvb);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &nb);
        glBindBuffer(GL_ARRAY_BUFFER, nb);
        glBufferData(GL_ARRAY_BUFFER, n.size() * sizeof(glm::vec3), &n[0], GL_STATIC_DRAW);
        
        texture = Texture;
        textureID = TextureID;
    }
    
    void del()
    {
        glDeleteBuffers(1, &uvb);
        glDeleteBuffers(1, &vb);
        glDeleteBuffers(1, &nb);
    }
    
    void setPos(float x, float y, float z)
    {
        pos = glm::vec3(x, y, z);
    }
    
    glm::vec3 getPos()
    {
        return pos;
    }
};

class ProjMatrix
{
public:
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ModelMatrix;
    glm::mat4 MVP;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    
    void uniform(GLuint &MID, GLuint &VMID, GLuint &MMID)
    {
        ProjectionMatrix = getProjectionMatrix();
        ViewMatrix = getViewMatrix();
        ModelMatrix = glm::mat4(1.0);
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        
        MatrixID = MID;
        ViewMatrixID = VMID;
        ModelMatrixID = MMID;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    }
    
};












