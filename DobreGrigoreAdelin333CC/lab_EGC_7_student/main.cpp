//-------------------------------------------------------------------------------------------------
// Descriere: fisier main
//
// Autor: student
// Data: today
//-------------------------------------------------------------------------------------------------

//incarcator de meshe
#include "lab_mesh_loader.hpp"

//geometrie: drawSolidCube, drawWireTeapot...
#include "lab_geometry.hpp"

//incarcator de shadere
#include "lab_shader_loader.hpp"

//interfata cu glut, ne ofera fereastra, input, context opengl
#include "lab_glut.hpp"


//time
#include <ctime>

#define pi 3.14159265359

class Laborator : public lab::glut::WindowListener{

//variabile
private:
	glm::mat4 model_matrix, view_matrix, projection_matrix;										//matrici 4x4 pt modelare vizualizare proiectie
	glm::mat4 matrice_translatie_1;
	glm::mat4 matrice_translatie_2;
	glm::mat4 matrice_scalare;

	unsigned int mesh_vbo_sphere, mesh_ibo_sphere, mesh_vao_sphere, mesh_num_indices_sphere;	//containere opengl pentru vertecsi, indecsi si stare pentru sfera ce va fi pusa la pozitia luminii

	unsigned int mesh_vbo_ground, mesh_ibo_ground, mesh_vao_ground, mesh_num_indices_ground;	//containere opengl pentru vertecsi, indecsi si stare pentru sol

	glm::vec3 light_position;
	glm::vec3 eye_position;
	unsigned int material_shininess;
	float material_kd;
	float material_ks;

	float amplitudine[3];
	float frecv[3], lungime_unda[3];
	float faza[3], viteza_s[3];
	int timp, flag;
	glm::vec2 centru;

	int i = 0, j, max;
	lab::Mesh* ground;

	// Camera
	lab::Camera camera;

//metode
public:
	
	//constructor .. e apelat cand e instantiata clasa
	Laborator(){
		//setari pentru desenare, clear color seteaza culoarea de clear pentru ecran (format R,G,B,A)
		glClearColor(0.5,0.5,0.5,1);
		glClearDepth(1);			//clear depth si depth test (nu le studiem momentan, dar avem nevoie de ele!)
		glEnable(GL_DEPTH_TEST);	//sunt folosite pentru a determina obiectele cele mai apropiate de camera (la curs: algoritmul pictorului, algoritmul zbuffer)
		
		//incarca un shader din fisiere si gaseste locatiile matricilor relativ la programul creat
		lab::gl_program_shader_gouraud = lab::loadShader("shadere\\shader_gouraud_vertex.glsl", "shadere\\shader_gouraud_fragment.glsl");
	
		//incarca o sfera
		lab::loadObj("resurse\\sphere.obj",mesh_vao_sphere, mesh_vbo_sphere, mesh_ibo_sphere, mesh_num_indices_sphere);	

		ground = myLoadGround_as_Mesh();

		// init camera
		camera.set(glm::vec3(0, 10, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		//lumina & material
		eye_position = glm::vec3(0, 10, 50);
		light_position = glm::vec3(10, 7, -100);
		material_shininess = 30;
		material_kd = 0.5;
		material_ks = 0.5;
		
		amplitudine[0] = 5.0f;
		amplitudine[1] = 5.0f;
		amplitudine[2] = 5.0f;
		lungime_unda[0] = 40.0f;
		lungime_unda[1] = 40.0f;
		lungime_unda[2] = 40.0f;
		viteza_s[0] = 2.0f;
		viteza_s[1] = 2.0f;
		viteza_s[2] = 0.5f;
		
		centru = glm::vec2(10, 10);

		for (int i = 0; i < 3; i++){	
			frecv[i] = 2 * pi / lungime_unda[i];
			faza[i] = viteza_s[i] * 2 * pi / lungime_unda[i];
		}

		timp = 1;

		//matrici de modelare si vizualizare
		model_matrix = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
		//view_matrix = glm::lookAt(eye_position, glm::vec3(0,0,0), glm::vec3(0,1,0));
		matrice_translatie_1 = glm::translate(model_matrix, glm::vec3(light_position.x, light_position.y, light_position.z + 100));
		//desenare wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//destructor .. e apelat cand e distrusa clasa
	~Laborator(){
		//distruge shader
		glDeleteProgram(lab::gl_program_shader_gouraud);

		//distruge sfera
		glDeleteBuffers(1,&mesh_vbo_sphere);
		glDeleteBuffers(1,&mesh_ibo_sphere);
		glDeleteVertexArrays(1,&mesh_vao_sphere);

		//distruge solul
		glDeleteBuffers(1,&mesh_vbo_ground);
		glDeleteBuffers(1,&mesh_ibo_ground);
		glDeleteVertexArrays(1,&mesh_vao_ground);
	}
	//format vertex de mana
	struct MyVertexFormat{
		glm::vec3 pozitie;
		MyVertexFormat(){
			pozitie = glm::vec3(0, 0, 0);
		}
		MyVertexFormat(float px, float py, float pz){
			pozitie = glm::vec3(px, py, pz);
		}
		MyVertexFormat operator=(const MyVertexFormat &rhs){
			pozitie = rhs.pozitie;
			return (*this);
		}
	};

	lab::Mesh* myLoadGround_as_Mesh(){

		//definim containere pentru date
		std::vector<lab::VertexFormat> vertecsi;
		std::vector<unsigned int> indecsi;
	
		vertecsi.clear();

		for (int z = 1; z <= 100; z++){
			for (int x = 1; x <= 100; x++){
				vertecsi.push_back(lab::VertexFormat(x * 10, 0, z * 10));
			}
		}
		for (i = 1; i < 100; i++){
			max = i * 100;
			for (j = max - 100; j < max - 1; j++){
				indecsi.push_back(j); indecsi.push_back(j + 100);
				indecsi.push_back(j + 1);

				indecsi.push_back(j + 1); indecsi.push_back(j + 1 + 100);
				indecsi.push_back(j + 100);
			}
		}

		//vertex array object -> un obiect ce reprezinta un container pentru starea de desenare
		glGenVertexArrays(1, &mesh_vao_ground);
		glBindVertexArray(mesh_vao_ground);

		//vertex buffer object -> un obiect in care tinem vertecsii
		glGenBuffers(1, &mesh_vbo_ground);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo_ground);
		glBufferData(GL_ARRAY_BUFFER, vertecsi.size()*sizeof(lab::VertexFormat), &vertecsi[0], GL_STATIC_DRAW);

		//index buffer object -> un obiect in care tinem indecsii
		glGenBuffers(1, &mesh_ibo_ground);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_ibo_ground);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecsi.size()*sizeof(unsigned int), &indecsi[0], GL_STATIC_DRAW);

		//legatura intre atributele vertecsilor si pipeline, datele noastre sunt INTERLEAVED.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)0);						//trimite pozitii pe pipe 0
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)(sizeof(float)* 3));		//trimite normale pe pipe 1
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(lab::VertexFormat), (void*)(2 * sizeof(float)* 3));	//trimite texcoords pe pipe 2
		
		mesh_num_indices_ground = indecsi.size();
		
		//creez obiect de tip mesh
		lab::Mesh* mesh = new lab::Mesh(mesh_vbo_ground, mesh_ibo_ground, mesh_vao_ground, indecsi.size());

		//return
		return mesh;
	}

	glm::mat4 myTranslate(float tx, float ty, float tz){
		return glm::transpose(glm::mat4(
			1, 0, 0, tx,
			0, 1, 0, ty,
			0, 0, 1, tz,
			0, 0, 0, 1));
	}

	//--------------------------------------------------------------------------------------------
	//functii de cadru ---------------------------------------------------------------------------

	//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
	void notifyBeginFrame(){
	}
	//functia de afisare (lucram cu banda grafica)
	void notifyDisplayFrame(){

		//bufferele din framebuffer sunt aduse la valorile initiale (setate de clear color si cleardepth)
		//adica se sterge ecranul si se pune culoare (si alte propietati) initiala
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//foloseste shaderul
		glUseProgram(lab::gl_program_shader_gouraud);
				
		//trimite variabile uniforme la shader
		view_matrix = camera.getViewMatrix();

		glUniformMatrix4fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "model_matrix"),1,false,glm::value_ptr(model_matrix));
		glUniformMatrix4fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "view_matrix"), 1, false, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "projection_matrix"), 1, false, glm::value_ptr(projection_matrix));
		glUniform3f(glGetUniformLocation(lab::gl_program_shader_gouraud, "light_position"), light_position.x, light_position.y, light_position.z);
		glUniform3f(glGetUniformLocation(lab::gl_program_shader_gouraud, "eye_position"), eye_position.x, eye_position.y, eye_position.z);
		glUniform1i(glGetUniformLocation(lab::gl_program_shader_gouraud, "material_shininess"), material_shininess);
		glUniform1f(glGetUniformLocation(lab::gl_program_shader_gouraud, "material_kd"), material_kd);
		glUniform1f(glGetUniformLocation(lab::gl_program_shader_gouraud, "material_ks"), material_ks);
		glUniform1fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "amplitudine"), 3, &amplitudine[0]);
		glUniform1fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "frecventa"), 3,  &frecv[0]);
		glUniform1fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "faza"), 3,  &faza[0]);
		glUniform2fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "centru"), 2, &centru[0]);
		glUniform1i(glGetUniformLocation(lab::gl_program_shader_gouraud, "timp"), timp/10);
		timp++;
	
		//pune o sfera la pozitia luminii
		flag = 0;
		glUniform1i(glGetUniformLocation(lab::gl_program_shader_gouraud, "flag"), flag);
		glUniformMatrix4fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "model_matrix"), 1, false, glm::value_ptr(matrice_translatie_1));
		lab::setColorObject(1, 1, 0, 0);
		glBindVertexArray(mesh_vao_sphere);
		glDrawElements(GL_TRIANGLES, mesh_num_indices_sphere, GL_UNSIGNED_INT, 0);

		//pune si un sol
		flag = 1;
		glUniform1i(glGetUniformLocation(lab::gl_program_shader_gouraud, "flag"), flag);
		matrice_translatie_2 = glm::translate(model_matrix, glm::vec3(-500, -200, -800));
		glUniformMatrix4fv(glGetUniformLocation(lab::gl_program_shader_gouraud, "model_matrix"), 1, false, glm::value_ptr(matrice_translatie_2));
		
		ground->setColor(0, 0.2, 1);
		lab::drawMesh(ground, 0);
	}
	//functie chemata dupa ce am terminat cadrul de desenare (poate fi folosita pt modelare/simulare)
	void notifyEndFrame(){}
	//functei care e chemata cand se schimba dimensiunea ferestrei initiale
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//reshape
		if(height==0) height=1;
		glViewport(0,0,width,height);
		projection_matrix = glm::perspective(90.0f, (float)width/(float)height,0.1f, 10000.0f);
	}


	//--------------------------------------------------------------------------------------------
	//functii de input output --------------------------------------------------------------------
	
	//tasta apasata
	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){

		if (key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
		if (key_pressed == 32) {	//SPACE
			//reincarca shader
			glDeleteProgram(lab::gl_program_shader_gouraud);
			lab::gl_program_shader_gouraud = lab::loadShader("shadere\\shader_gouraud_vertex.glsl", "shadere\\shader_gouraud_fragment.glsl");
		}

		if (key_pressed == 'h'){
			static bool wire = true;
			wire = !wire;
			glPolygonMode(GL_FRONT_AND_BACK, (wire ? GL_LINE : GL_FILL));
		}
		if (key_pressed == 'a') {
			matrice_translatie_1 *= myTranslate(-2.0f, 0, 0);
			light_position.x -= 10;
		}
		if (key_pressed == 'd') {
			matrice_translatie_1 *= myTranslate(2.0f, 0, 0);
			light_position.x += 10;
		}
		if (key_pressed == 'w') {
			matrice_translatie_1 *= myTranslate(0, 0, -2.0f);
			light_position.z -= 10;
		}
		if (key_pressed == 's') {
			matrice_translatie_1 *= myTranslate(0, 0, 2.0f);
			light_position.z += 10;
		}

		float moveSpeed = 5;

		// Camera Translation
		if (key_pressed == 'u') { camera.translateForward(moveSpeed); }
		if (key_pressed == 'h') { camera.translateRight(-moveSpeed); }
		if (key_pressed == 'j') { camera.translateForward(-moveSpeed); }
		if (key_pressed == 'k') { camera.translateRight(moveSpeed); }
		if (key_pressed == 'i') { camera.translateUpword(moveSpeed); }
		if (key_pressed == 'y') { camera.translateUpword(-moveSpeed); }
	}

	//tasta ridicata
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){
	}
	//tasta speciala (up/down/F1/F2..) apasata
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){

		float moveSpeed = 5;
		float rotateSpeedOX = moveSpeed / 200;
		float rotateSpeedOY = moveSpeed / 200;
		float rotateSpeedOZ = moveSpeed / 200;

		// Camera Rotate FPS
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();

		if (key_pressed == GLUT_KEY_RIGHT) { camera.rotateFPS_OY(rotateSpeedOY); }
		if (key_pressed == GLUT_KEY_LEFT) { camera.rotateFPS_OY(-rotateSpeedOY); }

		if (key_pressed == GLUT_KEY_UP) { camera.rotateFPS_OX(rotateSpeedOX); }
		if (key_pressed == GLUT_KEY_DOWN) { camera.rotateFPS_OX(-rotateSpeedOX); }

	}
	//tasta speciala ridicata
	void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){}
	//drag cu mouse-ul
	void notifyMouseDrag(int mouse_x, int mouse_y){ }
	//am miscat mouseul (fara sa apas vreun buton)
	void notifyMouseMove(int mouse_x, int mouse_y){ }
	//am apasat pe un boton
	void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ }
	//scroll cu mouse-ul
	void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ std::cout<<"Mouse scroll"<<std::endl;}

};

int main(){
	//initializeaza GLUT (fereastra + input + context OpenGL)
	lab::glut::WindowInfo window(std::string("EGC - lab 6 - iluminare Gouraud"),800,600,100,100,true);
	lab::glut::ContextInfo context(3,3,false);
	lab::glut::FramebufferInfo framebuffer(true,true,true,true);
	lab::glut::init(window,context, framebuffer);

	//initializeaza GLEW (ne incarca functiile openGL, altfel ar trebui sa facem asta manual!)
	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	//creem clasa noastra si o punem sa asculte evenimentele de la GLUT
	//DUPA GLEW!!! ca sa avem functiile de OpenGL incarcate inainte sa ii fie apelat constructorul (care creeaza obiecte OpenGL)
	Laborator mylab;
	lab::glut::setListener(&mylab);

	//taste
	std::cout<<"Taste:"<<std::endl<<"\tESC ... iesire"<<std::endl<<"\tSPACE ... reincarca shadere"<<std::endl<<"\tw ... toggle wireframe"<<std::endl;

	//run
	lab::glut::run();

	return 0;
}