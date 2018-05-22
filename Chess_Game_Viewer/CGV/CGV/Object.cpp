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
    const char *pieceType;
    bool firstMove = true;
    std::vector<glm::vec3> v;
    GLuint vb;
    GLuint uvb;
    GLuint nb;
    const GLuint *texture;
    const GLuint *textureID;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    
    void load(const char *path, const char *piecetype, const GLuint &Texture, const GLuint &TextureID)
    {
        pieceType = piecetype;
        
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
	
};

class StepsArray {
public:
	int index;
	Step steps[50];
//public:
	//void init_StepsArray(int index);
};


class BoardMatrix
{
private:
    Object *Matrix[8][8];
    bool movingPiece = false;
    int nBPiecesDead = 0, nWPiecesDead = 0;
public:
    void init(Object WhitePieces[16], Object BlackPieces[16])
    {
        // Init matrix with Null values
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Matrix[i][j] = NULL;
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

	public :StepsArray GetPosibilities(char destination[], char piece) {
		//For store the possibilities we are going to store only the .init part of each Step object.
		StepsArray posibilitiesStepsArrayAUX;
		posibilitiesStepsArrayAUX.steps[21];
		posibilitiesStepsArrayAUX.index = 0;
		std::string a, b;
		float PI = 3.1415;
		Step pos1;
		Step pos2;
		Step pos3;
		Step pos4;
		Step pos5;
		Step pos6;
		Step pos7;
		Step pos8;
		char letra = destination[0];
		char numero = destination[1];
		int posib = 0;
		std::cout << '\n';

		switch (piece) {
		case 'N': // Knight  Caballo
			std::cout << "Possible Caballo INIT:";
			std::cout << '\n';
			pos1.pieceStart[0] = destination[0] - 2;
			pos1.pieceStart[1] = destination[1] - 1;

			pos2.pieceStart[0] = destination[0] - 2;
			pos2.pieceStart[1] = destination[1] + 1;

			pos3.pieceStart[0] = destination[0] + 2;
			pos3.pieceStart[1] = destination[1] - 1;

			pos4.pieceStart[0] = destination[0] + 2;
			pos4.pieceStart[1] = destination[1] - 2;


			pos5.pieceStart[0] = destination[0] - 1;
			pos5.pieceStart[1] = destination[1] - 2;

			pos6.pieceStart[0] = destination[0] - 1;
			pos6.pieceStart[1] = destination[1] + 2;

			pos7.pieceStart[0] = destination[0] + 1;
			pos7.pieceStart[1] = destination[1] - 2;

			pos8.pieceStart[0] = destination[0] + 1;
			pos8.pieceStart[1] = destination[1] + 2;

			//Add them to array
			posibilitiesStepsArrayAUX.steps[0] = pos1;posibilitiesStepsArrayAUX.steps[1] = pos2; posibilitiesStepsArrayAUX.steps[2] = pos3; posibilitiesStepsArrayAUX.steps[3] = pos4; posibilitiesStepsArrayAUX.steps[4] = pos5; posibilitiesStepsArrayAUX.steps[5] = pos6; posibilitiesStepsArrayAUX.steps[6] = pos7; posibilitiesStepsArrayAUX.steps[7] = pos8;
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
						std::cout << '\n';
						posibilitiesStepsArrayAUX.index = posib;
						
						std::cout << posib;
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
						std::cout << posibilitiesStepsArrayAUX.steps[posib].pieceStart[0];
						std::cout << posibilitiesStepsArrayAUX.steps[posib].pieceStart[1];
						std::cout << '\n';
						//--end print
						posibilitiesStepsArrayAUX.index = posib;
						posib++;
						std::cout << posib;
					}
				}
			}		
			std::cout << posib;
			break;
		case 'K':

			std::cout << "Possible Queen King:";
			std::cout << '\n';

			pos1.pieceStart[0] = destination[0];
			pos1.pieceStart[1] = destination[1] - 1;

			pos2.pieceStart[0] = destination[0];
			pos2.pieceStart[1] = destination[1] + 1;

			pos3.pieceStart[0] = destination[0] - 1;
			pos3.pieceStart[1] = destination[1];

			pos4.pieceStart[0] = destination[0] + 1;
			pos4.pieceStart[1] = destination[1];


			pos5.pieceStart[0] = destination[0] + 1;
			pos5.pieceStart[1] = destination[1] - 1;

			pos6.pieceStart[0] = destination[0] + 1;
			pos6.pieceStart[1] = destination[1] + 1;

			pos7.pieceStart[0] = destination[0] - 1;
			pos7.pieceStart[1] = destination[1] - 1;

			pos8.pieceStart[0] = destination[0] - 1;
			pos8.pieceStart[1] = destination[1] + 1;

			//Add them to array
			posibilitiesStepsArrayAUX.steps[0] = pos1;posibilitiesStepsArrayAUX.steps[1] = pos2; posibilitiesStepsArrayAUX.steps[2] = pos3; posibilitiesStepsArrayAUX.steps[3] = pos4; posibilitiesStepsArrayAUX.steps[4] = pos5; posibilitiesStepsArrayAUX.steps[5] = pos6; posibilitiesStepsArrayAUX.steps[6] = pos7; posibilitiesStepsArrayAUX.steps[7] = pos8;
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

						posibilitiesStepsArrayAUX.index = posib;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[0] = letra;
						posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[1] = numero;
						//Print
						std::cout << posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[0];
						std::cout << posibilitiesStepsArrayAUX.steps[posibilitiesStepsArrayAUX.index].pieceStart[1];
						std::cout << '\n';
					}
				}
			}
			break;
		default: // Pawn
			
			//pos1.pieceStart[1] = destination[0] - 1; sirve para restar una letra
			
			//Normal move
			pos1.pieceStart[0] = destination[0];
			pos1.pieceStart[1] = destination[1] - 1;

			pos2.pieceStart[0] = destination[0];
			pos2.pieceStart[1] = destination[1] + 1;

			pos3.pieceStart[0] = destination[0];
			pos3.pieceStart[1] = destination[1] - 2;

			pos4.pieceStart[0] = destination[0];
			pos4.pieceStart[1] = destination[1] + 2;
			// Kill move diagonal
			pos5.pieceStart[0] = destination[0] + 1;
			pos5.pieceStart[1] = destination[1] - 1;

			pos6.pieceStart[0] = destination[0] + 1;
			pos6.pieceStart[1] = destination[1] + 1;

			pos7.pieceStart[0] = destination[0] - 1;
			pos7.pieceStart[1] = destination[1] - 1;

			pos8.pieceStart[0] = destination[0] - 1;
			pos8.pieceStart[1] = destination[1] + 1;

			//Ad possibilities to array
			posibilitiesStepsArrayAUX.steps[0] = pos1;posibilitiesStepsArrayAUX.steps[1] = pos2; posibilitiesStepsArrayAUX.steps[2] = pos3; posibilitiesStepsArrayAUX.steps[3] = pos4; posibilitiesStepsArrayAUX.steps[4] = pos5; posibilitiesStepsArrayAUX.steps[5] = pos6; posibilitiesStepsArrayAUX.steps[6] = pos7; posibilitiesStepsArrayAUX.steps[7] = pos8;
			posibilitiesStepsArrayAUX.index = 7;

			// Print possible moves
			std::cout <<"Possible PAWN INIT:";
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
			std::cout << b + '\n';
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

	 std::string Know_init_pos(char destination[], char piece) {
		std::string initialPos;

		//Create an array of possible initial positions
		StepsArray posibilitiesStepsArrayAUX = GetPosibilities(destination, piece );
		std::cout << '\n';
		std::cout << posibilitiesStepsArrayAUX.index;
	
		//For every possible position chech if the piece is in that position to verify (also need the infomation if its a kill movement(for pawns)(having an x in the step) and if the piece is B or W (first step or second step))
		for (int i = 0; i < posibilitiesStepsArrayAUX.index; i++) {
			int x = int(posibilitiesStepsArrayAUX.steps[i].pieceStart[0]);
			int y = posibilitiesStepsArrayAUX.steps[i].pieceStart[1];

			//Now only prints what we found in that possible initial position
			if (x >= 'a' && x <= 'h' && y >= '1' && y <= '8') {
				if (Matrix[y - 49][x - 97] == NULL) {
					std::cout << '\n';
					std::cout << "NO HAY NADA ";
					std::cout << " EN ";
					std::cout << (char)x;
					std::cout << (char)y;
				}
				else {
					std::cout << '\n';
					std::cout << "HAY UN ";
					std::cout << Matrix[y - 49][x - 97]->pieceType[1];
					std::cout << " EN ";
					std::cout << (char)x;
					std::cout << (char)y;
					
				}
			}
			
		}
		

		return initialPos;
	}

	public :Step GetStep(char array_s[]) {
	Step return_step;
	std::string aux1, aux2,row;

	//STORE DESTINATION
	if (strstr(array_s, "=") != NULL) { // When a pawn reach to the other side of the board
		return_step.pieceEnd[0] = array_s[strlen(array_s) - 4];
		return_step.pieceEnd[1] = array_s[strlen(array_s) - 3];

		aux1 = return_step.pieceEnd[0];
		aux2 = return_step.pieceEnd[1];

		std::cout << "STORE OBJET [0]" + aux1 + " [1]" + aux2;
	}
	else {
		if (strstr(array_s, "+") != NULL) { // When a piece do check
			return_step.pieceEnd[0] = array_s[strlen(array_s) - 3];
			return_step.pieceEnd[1] = array_s[strlen(array_s) - 2];

			aux1 = return_step.pieceEnd[0];
			aux2 = return_step.pieceEnd[1];

			std::cout << "STORE OBJET [0]" + aux1 + " [1]" + aux2;
		}
		else {
			return_step.pieceEnd[0] = array_s[strlen(array_s) - 2];
			return_step.pieceEnd[1] = array_s[strlen(array_s) - 1];


			aux1 = return_step.pieceEnd[0];
			aux2 = return_step.pieceEnd[1];

			std::cout << "STORE OBJET [0]" + aux1 + " [1]" + aux2;
		}
	}
	//END STORE DESTINATION

	//STORE INITIAL POSITION
	std::cout << '\n';

	char piece = array_s[0];

	if ((strstr(array_s, "x") == NULL) && (strstr(array_s, "+") == NULL) && (strstr(array_s, "=") == NULL) && (strlen(array_s) == 4)) {
		// If we dont have x + or = symbol and the lenght is 4 means that we have a turn with "Nda3" form (the Knight that is in d colum goes to a3).
		std::cout << " Search in row ";
		std::string init_pos = Know_init_pos(return_step.pieceEnd, piece);
	}
	else {
		std::string init_pos = Know_init_pos(return_step.pieceEnd, piece);
	}

	//END STORE INITIAL POSITION

	std::cout << '\n';
	return return_step;
}

	public :StepsArray Read_Steps(std::string fileStr){
	StepsArray steps_array_return;
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
			c = ifs.get();
			c = ifs.get();
			
			// Read chars refering to the step
			char step_array[6] = {NULL,NULL,NULL,NULL,NULL};
			char step_array2[6] ={ NULL,NULL,NULL,NULL,NULL };

			//FIRST TURN
			for (int i = 0; c !=' ' && c != '\n'; i++) {
				step_array[i] = c;
				c = ifs.get();		
			}
			turn++;
			//PRINT STEPS
			std::cout << '\n';
			std::string aux;std::string aux2;std::string aux3;std::string aux4;std::string aux5;
			aux = step_array[0];aux2 = step_array[1];aux3 = step_array[2];aux4 = step_array[3];aux5 = step_array[4];
			std::cout << " El paso 1 es: " + aux + aux2 + aux3 + aux4 + aux5 + "|";
			

			printf("Sentence entered %u long.\n", (unsigned)strlen(step_array));
			

			// Return a step object with initial position and destination position
			Step step = GetStep(step_array);


			std::cout << '\n';
			//Avoid end of line and strange cases
			if (c == '\n') {
				c = ifs.get();
				while (c == '\n' || c == ' ') {
					c = ifs.get();
				}
			}
			else {
				c = ifs.get();
			}

			//SECOND TURN
			for (int i = 0; c != ' ' && c!='\n'; i++) {
				step_array2[i] = c;
				c = ifs.get();
			}

			//PRINT STEPS
			aux = step_array2[0];aux2 = step_array2[1];aux3 = step_array2[2];aux4 = step_array2[3];aux5 = step_array2[4];
			std::cout << "El paso 2 es: " + aux + aux2 + aux3 + aux4 + aux5 + "|";
			printf("Sentence entered %u long.\n", (unsigned)strlen(step_array2));

			turn++;

			// Return a step object with initial position and destination position
			Step step2 = GetStep(step_array2);

			//Store the steps in the array to return
			//steps_array_return.steps[steps_index] = step;
			//steps_index++;
			//steps_array_return.steps[steps_index] = step2;
			//steps_index++;
		}
	}
	else {
		// show message:
		std::cout << "Error opening file";
	}

	system("pause");
	return steps_array_return;
	}

	 
};












