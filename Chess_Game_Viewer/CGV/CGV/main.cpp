
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#ifdef _MSC_VER
#include "dirent.h"
#else
#include <dirent.h>
#endif

// Include GLEW
#include "GL/glew.h"

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
int width = 960, height = 720;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include IMGUI

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "Object.cpp"

void drawOBJ(Object &obj, ProjMatrix &PM)
{
    //    if (dynamic)
    //    {
    PM.MVP = PM.ProjectionMatrix * PM.ViewMatrix * glm::translate(glm::mat4(), obj.pos);
    glUniformMatrix4fv(PM.MatrixID, 1, GL_FALSE, &PM.MVP[0][0]);
    
    glm::vec3 lightPos = glm::vec3(7 - obj.pos.x,12 - obj.pos.y,7 - obj.pos.z);
    glUniform3f(PM.LightID, lightPos.x, lightPos.y, lightPos.z);
    //    }
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *obj.texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(*obj.textureID, 0);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, obj.vb);
    glVertexAttribPointer(
                          0,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, obj.uvb);
    glVertexAttribPointer(
                          1,                                // attribute
                          2,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, obj.nb);
    glVertexAttribPointer(
                          2,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(obj.v.size()) );
    //glDrawElements(GL_VERTEX_ARRAY, vertices.size(), GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
}

void setInitialPos(Object *WPieces, Object *BPieces)
{
    // Set positions for all Pawns
    // X - move forward - rows / Y - move up / Z - move to the side - columns
    for (int i = 0; i < 8; i++) {
        WPieces[i].setPos(2.0f, 0.0f, 2.0f*i);
    }
    // Set inital positions for all pieces
    WPieces[8].setPos(0.0f, 0.0f, 0.0f);
    WPieces[9].setPos(0.0f, 0.0f, 14.0f);
    WPieces[10].setPos(0.0f, 0.0f, 2.0f);
    WPieces[11].setPos(0.0f, 0.0f, 12.0f);
    WPieces[12].setPos(0.0f, 0.0f, 4.0f);
    WPieces[13].setPos(0.0f, 0.0f, 10.0f);
    WPieces[14].setPos(0.0f, 0.0f, 6.0f);
    WPieces[15].setPos(0.0f, 0.0f, 8.0f);
    
    // Set positions for all Pawns
    for (int i = 0; i < 8; i++) {
        BPieces[i].setPos(12.0f, 0.0f, 2.0f*i);
    }
    // Set positions for black pieces
    BPieces[8].setPos(14.0f, 0.0f, 0.0f);
    BPieces[9].setPos(14.0f, 0.0f, 14.0f);
    BPieces[10].setPos(14.0f, 0.0f, 2.0f);
    BPieces[11].setPos(14.0f, 0.0f, 12.0f);
    BPieces[12].setPos(14.0f, 0.0f, 4.0f);
    BPieces[13].setPos(14.0f, 0.0f, 10.0f);
    BPieces[14].setPos(14.0f, 0.0f, 6.0f);
    BPieces[15].setPos(14.0f, 0.0f, 8.0f);
}

void initPieces(Object *WPieces, Object *BPieces, const GLuint &Texture_White, const GLuint &Texture_Black, const GLuint &TextureID) {
	//Load the obj file and copy the data for all Pawns
	WPieces[0].load("resources/CB_Pawn.obj", "WP", Texture_White, TextureID);
	for (int i = 0; i < 8; ++i)
		WPieces[i].load("resources/CB_Pawn.obj", "WP", Texture_White, TextureID);

	WPieces[8].load("resources/CB_Rook.obj", "WR", Texture_White, TextureID);
	WPieces[9] = WPieces[8];
	WPieces[10].load("resources/CB_Knight.obj", "WN", Texture_White, TextureID);
	WPieces[11] = WPieces[10];
	WPieces[12].load("resources/CB_Bishop.obj", "WB", Texture_White, TextureID);
	WPieces[13] = WPieces[12];
	WPieces[14].load("resources/CB_Queen.obj", "WQ", Texture_White, TextureID);
	WPieces[15].load("resources/CB_King.obj", "WK", Texture_White, TextureID);

	//Load the obj file and copy the data for all Pawns
	for (int i = 0; i < 8; ++i)
		BPieces[i].load("resources/CB_Pawn.obj", "BP", Texture_Black, TextureID);

	BPieces[8].load("resources/CB_Rook.obj", "BR", Texture_Black, TextureID);
	BPieces[9] = BPieces[8];
	BPieces[10].load("resources/CB_KnightB.obj", "BN", Texture_Black, TextureID);
	BPieces[11] = BPieces[10];
	BPieces[12].load("resources/CB_Bishop.obj", "BB", Texture_Black, TextureID);
	BPieces[13] = BPieces[12];
	BPieces[14].load("resources/CB_Queen.obj", "BQ", Texture_Black, TextureID);
	BPieces[15].load("resources/CB_King.obj", "BK", Texture_Black, TextureID);
}

void reloadPawns(Object *WPieces, Object *BPieces, GLuint &Texture_White, const GLuint &Texture_Black, const GLuint &TextureID) {
	for (int i = 0; i < 8; ++i) {
		if (WPieces[i].needReload) {
			WPieces[i].del();
			WPieces[i].load("resources/CB_Pawn.obj", "WP", Texture_White, TextureID);
			WPieces[i].needReload = false;
		}
		if (BPieces[i].needReload) {
			BPieces[i].del();
			BPieces[i].load("resources/CB_Pawn.obj", "BP", Texture_Black, TextureID);
			BPieces[i].needReload = false;
		}
	}
}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifndef _WIN32
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( width, height, "Chess Game Viewer", NULL, NULL); //1024, 768
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Don't hide cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, width/2, height/2);
    
    // Dark blue background
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "shaders/TransformVertexShader.vertexshader", "shaders/TextureFragmentShader.fragmentshader" );
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    
    // Load the texture
    GLuint Texture_Black = loadBMP_custom("resources/DW.bmp");
    GLuint Texture_White = loadBMP_custom("resources/LW.bmp");
    
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    
    Object Board[2];
    Board[0].load("resources/CB_BParts.obj", "BD", Texture_Black, TextureID);
    Board[1].load("resources/CB_WParts.obj", "BD", Texture_White, TextureID);
    
    //Create an array for all white pieces
    // 0 to 7 - Pawn / 8 and 9 - Rook / 10 and 11 - Knight / 12 and 13 - Bishop / 14 - Queen / 15 - King
    Object WPieces[16], BPieces[16];
    
	initPieces(WPieces, BPieces, Texture_White, Texture_Black, TextureID);
    
    setInitialPos(WPieces, BPieces);
    
    BoardMatrix boardMatrix;
    boardMatrix.init(WPieces, BPieces);
    boardMatrix.print();
    
    // Setup ImGui binding
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    // Setup style
    ImGui::StyleColorsLight();
    
    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    GLfloat LigthIntensity = glGetUniformLocation(programID, "LightIntensity");
    glUniform1f(LigthIntensity, 70.0f);
    
    //    glm::vec3 lightPos = glm::vec3(7,15,7);
    //    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    
    static bool menuOpen = false, menuDebug = false, menuEndGame = false, menuAbout = false;
    static bool mouseControl = false;
    static int mouseButton = GLFW_MOUSE_BUTTON_MIDDLE;
    static int oldState = GLFW_RELEASE;//, oldStateC = GLFW_RELEASE;
	bool movingPiece = false, movingBack = false, movingForward = false;
	int back = 0, forward = 0;
	bool promoting = false;
	
    std::string header = "Load PGN";
    std::string recentList[3] = {"./pgn/grafl_jones_2018.pgn", "./pgn/rathnakaran_arjun_2017.pgn", "./pgn/jackson_jones_2016.pgn"};
    StepsArray ax;
    
    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // update window size
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        // prepare frame for ImGui
        ImGui_ImplGlfwGL3_NewFrame();
        
        // Use our shader
        glUseProgram(programID);
        
        glm::vec3 camPos;
        //        char buffer[10];
        // Compute the MVP matrix from keyboard and mouse input
        camPos = computeMatricesFromInputs(mouseButton);
        
        ProjMatrix PM;
        PM.setLightID(LightID);
        PM.uniform(MatrixID, ViewMatrixID, ModelMatrixID);
        
        //Fixed parts - Chess Board
        for (int i = 0; i < 2; i++) {
            drawOBJ(Board[i], PM);
        }
        
        // Draw all the pieces
        for (int i = 0; i < 16; i++) {
            drawOBJ(WPieces[i], PM);
            drawOBJ(BPieces[i], PM);
        }
        
        // when Space is pressed the software loads the next turn and set the flag movingPiece=True indicating a new movement
        // oldState enables the function only when the key is pressed, not while it is pressed
        int nextStep = glfwGetKey( window, GLFW_KEY_RIGHT );
        if (nextStep == GLFW_PRESS && oldState == GLFW_RELEASE && !movingPiece && ax.index > 0 && !menuEndGame){
            movingPiece = true;
			movingForward = true;
			forward++;
            //call function to read file and return which piece to move and where to move it
            //            movingPiece, pieceToMove, whereToMove = getNextTurn(boardMatrix);
            printf("Key Right Pressed\n");
            oldState = GLFW_PRESS;
        }
        else if (nextStep == GLFW_RELEASE && oldState == GLFW_PRESS) oldState = GLFW_RELEASE;

		int previousStep = glfwGetKey(window, GLFW_KEY_LEFT);
		if (previousStep == GLFW_PRESS && oldState == GLFW_RELEASE && !movingPiece && ax.index > 0) {
			movingPiece = true;
			movingBack = true;
			back++;
			//call function to read file and return which piece to move and where to move it
			//            movingPiece, pieceToMove, whereToMove = getNextTurn(boardMatrix);
			printf("Key Left Pressed\n");
			oldState = GLFW_PRESS;
		}
		else if (previousStep == GLFW_RELEASE && oldState == GLFW_PRESS) oldState = GLFW_RELEASE;

        if (movingPiece) {
			if (movingBack) {
				std::cout << "movingBack " << back << std::endl;
				ax.active = min(max(ax.active - min(back - 1, 1), 0), ax.index);
			}
			if (movingForward) {
				std::cout << "movingForward " << forward << std::endl;
				ax.active = min(ax.active + min(forward - 1, 1), ax.index - 1);
			}

			if (ax.active == ax.index -1) {
				std::cout << "IT IS THE END OF GAME" << std::endl;
                menuEndGame = true;
            } else menuEndGame = false;

			std::cout << "active = " << ax.active << std::endl;
			if ((ax.active < ax.index) && (ax.active >= 0)) {
				Step activeStep = ax.steps[ax.active];
				std::cout << activeStep.pieceStart[0] << activeStep.pieceStart[1] << " to " << activeStep.pieceEnd[0] << activeStep.pieceEnd[1] << std::endl;
				char * pieceStart, *pieceEnd, *rookStart, *rookEnd;
				char promotedTo;
				if (movingBack) {
					pieceStart = activeStep.pieceEnd;
					pieceEnd = activeStep.pieceStart;
					if (activeStep.castling) {
						rookStart = activeStep.rookEnd;
						rookEnd = activeStep.rookStart;
					}
					if (activeStep.promotion) {
						promotedTo = 'P';
					}
					forward = 0;
					movingBack = false;
				}
				if (movingForward) {
					pieceStart = activeStep.pieceStart;
					pieceEnd = activeStep.pieceEnd;
					if (activeStep.castling) {
						rookStart = activeStep.rookStart;
						rookEnd = activeStep.rookEnd;
					}
					if (activeStep.promotion) {
						promotedTo = activeStep.promotedTo;
					}
					back = 0;
					movingForward = false;
				}
				if (activeStep.castling) {
					movingPiece = boardMatrix.castling(pieceStart, pieceEnd, rookStart, rookEnd);
				}
				else if (activeStep.promotion) {
					movingPiece = boardMatrix.promotion(pieceStart, pieceEnd, promotedTo, forward > 0, activeStep.capture);
				}
				else {
					std::cout << "move piece" << std::endl;
					movingPiece = boardMatrix.move(pieceStart, pieceEnd, forward > 0, activeStep.capture);
				}
			}
            if (!movingPiece) boardMatrix.print();
        }
        
        // creates the menu bar
        {
            static bool b = false;
            
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open PGN", "Ctrl+O")) {
                        {
                            menuOpen = true;
                        }
                    }
                    if (ImGui::BeginMenu("Open Recent"))
                    {
                        ImGui::TextDisabled(recentList[0].c_str());
                        ImGui::TextDisabled(recentList[1].c_str());
                        ImGui::TextDisabled(recentList[2].c_str());
                        
                        ImGui::EndMenu();
                    }
                    if (ImGui::MenuItem("Quit", "Esc")) glfwSetWindowShouldClose(window, 1);
                    
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::BeginMenu("Texture"))
                    {
                        if (ImGui::MenuItem("Marble", NULL, b) && b == false){
                            Texture_Black = loadBMP_custom("resources/DM.bmp");
                            Texture_White = loadBMP_custom("resources/LM.bmp");
                            b = !b;
                        }
                        if (ImGui::MenuItem("Wood", NULL, !b) && b == true){
                            Texture_Black = loadBMP_custom("resources/DW.bmp");
                            Texture_White = loadBMP_custom("resources/LW.bmp");
                            b = !b;
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Lighting"))
                    {
                        static float f = 70.0f;
                        
                        //                        if (ImGui::MenuItem("Color", NULL, false)){
                        //
                        //                        }
                        if (ImGui::SliderFloat("Intensity", &f, 0.0f, 200.0f)){
                            glUniform1f(LigthIntensity, f);
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Mouse"))
                    {
                        if (ImGui::MenuItem("Rotation Middle Button", NULL, !mouseControl) && mouseControl) {
                            mouseControl = !mouseControl;
                            mouseButton = GLFW_MOUSE_BUTTON_MIDDLE;
                        };
                        if (ImGui::MenuItem("Rotation Right Button", NULL, mouseControl) && !mouseControl) {
                            mouseControl = !mouseControl;
                            mouseButton = GLFW_MOUSE_BUTTON_RIGHT;
                        };
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                    
                }
                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("About", NULL, menuAbout)) menuAbout = !menuAbout;
                    if (ImGui::MenuItem("Debug", NULL, menuDebug)) menuDebug = !menuDebug;
                    ImGui::EndMenu();
                }
                
                ImGui::EndMainMenuBar();
            }
        }
        
        // creates the bottom action buttons
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoScrollbar;
            window_flags |=ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoNav;
            ImGui::Begin("Buttons", 0, window_flags);
            ImGui::SetWindowPos(ImVec2 (width/2 - 160/2, height - 45));
            ImGui::SetWindowSize(ImVec2 (160, 35));
//            if (ImGui::Button("   <<  "))
//            {
//
//            }
            ImGui::SameLine();
            if (ImGui::Button("   <   ") && !movingPiece && ax.index > 0)         // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            {
				movingBack = true;
				back++;
				movingPiece = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("   >   ") && !movingPiece && ax.index > 0 && !menuEndGame)         // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            {
				movingForward = true;
				forward++;
                movingPiece = true;
            }
            ImGui::SameLine();
//            if (ImGui::Button("   >>  "))
//            {
//
//            }
            ImGui::End();
        }
        
        // debug window
        if (menuDebug) {
            ImGui::SetNextWindowBgAlpha(0.2);
            ImGui::Begin("Debug", NULL);
            ImGui::Text("Cam.X %.2f, Cam.Y %.2f, Cam.Z %.2f", camPos.x, camPos.y, camPos.z);
            ImGui::Text("Moving piece: %s", movingPiece ? "True":"False");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
        // About window
        if (menuAbout) {
            ImGui::SetNextWindowBgAlpha(1.0);
            ImGui::Begin("About", NULL);
            ImGui::Text("                       Chess Game Viewer v1.0\n\n");
            ImGui::Text("Developed by:");
            ImGui::Text("Diego Apestegui");
            ImGui::Text("Elias Kasma Piovani");
            ImGui::Text("Joanna Wiszowata");
            ImGui::Text("Nicolas Santullo");
            ImGui::Text("Moran Gybels");
            ImGui::Text("Zhoobin Jalili");
            ImGui::Text("\n                     Project Based Learning\nFor the Master in Computer Engineering, UAB. Year 2018.");
            ImGui::Text("Mentoring: Professor Enric Marti Godia - enric.marti@uab.cat");
            ImGui::End();
        }
        
        // End game window
        if (menuEndGame) {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |=ImGuiWindowFlags_NoTitleBar;
            ImGui::SetNextWindowBgAlpha(1.0);
            ImGui::SetNextWindowSize(ImVec2 (130,35));
            ImGui::SetNextWindowPos(ImVec2 (width/2 - 65, 30));
            ImGui::Begin("", NULL, window_flags);
            ImGui::Text("End of the game");
            ImGui::End();
        }
        
        // Game information
        if (true) {
            ImVec4 color = ImColor(255, 255, 255, 255);
            ImGui::SetNextWindowBgAlpha(0.0);
            ImGui::Begin("Game Information", NULL);
            ImGui::TextColored(color, "%s", header.c_str());
            ImGui::End();
        }
        
        // create a open file menu
        if (menuOpen) {
            ImGuiWindowFlags window_flags = 0;
            ImGui::SetNextWindowBgAlpha(1.0);
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |=ImGuiWindowFlags_NoTitleBar;
            ImGui::Begin("Open File", NULL, window_flags);
            ImGui::SetWindowSize(ImVec2 (264, 264));
            
            ImGui::Text("Select a PGN file:");
            
            ImGui::BeginChild("child", ImVec2(0, 180), true);
            char file[1024];
            struct dirent *ent;
            
            // read folder and get all files .pgn inside that
            // read files in the directory PGN
            DIR *dir;
            if ((dir = opendir ("pgn")) != NULL) {
                // print all the files .pgn inside the menu item
                while ((ent = readdir (dir)) != NULL) {
                    
                    std::size_t is_pgn = std::string(ent->d_name).find(".pgn");
                    
                    if (is_pgn!=std::string::npos && ImGui::MenuItem(ent->d_name)) memcpy(file, ent->d_name, strlen(ent->d_name)+1);
                }
                closedir (dir);
            } else {
                // could not open directory
                perror ("Could not open directory pgn");
                return EXIT_FAILURE;
            }
            
            ImGui::EndChild();
            
            file[0] != '\0' ? ImGui::Text("Open: %s", file) : ImGui::Text("Open: ");
            
            // when cancel is pressed, just close the window
            if (ImGui::Button("Cancel"))
            {
                menuOpen = false;
            }
            ImGui::SameLine();
            // button open
            if (ImGui::Button("Open"))
            {
                char filePath[1024] = "./pgn/";
                strcat(filePath, file);
                //                strcat(filePath, ".pgn");
                
                // open file defined by pgn/ file
                printf("%s\n", filePath);
                ax.index = 0;
                ax.active = 0;
                back, forward = 0;
                menuEndGame = false;
				reloadPawns(WPieces, BPieces, Texture_White, Texture_Black, TextureID);
                ax = boardMatrix.Read_Steps(filePath, header);
                // reset board to the initial positions
                setInitialPos(WPieces, BPieces);
                boardMatrix.init(WPieces, BPieces);
                // insert it on the top of the recent list
                
                // verify if list is greatter than 5, if yes delete values
                
                // close menu
                menuOpen = false;
            }
            ImGui::End();
        }
        
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader
    for (int i = 0; i < 2; i++) {
        Board[i].del();
    }
    for (int i = 0; i < 16; i++) {
        WPieces[i].del();
    }
    
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture_Black);
    glDeleteTextures(1, &Texture_White);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    //    // Delete the text's VBO, the shader and the texture
    //    cleanupText2D();
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

