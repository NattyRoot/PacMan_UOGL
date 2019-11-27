#include <Core/Core.h>
#include <UltimateOpenGL_V2/UltimateOpenGL.h>
#include <GLFW/glfw3.h>
#include "Cube.h"

#define v3 glm::vec3

using namespace Upp;

UltimateOpenGL_Context context;
GLFWwindow* window;

void init();
void processInput(GLFWwindow* window, UltimateOpenGL_Context& context);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
Scene& createMainScene();
bool isCollided();
bool CheckCollision(Transform& transPm, Transform& transWall);



//CONSTANTES
float z = -3.0f;
float width = 800.0f;
float height = 800.0f;
float speed = 5.0f;



CONSOLE_APP_MAIN{
	
	init();
	
	//CREATION SCENE, GOs, ETC.
	
	Scene& mainScene = createMainScene();
	
	mainScene.Load();
	
	// -------
	
	while (!glfwWindowShouldClose(window)){
        // Gere les event de clavier / souris
        processInput(window, context);
        
        // Repaint la fenetre + le titre ac les fps
        glfwSetWindowTitle(window, "UltimateOpenGL V2 - " +AsString(context.GetFPS()) +" FPS");

		// Applique la scene active sur le contexte
        context.Draw(); //Drawing the context (active scene)

		// events GLFW de la fenetre
        glfwSwapBuffers(window); 
        glfwPollEvents();
    }
	
	glfwTerminate();
}

void init(){
	StdLogSetup(LOG_COUT|LOG_FILE);
	
	//Initalise glfw pour ouvrir une fenetre
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Anti alliasing sur la fenetre
    
    //Création de la fenetre
    window = glfwCreateWindow(width, height, "Coucou", NULL, NULL);
    // Taille du context UltimateOpenGL
    context.SetScreenSize(width, height);
    
    
    // Pas de window, pas d'appli
    if (window == NULL){
        glfwTerminate();
        Exit(-1);
    }
    
    // Set le contexte OpenGL à la fenetre (UltimateOpenGL -> OpenGL -> GLFW Window)
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Load OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        Cout()  << "Failed to initialize GLAD" << "\n";;
        Exit(-1);
    }
    
    // Transforme le mode d'affichage OpenGL
    glEnable(GL_DEPTH_TEST);
    // Antialiasing sur le contexte OpenGL
    glEnable(GL_MULTISAMPLE);
}

// Evenements de claviers
void processInput(GLFWwindow* window, UltimateOpenGL_Context& context){
    try{
 		GameObject& cube = context.GetActiveScene().GetGameObject<GameObject>("PacMan");
 		
 		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	        glfwSetWindowShouldClose(window, true);
	    
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	        cube.GetTransform().MoveFrom(v3(0.0f, speed * context.GetDeltaTime(), 0.0f));
	    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	        cube.GetTransform().MoveFrom(v3(0.0f, -speed * context.GetDeltaTime(), 0.0f));
	    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	        cube.GetTransform().MoveFrom(v3(-speed * context.GetDeltaTime(), 0.0f , 0.0f));
	    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	        cube.GetTransform().MoveFrom(v3(speed * context.GetDeltaTime(), 0.0f, 0.0f));
    }
    catch(...){
     	Cout() << "Pas de cube";   
    }     
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    context.SetScreenSize(height,width);
    glViewport(0, 0, width, height);
}

Scene& createMainScene(){
	Scene& mainScene = context.AddScene("Main");
	
	mainScene.SetBackGroundColor(v3(1.0f, 1.0f, 1.0f));
	
	mainScene.AddCamera();
	
	// ---------------- Creation PacMan ----------------
	
	Object3D& pm = mainScene.CreateGameObject<Object3D>("PacMan", SquareVertices);
	pm.SetLightAffected(false);
	pm.GetTransform().SetNewPosition(v3(2.0f, 2.0f, z));
	pm.Load();
	
	pm.SetOnDrawFunction([](GameObject& go) {
		if (isCollided()){
 			return;
 		}
		
		v3 buffer = go.GetTransform().GetPosition();
		if (buffer.x > 5.8f){
			buffer.x = 5.8f;
			go.GetTransform().SetNewPosition(buffer);
		}
		else if(buffer.x < -5.8f){
			buffer.x = -5.8f;
			go.GetTransform().SetNewPosition(buffer);
		}
		
		if (buffer.y > 5.8f){
			buffer.y = 5.8f;
			go.GetTransform().SetNewPosition(buffer);
		}
		else if(buffer.y < -5.8f){
			buffer.y = -5.8f;
			go.GetTransform().SetNewPosition(buffer);
		}
	});
	// ------------------------------------------------
	
	
	// ---------------- Creation Walls ----------------
	
	float tmpX = -5.0f;
	float tmpY = -5.0f;
	
	String name = "Wall_";
		
	for (int i = 1; i <= 9; i++){
		Object3D& wall = mainScene.CreateGameObject<Object3D>(name + AsString(i), SquareVertices);
		wall.SetLightAffected(false);
		wall.GetTransform().SetNewPosition(v3(tmpX, tmpY, z));
		wall.GetTransform().ScaleNewValue(glm::vec3(3.0f, 3.0f, 3.0f));
		
		// Coords (5, 5) = Top-right, tout les murs sont construits
		if (tmpY == 5.0f && tmpX == 5.0f){
			break;
		}
		
		if (tmpX == 5.0f){
			tmpX = -5.0f;
			tmpY += 5.0f;	
		}
		else{
			tmpX += 5.0f;
		}
	}
	
	
	// ------------------------------------------------
	
	
	return mainScene;
}

bool isCollided(){
	GameObject& pm = context.GetActiveScene().GetGameObject<GameObject>("PacMan");
	
	String name = "Wall_";
			
	for (int i = 1; i <= 9; i++){
		try{
			if (CheckCollision(pm.GetTransform(), context.GetActiveScene().GetGameObject<GameObject>(name + AsString(i)).GetTransform())){
				Cout() << "COLLISION !!!!!!!!" << EOL;
				return true;
			}
		}catch(...){
			Cout() << "Marche po";
			break;
		}
	}
	return false;
}

bool CheckCollision(Transform& transPm, Transform& transWall){
	/*
	4.96 					+ (0.02f * 1							) >= 5.02					   - (0.02f * 1							 	 )
	5 >= 4.98
	transPm.GetPosition().x + (0.02f * transPm.GetScallabeMatrix().x) >= transWall.GetPosition().x - (0.02f * transWall.GetScallabeMatrix().x)
	*/
	/*
	bool collisionX = transPm.GetPosition().x + transPm.GetScallabeMatrix().x >= transWall.GetPosition().x
				   && transWall.GetPosition().x - transWall.GetScallabeMatrix().x >= transPm.GetPosition().x;
	
	
	bool collisionY = transPm.GetPosition().y + transPm.GetScallabeMatrix().y >= transWall.GetPosition().y 
				   && transWall.GetPosition().y + transWall.GetScallabeMatrix().y >= transPm.GetPosition().y;
	*/
	
	bool collisionX = transPm.GetPosition().x + (0.02f * transPm.GetScallabeMatrix().x) >= transWall.GetPosition().x - (0.02f * transWall.GetScallabeMatrix().x)
				   && transWall.GetPosition().x + (0.02f * transWall.GetScallabeMatrix().x) >= transPm.GetPosition().x - (0.02f * transWall.GetScallabeMatrix().x);
	
	
	bool collisionY = transPm.GetPosition().y + (0.02f * transPm.GetScallabeMatrix().y) >= transWall.GetPosition().y - (0.02f * transWall.GetScallabeMatrix().y)
				   && transWall.GetPosition().y + (0.02f * transWall.GetScallabeMatrix().y) >= transPm.GetPosition().y - (0.02f * transWall.GetScallabeMatrix().y);
	
	return collisionX && collisionY;
}

 