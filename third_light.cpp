#ifndef THIRD_LIGHT_CPP
#define THIRD_LIGHT_CPP

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include"shader.h"
#include"camera.h"

#include <iostream>
#include <string>

using namespace glm;

const float PI = (const float)atan(1.0) * 4;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);
void Render(Shader lightingShader);
void calculate_angel();


// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

bool blinn = false;
bool blinnKeyPressed = false;
bool blinnKeyPressed1 = false;
bool blinnKeyPressed2 = false;
bool blinnKeyHome = false;
bool Open_test = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(6.0f, 8.0f, 0.0f);


/*
	运动时候的变量 用来操作腿和手臂
*/
float catchx = 0.5f, catchy = 1.0f, catchz = 0.0f;
float catchxr = -0.5f, catchyr = 1.0f, catchzr = 0.0f;
float legx = 0.25f, legy = -2.0f, legz = 0.0f;
float legxr = -0.25f, legyr = -1.75f, legzr = 0.2f;
float UP_z = 0.5f, UP_zr = 0.5f, DOWN_z = 0.5f, DOWN_zr = 0.5f;
float walk_tran = 0.00f, angle_walk = 0.00f, r = 0.5f;
bool change_hand = false;

float angle_leg_up = 0.0f, angle_leg_down = 0.0f;/*左腿的抬腿角度*/
float angle_left_up=  PI/2, angle_left_down=0.0f;/*左手的动作抓取角度*/
float angle_legr_up = 0.0f, angle_legr_down = 0.0f;/*右腿的提腿角度*/
float angle_right_up = PI / 2, angle_right_down = 0.0f;/*右手的动作抓取角度*/


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader simpleShader("simpleShader.vs", "simpleShader.fs");
	Shader lightingShader("enhanced_lighting.vs", "enhanced_lighting.fs");
	Shader lampShader("lamp.vs", "lamp.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	
	static const GLfloat vertices[] = {
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f,  0.0f,
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f,  0.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f,  0.0f,

		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f,

		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f,  0.0f,
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  0.0f,
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f,  0.0f,

		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,  0.0f,
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,  0.0f,

		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f,  0.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f,  0.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f,  0.0f,
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f,  0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*
		创造深度贴图并绑定深度缓冲
	*/
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	/*
		加载纹理图片
	*/
	std::string path_image = "container2.png";
	unsigned int diffuseMap = loadTexture(path_image.c_str());

	/*-------------------------------------------------------*/
	/*-------------------------------------------------------*/
	//循环渲染//
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/*
			判断简单输入
		*/
		processInput(window);

		/*
			计算每一个部件的角度变化
		*/
		calculate_angel();


		/*
			----------------------------------------
			第一次阴影贴图渲染
			----------------------------------------
		*/
		mat4 lightProjection, lightView, lightSpaceMatrix;
		GLfloat near_plane = 0.0f, far_plane = 40.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = lookAt(lightPos, vec3(0, 0, 10), vec3(0, 1, 0));
		lightSpaceMatrix = lightProjection * lightView;

		simpleShader.use();
		simpleShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(lightVAO);
		Render(simpleShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		/*
			----------------------------------------------
			 第二次真正渲染
			----------------------------------------------
		*/

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();

		glUniform1i(glGetUniformLocation(lightingShader.ID, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.ID, "shadowMap"), 1);

		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setInt("blinn", blinn);
		// light properties
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		// material properties
		lightingShader.setVec3("material.specular", 0.8f, 0.8f, 0.8f); // specular lighting doesn't have full effect on this object's material
		lightingShader.setFloat("material.shininess", 32.0f);
		// view/projection transformations
		mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glBindVertexArray(cubeVAO);
		Render(lightingShader);
		



		// 画出光源来
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// tell GLFW to capture our mouse
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS&&!blinnKeyPressed1)
	{
		blinnKeyPressed1 = true;
		if (!blinnKeyHome)
		{
			blinnKeyHome = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			blinnKeyHome = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_RELEASE)
	{
		blinnKeyPressed1 = false;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);


	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		legy += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		legy -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		DOWN_z += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		DOWN_z -= 0.01f;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		catchy += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		catchy -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		UP_z += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		UP_z -= 0.01f;

	if (!Open_test)
	{
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{//按下UP键往前走
			walk_tran += 0.01; angle_walk += 0.03f;
			if (angle_walk > PI)
			{
				change_hand = !change_hand;
			}
			angle_walk = angle_walk > PI ? angle_walk - PI : angle_walk;
		}
	}


	if (glfwGetKey(window, GLFW_KEY_NUM_LOCK) == GLFW_PRESS && !blinnKeyPressed2)
	{
		blinnKeyPressed2 = true;
		Open_test = !Open_test;
	}
	if (glfwGetKey(window, GLFW_KEY_NUM_LOCK) == GLFW_RELEASE)
	{
		blinnKeyPressed2 = false;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void calculate_angel()
{
	//------------------------------------------------
	/*
		控制动作的协调性 与test逆向动力学的动作操作
	*/
	if (Open_test)
	{//可以进行动作测试 反之关闭动作测试
		catchz = walk_tran + UP_z;
		legz = walk_tran + DOWN_z;
	}
	else
	{
		if (!change_hand)
		{//两腿或者两腿交换进行操作
			catchzr = walk_tran + 0.5f - r * cos(angle_walk);
			catchyr = 1.0f;

			legz = walk_tran - 0.30 - r * cos(angle_walk);
			legy = r * sin(angle_walk) - 1.75;
		}
		else
		{
			catchz = walk_tran + 1.0f - r * cos(angle_walk);
			catchy = 1.0f;

			legzr = walk_tran - 0.30 - r * cos(angle_walk);
			legyr = r * sin(angle_walk) - 1.75;
		}
	}



	/*左手的动作抓取角度*/
	//float angle_left_up=  PI/2, angle_left_down=0.0f;
	float Length = (catchx - 0.5f)*(catchx - 0.5f) + (catchy - 2.0f)*(catchy - 2.0f) + (catchz - walk_tran)*(catchz - walk_tran);
	if (Length >= 4)
	{
		angle_left_down = angle_left_up = acosf((2 - catchy) / sqrtf(Length));
	}
	else
	{
		float t_t, a_t;
		if (catchy > 2)
		{
			t_t = acosf((catchy - 2) / sqrtf(Length));
			a_t = acosf(Length / (2 * sqrtf(Length)));
			angle_left_up = PI - a_t - t_t;
			angle_left_down = angle_left_up + 2 * a_t;
		}
		else
		{
			t_t = acosf((catchz - walk_tran) / sqrtf(Length));
			a_t = acosf(Length / (2 * sqrtf(Length)));
			angle_left_up = PI / 2 - a_t - t_t;
			angle_left_down = angle_left_up + 2 * a_t;
		}
	}

	/*右手的动作抓取*/
	//float angle_right_up = PI / 2, angle_right_down = 0.0f;
	float Length_right = (catchxr + 0.5f)*(catchxr + 0.5f) + (catchyr - 2.0f)*(catchyr - 2.0f) + (catchzr - walk_tran)*(catchzr - walk_tran);
	if (Length_right >= 4)
	{
		angle_right_down = angle_right_up = acosf((2 - catchyr) / sqrtf(Length_right));
	}
	else
	{
		float t_t_r, a_t_r;
		if (catchyr > 2)
		{
			t_t_r = acosf((catchyr - 2) / sqrtf(Length_right));
			a_t_r = acosf(Length_right / (2 * sqrtf(Length_right)));
			angle_right_up = PI - a_t_r - t_t_r;
			angle_right_down = angle_right_up + 2 * a_t_r;
		}
		else
		{
			t_t_r = acosf((catchzr - walk_tran) / sqrtf(Length_right));
			a_t_r = acosf(Length_right / (2 * sqrtf(Length_right)));
			angle_right_up = PI / 2 - a_t_r - t_t_r;
			angle_right_down = angle_right_up + 2 * a_t_r;
		}
	}

	/*左腿的抬腿动作*/
	//float angle_leg_up = 0.0f, angle_leg_down = 0.0f;
	float Len_leg = (legx - 0.25f)*(legx - 0.25f) + (0 - legy) * (0 - legy) + (legz - walk_tran)*(legz - walk_tran);
	if (Len_leg > 4)
	{
		angle_leg_down = angle_leg_up = acosf((0 - legy) / sqrtf(Len_leg));
	}
	else
	{
		float leg_M, a_leg_T;
		if (legy < 0)
		{
			if (legz >= walk_tran)
			{
				a_leg_T = acosf((-legy) / sqrtf(Len_leg));
				leg_M = acosf(Len_leg / (2 * sqrtf(Len_leg)));
				angle_leg_up = leg_M + a_leg_T;
				angle_leg_down = -leg_M + a_leg_T;
			}
			else
			{
				a_leg_T = acosf((walk_tran - legz) / sqrtf(Len_leg));
				leg_M = acosf(Len_leg / (2 * sqrtf(Len_leg))) + a_leg_T;
				angle_leg_up = leg_M - PI / 2;
				angle_leg_down = -(leg_M - 2 * a_leg_T + PI / 2);
			}
		}
	}


	/*右腿的提腿动作*/
	//float angle_legr_up = 0.0f, angle_legr_down = 0.0f;
	float Len_legr = (legxr + 0.25f)*(legxr + 0.25f) + (0 - legyr) * (0 - legyr) + (legzr - walk_tran)*(legzr - walk_tran);
	if (Len_legr > 4)
	{
		angle_legr_down = angle_legr_up = acosf((0 - legyr) / sqrtf(Len_legr));
	}
	else
	{
		float leg_Mr, a_leg_Tr;
		if (legyr < 0)
		{
			if (legzr >= walk_tran)
			{
				a_leg_Tr = acosf((-legyr) / sqrtf(Len_legr));
				leg_Mr = acosf(Len_legr / (2 * sqrtf(Len_legr)));
				angle_legr_up = leg_Mr + a_leg_Tr;
				angle_legr_down = -leg_Mr + a_leg_Tr;
			}
			else
			{
				a_leg_Tr = acosf((walk_tran - legzr) / sqrtf(Len_legr));
				leg_Mr = acosf(Len_legr / (2 * sqrtf(Len_legr))) + a_leg_Tr;
				angle_legr_up = leg_Mr - PI / 2;
				angle_legr_down = -(leg_Mr - 2 * a_leg_Tr + PI / 2);
			}
		}
	}
}

void Render(Shader lightingShader)
{
	//画小球左手——————————————————————————
	mat4 model_simple = mat4(1.0f);
	model_simple = translate(model_simple, vec3(catchx + 0.1f, catchy, catchz));
	model_simple = scale(model_simple, vec3(0.1f, 0.1f, 0.1f));
	lightingShader.setMat4("model", model_simple);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//画小球右手——————————————————————————
	mat4 model_simpler = mat4(1.0f);
	model_simpler = translate(model_simpler, vec3(catchxr - 0.1f, catchyr, catchzr));
	model_simpler = scale(model_simpler, vec3(0.1f, 0.1f, 0.1f));
	lightingShader.setMat4("model", model_simpler);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//画小球左腿——————————————————————————
	mat4 model_leg = mat4(1.0f);
	model_leg = translate(model_leg, vec3(legx + 0.05f, legy, legz + 0.2f));
	model_leg = scale(model_leg, vec3(0.1f, 0.1f, 0.3f));
	lightingShader.setMat4("model", model_leg);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//画小球右腿——————————————————————————
	mat4 model_legr = mat4(1.0f);
	model_legr = translate(model_legr, vec3(legxr - 0.05f, legyr, legzr + 0.2f));
	model_legr = scale(model_legr, vec3(0.1f, 0.1f, 0.3f));
	lightingShader.setMat4("model", model_legr);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//画地板——————————————————————————
	mat4 model_stay = mat4(1.0f);
	model_stay = translate(model_stay, vec3(0.0f, -2.0f, 0.0f));
	model_stay = scale(model_stay, vec3(10.0f, 0.1f, 20.0f));
	lightingShader.setMat4("model", model_stay);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//画身体——————————————————————————
	mat4 model_body(1.0f), model_body2(1.0f);
	model_body = translate(model_body, vec3(0.0f, 1.0f, walk_tran));
	model_body2 = scale(model_body, vec3(0.5f, 1.0f, 0.5f));
	lightingShader.setMat4("model", model_body2);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//画脑袋————————————————
	vec4 neck(0.0f, 1.0f, 0.0f, 1.0f);
	neck = model_body * neck;
	mat4 model_head = mat4(1.0f);
	model_head = translate(model_head, vec3(neck.x, neck.y + 0.3, neck.z));
	model_head = rotate(model_head, (float)sin(glfwGetTime() * 2) / 5, vec3(0.0f, 0.0f, 1.0f));
	model_head = scale(model_head, vec3(0.1f, 0.1f, 0.1f));
	lightingShader.setMat4("model", model_head);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//画左手————————————————————————
	vec4 shd_left(0.5f, 1.0f, 0.0f, 1.0f);//初始左肩膀的位置 要与身体做相同的变换
	//获得身体的左肩膀位置
	shd_left = model_body * shd_left;
	mat4 model_hand_left = mat4(1.0f);
	//手臂移动到左肩膀处
	model_hand_left = translate(model_hand_left, vec3(shd_left.x + 0.1, shd_left.y, shd_left.z));
	//手臂按照角度摆动
	model_hand_left = rotate(model_hand_left, angle_left_up, vec3(-1.0f, 0.0f, 0.0f));
	//平移手臂位置，使旋转轴位于手臂的顶端
	model_hand_left = translate(model_hand_left, vec3(0.0f, -0.5f, 0.0f));
	//将正方体缩放成长条形的手臂形状
	model_hand_left = scale(model_hand_left, vec3(0.1f, 0.5f, 0.1f));

	lightingShader.setMat4("model", model_hand_left);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/*左手柱*/
	vec4 arm_left(0.5f, 0.0f, 0.0f, 1.0f);
	arm_left = model_body * arm_left;
	mat4 model_arm_left = mat4(1.0f);
	model_arm_left = translate(model_arm_left, vec3(arm_left.x + 0.1, arm_left.y + 1 - cos(angle_left_up), arm_left.z + sin(angle_left_up)));
	model_arm_left = rotate(model_arm_left, angle_left_down, vec3(-1.0f, 0.0f, 0.0f));
	model_arm_left = translate(model_arm_left, vec3(0.0f, -0.5f, 0.0f));
	model_arm_left = scale(model_arm_left, vec3(0.1f, 0.5f, 0.1f));
	lightingShader.setMat4("model", model_arm_left);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//画右手————————————
	vec4 shd_right(-0.5f, 1.0f, 0.0f, 1.0f);
	shd_right = model_body * shd_right;
	mat4 model_hand_right = mat4(1.0f);
	model_hand_right = translate(model_hand_right, vec3(shd_right.x - 0.1, shd_right.y, shd_right.z));
	model_hand_right = rotate(model_hand_right, angle_right_up, vec3(-1.0f, 0.0f, 0.0f));
	model_hand_right = translate(model_hand_right, vec3(0.0f, -0.5f, 0.0f));
	model_hand_right = scale(model_hand_right, vec3(0.1f, 0.5f, 0.1f));
	lightingShader.setMat4("model", model_hand_right);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	/*右手柱*/
	vec4 arm_right(-0.5f, 0.0f, 0.0f, 1.0f);
	arm_right = model_body * arm_right;
	mat4 model_arm_right = mat4(1.0f);
	model_arm_right = translate(model_arm_right, vec3(arm_right.x - 0.1, arm_right.y + 1 - cos(angle_right_up), arm_right.z + sin(angle_right_up)));
	model_arm_right = rotate(model_arm_right, angle_right_down, vec3(-1.0f, 0.0f, 0.0f));
	model_arm_right = translate(model_arm_right, vec3(0.0f, -0.5f, 0.0f));
	model_arm_right = scale(model_arm_right, vec3(0.1f, 0.5f, 0.1f));
	lightingShader.setMat4("model", model_arm_right);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//左脚
	vec4 pp_left(0.25f, -1.0f, 0.0f, 1.0f);
	pp_left = model_body * pp_left;
	mat4 model_leg_left = mat4(1.0f);
	model_leg_left = translate(model_leg_left, vec3(pp_left.x + 0.05, pp_left.y, pp_left.z));
	model_leg_left = rotate(model_leg_left, angle_leg_up, vec3(-1.0f, 0.0f, 0.0f));
	model_leg_left = translate(model_leg_left, vec3(0.0f, -0.5f, 0.0f));
	model_leg_left = scale(model_leg_left, vec3(0.1f, 0.5f, 0.1f));
	lightingShader.setMat4("model", model_leg_left);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//左脚柱
	//Sleep(20);
	vec4 foot_left(0.25f, -2.0f, 0.0f, 1.0f);
	foot_left = model_body * foot_left;
	mat4 model_foot_left(1.0f);
	model_foot_left = translate(model_foot_left, vec3(foot_left.x + 0.05, foot_left.y + 1 - cos(angle_leg_up), foot_left.z + sin(angle_leg_up)));
	model_foot_left = rotate(model_foot_left, angle_leg_down, vec3(-1.0f, 0.0f, 0.0f));
	model_foot_left = translate(model_foot_left, vec3(0.0f, -0.5f, 0.0f));
	model_foot_left = scale(model_foot_left, vec3(0.1f, 0.5f, 0.1f));
	lightingShader.setMat4("model", model_foot_left);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//右脚
	vec4 pp_right(-0.25f, -1.0f, 0.0f, 1.0f);
	pp_right = model_body * pp_right;
	mat4 model_leg_right = mat4(1.0f);
	model_leg_right = translate(model_leg_right, vec3(pp_right.x - 0.05, pp_right.y, pp_right.z));
	model_leg_right = rotate(model_leg_right, angle_legr_up, vec3(-1.0f, 0.0f, 0.0f));
	model_leg_right = translate(model_leg_right, vec3(0.0f, -0.5f, 0.0f));
	model_leg_right = scale(model_leg_right, vec3(0.1f, 0.5f, 0.1f));
	lightingShader.setMat4("model", model_leg_right);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//右脚柱
	vec4 foot_right(-0.25f, -2.0f, 0.0f, 1.0f);
	foot_right = model_body * foot_right;
	mat4 model_foot_right = mat4(1.0f);
	model_foot_right = translate(model_foot_right, vec3(foot_right.x - 0.05, foot_right.y + 1 - cos(angle_legr_up), foot_right.z + sin(angle_legr_up)));
	model_foot_right = rotate(model_foot_right, angle_legr_down, vec3(-1.0f, 0.0f, 0.0f));
	model_foot_right = translate(model_foot_right, vec3(0.0f, -0.5f, 0.0f));
	model_foot_right = scale(model_foot_right, vec3(0.1f, 0.5f, 0.1f));
	lightingShader.setMat4("model", model_foot_right);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


#endif // !