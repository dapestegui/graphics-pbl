//
//  Object.cpp
//  tutorial07_model_loading
//
//  Created by Elias Piovani on 21/04/2018.
//
#include <stdio.h>
#include <string.h>

#include <filesystem>
#include <fstream>
#include <iostream>
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

#include <math.h>

class Object
{
private:
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> n;
    
public:
//    char path[10];
    char pieceType[2];
    bool firstMove = true;
    std::vector<glm::vec3> v;
    GLuint vb;
    GLuint uvb;
    GLuint nb;
    const GLuint *texture;
    const GLuint *textureID;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);

    Object cloneLogic() {
        Object object;
        memcpy(object.pieceType, pieceType, 2);
        object.firstMove = firstMove;
        return object;
    }
    
    void load(const char *path, char *piecetype, const GLuint &Texture, const GLuint &TextureID)
    {
        memcpy(pieceType, piecetype, 2);
        
        // implement if obj was loaded well
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
        
        texture = &Texture;
        textureID = &TextureID;
    }
    
    void del()
    {
        glDeleteBuffers(1, &uvb);
        glDeleteBuffers(1, &vb);
        glDeleteBuffers(1, &nb);
    }
    
//    bool fstMove()
//    {
//        return firstMove;
//    }
    
    void setPos(float x, float y, float z)
    {
        pos = glm::vec3(x, y, z);
        firstMove = false;
    }
    
//    glm::vec3 getPos()
//    {
//        return pos;
//    }
    
//    const char* getType()
//    {
//        return pieceType;
//    }
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
    GLuint LightID;
    
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
    
    void setLightID(GLuint &lightID)
    {
        LightID = lightID;
    }
    
};

class Step {
public:
	char pieceStart[3];
	char pieceEnd[3];
	bool castling;
	bool promotion;
	char rookStart[3];
	char rookEnd[3];
	char promotedTo;
};

class StepsArray {
public:
	int index;
	int active;
	Step steps[1000];
//public:
	//void init_StepsArray(int index);
};


class BoardMatrix
{
private:
    Object *Matrix[8][8];
	Object *SimulationMatrix[8][8];
    bool movingPiece = false;
    int nBPiecesDead = 0, nWPiecesDead = 0;
public:
    void init(Object WhitePieces[16], Object BlackPieces[16])
    {
        // Init matrix with Null values
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Matrix[i][j] = NULL;
				SimulationMatrix[i][j] = NULL;
            }
        }
        // 0 to 7 - Pawn / 8 and 9 - Rook / 10 and 11 - Knight / 12 and 13 - Bishop / 14 - Queen / 15 - King
        for (int j = 0; j < 8; j++) {
            Matrix[1][j] = &WhitePieces[j];
            Matrix[6][j] = &BlackPieces[j];
        }

        Matrix[0][0] = &WhitePieces[8];
        Matrix[0][1] = &WhitePieces[10];
        Matrix[0][2] = &WhitePieces[12];
        Matrix[0][3] = &WhitePieces[14];
        Matrix[0][4] = &WhitePieces[15];
        Matrix[0][5] = &WhitePieces[13];
        Matrix[0][6] = &WhitePieces[11];
        Matrix[0][7] = &WhitePieces[9];
        
        Matrix[7][0] = &BlackPieces[8];
        Matrix[7][1] = &BlackPieces[10];
        Matrix[7][2] = &BlackPieces[12];
        Matrix[7][3] = &BlackPieces[14];
        Matrix[7][4] = &BlackPieces[15];
        Matrix[7][5] = &BlackPieces[13];
        Matrix[7][6] = &BlackPieces[11];
        Matrix[7][7] = &BlackPieces[9];

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (Matrix[i][j]) {
                    SimulationMatrix[i][j] = new Object();
                    *SimulationMatrix[i][j] = Matrix[i][j]->cloneLogic();
                }
            }
        }
    }
    
    void print()
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (Matrix[i][j] != NULL) printf("%s\t", Matrix[i][j]->pieceType);
                else printf("%p\t", Matrix[i][j]);
            }
            printf("\n");
        }
    }

	void printSimulation()
	{
		std::cout << "  a \t  b \t  c \t  d \t  e \t  f \t  g \t  h \t" << std::endl;
		for (int i = 0; i < 8; i++) {
			std::cout << i + 1 << " ";
			for (int j = 0; j < 8; j++) {
				if (SimulationMatrix[i][j] != NULL) printf("[%s\t]", SimulationMatrix[i][j]->pieceType);
				else printf("[  \t]");
			}
			printf("\n");
		}
	}

	bool castling(const char kingStart[3], const char kingEnd[3], const char rookStart[3], const char rookEnd[3]) {
		return move(kingStart, kingEnd) || move(rookStart, rookEnd);
	}

	bool promotion(char pieceStart[3], char pieceEnd[3], char promotedTo) {

	}
    
    bool move(const char pieceStart[3], const char pieceEnd[3])
    {
        int initPosX = 0, initPosZ = 0, finalPosX = 0, finalPosZ = 0;
        
        movingPiece = true;
        
        char startColumn = pieceStart[0];
        char startRow = pieceStart[1];
        char endColumn = pieceEnd[0];
        char endRow = pieceEnd[1];
        
        switch (startColumn) {
            case 'a':
                initPosZ = 0;
                break;
            case 'b':
                initPosZ = 1;
                break;
            case 'c':
                initPosZ = 2;
                break;
            case 'd':
                initPosZ = 3;
                break;
            case 'e':
                initPosZ = 4;
                break;
            case 'f':
                initPosZ = 5;
                break;
            case 'g':
                initPosZ = 6;
                break;
            case 'h':
                initPosZ = 7;
                break;
        }
        
        switch (startRow) {
            case '1':
                initPosX = 0;
                break;
            case '2':
                initPosX = 1;
                break;
            case '3':
                initPosX = 2;
                break;
            case '4':
                initPosX = 3;
                break;
            case '5':
                initPosX = 4;
                break;
            case '6':
                initPosX = 5;
                break;
            case '7':
                initPosX = 6;
                break;
            case '8':
                initPosX = 7;
                break;
                
        }
        
        switch (endColumn) {
            case 'a':
                finalPosZ = 0;
                break;
            case 'b':
                finalPosZ = 1;
                break;
            case 'c':
                finalPosZ = 2;
                break;
            case 'd':
                finalPosZ = 3;
                break;
            case 'e':
                finalPosZ = 4;
                break;
            case 'f':
                finalPosZ = 5;
                break;
            case 'g':
                finalPosZ = 6;
                break;
            case 'h':
                finalPosZ = 7;
                break;
        }
        
        switch (endRow) {
            case '1':
                finalPosX = 0;
                break;
            case '2':
                finalPosX = 1;
                break;
            case '3':
                finalPosX = 2;
                break;
            case '4':
                finalPosX = 3;
                break;
            case '5':
                finalPosX = 4;
                break;
            case '6':
                finalPosX = 5;
                break;
            case '7':
                finalPosX = 6;
                break;
            case '8':
                finalPosX = 7;
                break;
        }
        
//        float step = 0.15f;
        float speed = 20.0f;
        float stepX = float(finalPosX - initPosX)/speed;
        float stepZ = float(finalPosZ - initPosZ)/speed;
        
        //implementation - verify if contains s piece on pieceStart!
        if (Matrix[initPosX][initPosZ] == NULL) {
            return false;
        }
        
        float dist = sqrt(pow((finalPosX * 2.0f) - (Matrix[initPosX][initPosZ]->pos.x), 2.0) +
                          pow((finalPosZ * 2.0f) - (Matrix[initPosX][initPosZ]->pos.z), 2.0));
        
        float initDist = sqrt(pow((finalPosX * 2.0f) - (initPosX * 2.0f), 2.0) +
                    pow((finalPosZ * 2.0f) - (initPosZ * 2.0f), 2.0));
        
        if (dist > 0.0002) {
            Matrix[int(initPosX)][int(initPosZ)]->pos.x += stepX;
            // Only Knights can jump over other piece, so applies an arc movement to the Knights only
            if (Matrix[initPosX][initPosZ]->pieceType[1] == 'N') {
                float y = sin(((3.14159)/initDist) * (initDist - dist));
                Matrix[int(initPosX)][int(initPosZ)]->pos.y = y * 4.0;
            }
            Matrix[int(initPosX)][int(initPosZ)]->pos.z += stepZ;
        }
        else
        {
            Matrix[int(initPosX)][int(initPosZ)]->pos.y = 0.0;
            //update boardMatrix with new position of the piece
            // if piece killed another pice put it aside of the table
            if (Matrix[finalPosX][finalPosZ] != NULL) {
                // if piece is black piece goes to the right of the board, if is white goes to the left
                if (Matrix[finalPosX][finalPosZ]->pieceType[0] == 'B') {
                    Matrix[finalPosX][finalPosZ]->pos.x = -2.0f + nBPiecesDead; // -2.0f 18.0f
                    Matrix[finalPosX][finalPosZ]->pos.z = 18.0f;
                    nBPiecesDead++;
                }
                else
                {
                    Matrix[finalPosX][finalPosZ]->pos.x = 16.0f + nWPiecesDead;
                    Matrix[finalPosX][finalPosZ]->pos.z = -4.0f;
                    nBPiecesDead++;
                }
                
            }
            Matrix[finalPosX][finalPosZ] = Matrix[initPosX][initPosZ];
            Matrix[initPosX][initPosZ] = NULL;
            
            movingPiece = false;
        }
        
        return movingPiece;
    }

	public :StepsArray GetPosibilities(char destination[], char piece, char color, bool capture,  Object *SimulationMatrix[8][8]) {
		//For store the possibilities we are going to store only the .init part of each Step object.
		StepsArray posibilitiesStepsArrayAUX;
		posibilitiesStepsArrayAUX.steps[100];
		posibilitiesStepsArrayAUX.index = 0;
		std::string a, b;
		float PI = 3.1415;
		/*Step pos1;
		Step pos2;
		Step pos3;
		Step pos4;
		Step pos5;
		Step pos6;
		Step pos7;
		Step pos8;*/
		char letra = destination[0];
		char numero = destination[1];
		int posib = 0;
		std::cout << '\n';

		switch (piece) {
		case 'N': // Knight  Caballo
			std::cout << "Possible Caballo INIT:\n";
			posibilitiesStepsArrayAUX.steps[0].pieceStart[0] = destination[0] - 2;
			posibilitiesStepsArrayAUX.steps[0].pieceStart[1] = destination[1] - 1;

			posibilitiesStepsArrayAUX.steps[1].pieceStart[0] = destination[0] - 2;
			posibilitiesStepsArrayAUX.steps[1].pieceStart[1] = destination[1] + 1;

			posibilitiesStepsArrayAUX.steps[2].pieceStart[0] = destination[0] + 2;
			posibilitiesStepsArrayAUX.steps[2].pieceStart[1] = destination[1] - 1;

			posibilitiesStepsArrayAUX.steps[3].pieceStart[0] = destination[0] + 2;
			posibilitiesStepsArrayAUX.steps[3].pieceStart[1] = destination[1] + 1; //yyy? it was -2


			posibilitiesStepsArrayAUX.steps[4].pieceStart[0] = destination[0] - 1;
			posibilitiesStepsArrayAUX.steps[4].pieceStart[1] = destination[1] - 2;

			posibilitiesStepsArrayAUX.steps[5].pieceStart[0] = destination[0] - 1;
			posibilitiesStepsArrayAUX.steps[5].pieceStart[1] = destination[1] + 2;

			posibilitiesStepsArrayAUX.steps[6].pieceStart[0] = destination[0] + 1;
			posibilitiesStepsArrayAUX.steps[6].pieceStart[1] = destination[1] - 2;

			posibilitiesStepsArrayAUX.steps[7].pieceStart[0] = destination[0] + 1;
			posibilitiesStepsArrayAUX.steps[7].pieceStart[1] = destination[1] + 2;

			//Add them to array
			//posibilitiesStepsArrayAUX.steps[1] = pos2; 
			//posibilitiesStepsArrayAUX.steps[2] = pos3; 
			//posibilitiesStepsArrayAUX.steps[3] = pos4; 
			//posibilitiesStepsArrayAUX.steps[4] = pos5; 
			//posibilitiesStepsArrayAUX.steps[5] = pos6; 
			//posibilitiesStepsArrayAUX.steps[6] = pos7; 
			//posibilitiesStepsArrayAUX.steps[7] = pos8;
			posibilitiesStepsArrayAUX.index = 7;

			break;
		case 'B': //Bishop Alfil
			std::cout << "Possible Bishop INIT:";
			std::cout << '\n';
			for (int R = 1; R < 8; R++) {
				for (int theta = 45, i = 0; theta < 360; theta += 90, i++) {
					signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
					signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
					int s = destination[0] + R * x;
					int m = destination[1] + R * y;
					letra = s;
					numero = m;
					a = s;
					b = m;
					// only puts in posssible values those inside the board
					if (a >= "a" && a <= "h" && b >= "1" && b <= "8") {
						posibilitiesStepsArrayAUX.steps[posib].pieceStart[0] = letra;
						posibilitiesStepsArrayAUX.steps[posib].pieceStart[1] = numero;
						
						//Print possibilities	
						std::cout << posibilitiesStepsArrayAUX.steps[posib].pieceStart[0];
						std::cout << posibilitiesStepsArrayAUX.steps[posib].pieceStart[1];
						//std::cout << '\n';
						posibilitiesStepsArrayAUX.index = posib;
						
						//std::cout << posib;
						posib++;
					} else {
						posibilitiesStepsArrayAUX.index = posib;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[0] = '\0';
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[1] = '\0';
						posib++;
					}

				}
			}
			
			break;
		case 'R': // Rook Torre

			std::cout << "Possible ROOK (torre) INIT:";
			std::cout << '\n';

			for (int R = 1; R < 8; R++) {
				for (int theta = 0, i = 0; theta < 360; theta += 90, i++) {
					signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
					signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
					int a = destination[0] + R * x;
					int b = destination[1] + R * y;

					if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
						letra = a;
						numero = b;
						posibilitiesStepsArrayAUX.steps[posib].pieceStart[0] = letra;
						posibilitiesStepsArrayAUX.steps[posib].pieceStart[1] = numero;
						
						//Print
						//std::cout << posibilitiesStepsArrayAUX.steps[posib].pieceStart[0];
						//std::cout << posibilitiesStepsArrayAUX.steps[posib].pieceStart[1];
						//std::cout << '\n';
						//--end print
						posibilitiesStepsArrayAUX.index = posib;
						posib++;
						//std::cout << posib;
					} else {
						posibilitiesStepsArrayAUX.index = posib;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[0] = '\0';
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[1] = '\0';
						posib++;
					}
				}
			}		
			//std::cout << posib;
			break;
		case 'K':

			std::cout << "Possible Queen King:";
			std::cout << '\n';

			posibilitiesStepsArrayAUX.steps[0].pieceStart[0] = destination[0];
			posibilitiesStepsArrayAUX.steps[0].pieceStart[1] = destination[1] - 1;

			posibilitiesStepsArrayAUX.steps[1].pieceStart[0] = destination[0];
			posibilitiesStepsArrayAUX.steps[1].pieceStart[1] = destination[1] + 1;

			posibilitiesStepsArrayAUX.steps[2].pieceStart[0] = destination[0] - 1;
			posibilitiesStepsArrayAUX.steps[2].pieceStart[1] = destination[1];

			posibilitiesStepsArrayAUX.steps[3].pieceStart[0] = destination[0] + 1;
			posibilitiesStepsArrayAUX.steps[3].pieceStart[1] = destination[1];

			posibilitiesStepsArrayAUX.steps[4].pieceStart[0] = destination[0] + 1;
			posibilitiesStepsArrayAUX.steps[4].pieceStart[1] = destination[1] - 1;

			posibilitiesStepsArrayAUX.steps[5].pieceStart[0] = destination[0] + 1;
			posibilitiesStepsArrayAUX.steps[5].pieceStart[1] = destination[1] + 1;

			posibilitiesStepsArrayAUX.steps[6].pieceStart[0] = destination[0] - 1;
			posibilitiesStepsArrayAUX.steps[6].pieceStart[1] = destination[1] - 1;

			posibilitiesStepsArrayAUX.steps[7].pieceStart[0] = destination[0] - 1;
			posibilitiesStepsArrayAUX.steps[7].pieceStart[1] = destination[1] + 1;

			//Add them to array
			//posibilitiesStepsArrayAUX.steps[0] = pos1;posibilitiesStepsArrayAUX.steps[1] = pos2; posibilitiesStepsArrayAUX.steps[2] = pos3; posibilitiesStepsArrayAUX.steps[3] = pos4; posibilitiesStepsArrayAUX.steps[4] = pos5; posibilitiesStepsArrayAUX.steps[5] = pos6; posibilitiesStepsArrayAUX.steps[6] = pos7; posibilitiesStepsArrayAUX.steps[7] = pos8;
			posibilitiesStepsArrayAUX.index = 7;


			break;
		case 'Q':
			std::cout << "Possible Queen INIT:";
			std::cout << '\n';
			for (int R = 1; R < 8; R++) {
				for (int theta = 0, i = 0; theta < 360; theta += 45, i++) {
					signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
					signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
					int a = destination[0] + R * x;
					int b = destination[1] + R * y;
					if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
						letra = a;
						numero = b;
					}
					else {
						letra = 'w';
						numero = '9';
					}

						posibilitiesStepsArrayAUX.index = posib;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[0] = letra;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[1] = numero;
						posib++;
						//Print
						std::cout << posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[0];
						std::cout << posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[1];
						std::cout << '\n';
					/*} else {
						std::cout << "shit1" << std::endl;
						posibilitiesStepsArrayAUX.index = posib;
						std::cout << "shit2" << std::endl;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[0] = 'w';
						std::cout << "shit3" << std::endl;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[1] = '9';
						std::cout << "shit4" << std::endl;
						posib++;
						std::cout << "shit5" << std::endl;
					}*/
				}
			}
			break;
		default: // Pawn
			
			//pos1.pieceStart[1] = destination[0] - 1; sirve para restar una letra
			std::cout << "PAWN " << color << " " << destination[0] << destination[1] << std::endl;
			//Normal move
			int l = 0;
			if (!capture) {
				if (color == 'W') {
					std::cout << "HOLA w!" << std::endl;
					posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0];
					posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] - 1;
					l++;

					posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0];
					posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] - 2;
					l++;
				}
				else {
					std::cout << "HOLA b!" << std::endl;
					posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0];
					posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] + 1;
					l++;

					posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0];
					posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] + 2;
					l++;
				}
			} else {
				// Kill move diagonal
				if (color == 'W') {
					std::cout << "w KILL" << std::endl;
					std::cout << (char)(destination[1]) << (char)(destination[0]) << std::endl;
					if (SimulationMatrix[destination[1] - '1'][destination[0] - 'a'] &&
						(SimulationMatrix[destination[1] - '1'][destination[0] - 'a']->pieceType[0] == 'B')) {
						posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0] - 1;
						posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] - 1;
						l++;
            posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0] + 1;
            posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] - 1;
            l++;
					}
				} else {
					std::cout << "b KILL" << std::endl;
					std::cout << (char)(destination[1]) << (char)(destination[0]) << std::endl;
					///std::cout << SimulationMatrix[destination[1] + 1 - '1'][destination[0] + 1 - 'a'] << std::endl;
					if (SimulationMatrix[destination[1] - '1'][destination[0] - 'a'] &&
						(SimulationMatrix[destination[1] - '1'][destination[0] - 'a']->pieceType[0] == 'W')) {
            posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0] - 1;
            posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] + 1;
            l++;
            posibilitiesStepsArrayAUX.steps[l].pieceStart[0] = destination[0] + 1;
            posibilitiesStepsArrayAUX.steps[l].pieceStart[1] = destination[1] + 1;
            l++;
					}
				}
				std::cout << "after KILL" << std::endl;
			}

			//Ad possibilities to array
			//posibilitiesStepsArrayAUX.steps[0] = pos1;posibilitiesStepsArrayAUX.steps[1] = pos2; posibilitiesStepsArrayAUX.steps[2] = pos3; posibilitiesStepsArrayAUX.steps[3] = pos4; posibilitiesStepsArrayAUX.steps[4] = pos5; posibilitiesStepsArrayAUX.steps[5] = pos6; posibilitiesStepsArrayAUX.steps[6] = pos7; posibilitiesStepsArrayAUX.steps[7] = pos8;
			std::cout << "l = " << l << std::endl;
			posibilitiesStepsArrayAUX.index = l - 1;

			// Print possible moves
			/*std::cout <<"Possible PAWN INIT:";
			std::string a, b;
			std::cout << '\n';
			a = pos1.pieceStart[0];
			b = pos1.pieceStart[1];
			std::cout << a;
			std::cout << b+'\n';

			a = pos2.pieceStart[0];
			b = pos2.pieceStart[1];
			std::cout << a;
			std::cout << b + '\n';

			a = pos3.pieceStart[0];
			b = pos3.pieceStart[1];
			std::cout << a;
			std::cout << b + '\n';

			a = pos4.pieceStart[0];
			b = pos4.pieceStart[1];
			std::cout << a;
			std::cout << b + '\n';

			a = pos5.pieceStart[0];
			b = pos5.pieceStart[1];
			std::cout << a;
			std::cout << b + '\n';

			a = pos6.pieceStart[0];
			b = pos6.pieceStart[1];
			std::cout << a;
			std::cout << b + '\n';

			a = pos7.pieceStart[0];
			b = pos7.pieceStart[1];
			std::cout << a;
			std::cout << b + '\n';

			a = pos8.pieceStart[0];
			b = pos8.pieceStart[1];
			std::cout << a;
			std::cout << b + '\n';*/
			break;
		}
		

		return posibilitiesStepsArrayAUX;
	}

	void Ommit_headers(std::ifstream& file) {

		std::string line;
		while (getline(file, line)) {
			std::cout << line;
			if (line.compare("") == 0) {
				break;
			}


		}
	}

	void simulatePromotion(char endPos[], char *piece) {
		//simulateMove(startPos, endPos);
		std::cout << "simulatePromotion " << endPos[0] << endPos[1] << "=" << piece << std::endl;
		std::cout << SimulationMatrix[endPos[1] - '1'][endPos[0] - 'a']->pieceType[1] << std::endl;
        SimulationMatrix[endPos[1] - '1'][endPos[0] - 'a']->pieceType[0] = piece[0];
        SimulationMatrix[endPos[1] - '1'][endPos[0] - 'a']->pieceType[1] = piece[1];
		//strcpy(SimulationMatrix[endPos[1] - '1'][endPos[0] - 'a']->pieceType + 1, &piece);
		//SimulationMatrix[endPos[1] - '1'][endPos[0] - 'a']->pieceType[1] = piece;
	}

	bool checkIfInitPos(char piece, char color, bool *directions, char startPos[], int idx, int d) {
		if (directions[idx % d]) {
			if ((SimulationMatrix[startPos[1] - '1'][startPos[0] - 'a']->pieceType[1] == piece) &&
				(SimulationMatrix[startPos[1] - '1'][startPos[0] - 'a']->pieceType[0] == color)) {
				memset(directions, 0, sizeof(directions));
				return true;
			} else {
				std::cout << "checkIfInitPos " << piece << color << " " << SimulationMatrix[startPos[1] - '1'][startPos[0] - 'a']->pieceType[1] << SimulationMatrix[startPos[1] - '1'][startPos[0] - 'a']->pieceType[0] << std::endl;
				directions[idx % d] = false;
				return false;
			}
		} else {
			return false;
		}
	}
	void simulateMove(char from[], char to[]) {
		printSimulation();
		std::cout << "simulateMove" << std::endl;
		//std::cout << SimulationMatrix[from[0] - 'a'][from[1] - '1']->pos.z << std::endl;
		//std::cout << SimulationMatrix[from[0] - 'a'][from[1] - '1']->pos.x << std::endl;
		std::cout << SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.z << std::endl;
		std::cout << SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.x << std::endl;
		int posZ = SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.z;
		int posX = SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.x;
		SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.z = to[0] - 'a' + 1;
		SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.x = to[1] - '1' + 1;
		std::cout << SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.z << std::endl;
		std::cout << SimulationMatrix[from[1] - '1'][from[0] - 'a']->pos.x << std::endl;
		SimulationMatrix[to[1] - '1'][to[0] - 'a'] = SimulationMatrix[from[1] - '1'][from[0] - 'a'];
		SimulationMatrix[from[1] - '1'][from[0] - 'a'] = NULL;
		printSimulation();
	}

	std::string Know_init_pos(char destination[], char piece, char color, char info, char promotedTo) {

		//Create an array of possible initial positions
		StepsArray posibilitiesStepsArrayAUX = GetPosibilities(destination, piece, color, info == 'x', SimulationMatrix);
		std::cout << posibilitiesStepsArrayAUX.index << std::endl;

		//For every possible position check if the piece is in that position to verify (also need the infomation if its a kill movement(for pawns)
		//(having an x in the step) and if the piece is B or W (first step or second step))
		int counter = 0;
		//bool goodStepIndex[8] = { 0,0,0,0,0,0,0,0};
		int good = 0;
		bool directions[9] = { true, true, true, true, true, true, true, true };

		for (int i = 0; i <= posibilitiesStepsArrayAUX.index; i++) {
			std::cout << posibilitiesStepsArrayAUX.steps[i].pieceStart[0] << posibilitiesStepsArrayAUX.steps[i].pieceStart[1] << std::endl;
			char x = posibilitiesStepsArrayAUX.steps[i].pieceStart[0];
			char y = posibilitiesStepsArrayAUX.steps[i].pieceStart[1];
			//Now only prints what we found in that possible initial position
			if (x >= 'a' && x <= 'h' && y >= '1' && y <= '8') {
				if (SimulationMatrix[y - '1'][x - 'a'] == NULL) {
					//std::cout << "NO HAY NADA EN " << x << y << std::endl;
				} else {
					if ((piece == 'R') || (piece == 'B') || (piece == 'Q')) {
						if (checkIfInitPos(piece, color, directions, posibilitiesStepsArrayAUX.steps[i].pieceStart, i, (piece == 'Q') ? 8 : 4)) {
							counter++;
							good = i;
						}
						std::cout << "directions: " << directions[0] << directions[1] << directions[2] << directions[3] << directions[4] << directions[5] << directions[6] << directions[7] << directions[8] << std::endl;
					} else {
						std::cout << SimulationMatrix[y - '1'][x - 'a']->pieceType[1] << " should be " << piece << std::endl;
						if ((SimulationMatrix[y - '1'][x - 'a']->pieceType[1] == piece) &&
							(SimulationMatrix[y - '1'][x - 'a']->pieceType[0] == color)) {
							if ((info != 'x') && (info != '\0')) {
								if (x == info) {
									counter++;
									good = i;
								}
							} else {
								counter++;
								good = i;
							}
							//goodStepIndex[i] = true;
						}
					}
				}
			}
		}

		if (counter < 1) {
			std::cout << "UPS... None of posibilities is good :(" << std::endl;
			throw "UPS... None of posibilities is good :(";
		} else if (counter == 1) {
			std::cout << "YAY! Only one option! " << posibilitiesStepsArrayAUX.steps[good].pieceStart[0] << posibilitiesStepsArrayAUX.steps[good].pieceStart[1] << std::endl;
			simulateMove(posibilitiesStepsArrayAUX.steps[good].pieceStart, destination);
		} else {
			std::cout << "AHR... More than one posibility is correct :/" << std::endl;
			throw "AHR... More than one posibility is correct :/";
		}
		if (promotedTo) { // TODO needs change
			if (color == 'B' && promotedTo == 'Q') {
				simulatePromotion(destination, "BQ");
			}
			if (color == 'W' && promotedTo == 'Q') {
				simulatePromotion(destination, "WQ");
			}
		}
		return posibilitiesStepsArrayAUX.steps[good].pieceStart;
	}

	public :Step QCastling(char color) {
		Step s;
		if (color == 'W') {
			s.pieceStart[0] = 'e';
			s.pieceStart[1] = '1';
			s.pieceEnd[0] = 'c';
			s.pieceEnd[1] = '1';
			s.castling = true;
			s.rookStart[0] = 'a';
			s.rookStart[1] = '1';
			s.rookEnd[0] = 'd';
			s.rookEnd[1] = '1';
			simulateMove("e1", "c1");
			simulateMove("a1", "d1");

		} else {
			s.pieceStart[0] = 'e';
			s.pieceStart[1] = '8';
			s.pieceEnd[0] = 'c';
			s.pieceEnd[1] = '8';
			s.castling = true;
			s.rookStart[0] = 'a';
			s.rookStart[1] = '8';
			s.rookEnd[0] = 'd';
			s.rookEnd[1] = '8';
			simulateMove("e8", "c8");
			simulateMove("a8", "d8");
		}
		return s;
	}

	public:Step KCastling(char color) {
		Step s;
		if (color == 'W') {
			s.pieceStart[0] = 'e';
			s.pieceStart[1] = '1';
			s.pieceEnd[0] = 'g';
			s.pieceEnd[1] = '1';
			s.castling = true;
			s.rookStart[0] = 'h';
			s.rookStart[1] = '1';
			s.rookEnd[0] = 'f';
			s.rookEnd[1] = '1';
			simulateMove("e1", "g1");
			simulateMove("h1", "f1");
		}
		else {
			s.pieceStart[0] = 'e';
			s.pieceStart[1] = '8';
			s.pieceEnd[0] = 'g';
			s.pieceEnd[1] = '8';
			s.castling = true;
			s.rookStart[0] = 'h';
			s.rookStart[1] = '8';
			s.rookEnd[0] = 'f';
			s.rookEnd[1] = '8';
			simulateMove("e8", "g8");
			simulateMove("h8", "f8");
		}
		return s;
	}

	public :Step GetStep(char array_s[], char color) {
		Step return_step;
		std::string aux1, aux2, row;
		bool capture = false;
		char aditional_info = '\0';
		return_step.promotedTo = '\0';

		//STORE DESTINATION
		if (strstr(array_s, "=") != NULL) { // When a pawn reach to the other side of the board
			return_step.pieceEnd[0] = array_s[strlen(array_s) - 4];
			return_step.pieceEnd[1] = array_s[strlen(array_s) - 3];
			return_step.promotion = true;
			return_step.promotedTo = array_s[strlen(array_s) - 1];
		} else {
			if (strstr(array_s, "+") != NULL) { // When a piece do check
				return_step.pieceEnd[0] = array_s[strlen(array_s) - 3];
				return_step.pieceEnd[1] = array_s[strlen(array_s) - 2];
			} else {
				if (strlen(array_s) > 3) {
					aditional_info = return_step.pieceEnd[0] = array_s[strlen(array_s) - 3];
				}
				return_step.pieceEnd[0] = array_s[strlen(array_s) - 2];
				return_step.pieceEnd[1] = array_s[strlen(array_s) - 1];
			}
		}

		std::cout << array_s[0] << array_s[1] << std::endl;
		if (strstr(array_s, "-O-") != NULL) {
			std::cout << "QCastling" << std::endl;
			return QCastling(color);
		}

		if ((strstr(array_s, "-O-") == NULL) && (strstr(array_s, "O-O") != NULL)) {
			std::cout << "KCastling" << std::endl;
			return KCastling(color);
		}
		std::cout << "STORE OBJET [0]" << return_step.pieceEnd[0] << " [1]" << return_step.pieceEnd[1] << std::endl;
		//END STORE DESTINATION

		//STORE INITIAL POSITION

		char piece = (array_s[0] < 'a') ? array_s[0] : 'P';

		std::string init_pos = Know_init_pos(return_step.pieceEnd, piece, color, aditional_info, return_step.promotedTo);
		/*if ((strstr(array_s, "x") == NULL) && (strstr(array_s, "+") == NULL) && (strstr(array_s, "=") == NULL) && (strlen(array_s) == 4)) {
			// If we dont have x + or = symbol and the lenght is 4 means that we have a turn with "Nda3" form (the Knight that is in d colum goes to a3).
			std::cout << " Search in row "; // TODO: different kind of finding for this special situation?
			std::string init_pos = Know_init_pos(return_step.pieceEnd, piece, color, aditional_info, return_step.promotedTo);
		} else {
			std::string init_pos = Know_init_pos(return_step.pieceEnd, piece, color, aditional_info, return_step.promotedTo);
		}*/

		//END STORE INITIAL POSITION

		std::cout << '\n';
		return_step.pieceStart[0] = init_pos[0];
		return_step.pieceStart[1] = init_pos[1];

		return return_step;
	}

	public :StepsArray Read_Steps(std::string fileStr){
		StepsArray steps_array_return;
		steps_array_return.active = 0;
		int steps_index = 0;

		std::ifstream ifs;
		ifs.open(fileStr, std::ifstream::in);

		if (ifs.is_open()) {
			// Each PGN file has a header useless for us, we ommit them
			Ommit_headers(ifs);

			int turn = 0;
			char c='x';

			std::cout << '\n';
		
			while (!ifs.eof()) { // In every iteration we extract info from 1. FIRST TURN SECOND TURN | iteration 2. FIRST TURN SECOND TURN
			
				while ((c != '.') && !ifs.eof()) { //ommit number of turn, dot and space of every turn
					std::cout << c;
					c = ifs.get();
				}

				if (ifs.eof()) {
					std::cout << "DONE!" << std::endl;
					ifs.close();
					std::cout << steps_array_return.index << std::endl;
					return steps_array_return;
				}

				c = ifs.get(); // space after dot
				c = ifs.get(); // first sign of step
				std::cout << c << std::endl;
				// Read chars refering to the step
				char step_array[2][6] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

				// two turns
				for (int k = 0; k < 2; k++) {
					//TURN
					for (int i = 0; c != ' ' && c != '\n'; i++) {
						step_array[k][i] = c;
						c = ifs.get();
					}
					turn++;
					//PRINT STEPS
					//std::cout << '\n';
					//std::string aux, aux2, aux3, aux4, aux5;
					//aux = step_array[0];
					//aux2 = step_array[1];aux3 = step_array[2];aux4 = step_array[3];aux5 = step_array[4];
					std::cout << " El paso " << turn << " es: " << step_array[k] << " | " << "Sentence entered " << strlen(step_array[k]) << " long\n"; // aux + aux2 + aux3 + aux4 + aux5 + "|";


					//printf("Sentence entered %u long.\n", (unsigned)strlen(step_array));


					// Return a step object with initial position and destination position
					char color = turn % 2 == 1 ? 'W' : 'B';
                    Step step = GetStep(step_array[k], color);

					steps_array_return.steps[steps_index] = step;
					steps_index++;
					steps_array_return.index = steps_index;

					//Avoid end of line and strange cases
					if (c == '\n') {
						c = ifs.get();
						while (c == '\n' || c == ' ') {
							c = ifs.get();
						}
					} else {
						c = ifs.get();
					}
				}
			}
		} else {
			// show message:
			std::cout << "Error opening file";
		}

		//system("pause");
		ifs.close();
		return steps_array_return;
	}

	 
};












