#include "../header/window.h"

/*--WINDOW CLASSE--*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

Window::Window(const unsigned int windowW, const unsigned int windowH, const char* windowTitle)
{
    // glfw: initialize and configure
      // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(windowW, windowH, "LearnOpenGL", NULL, NULL);
    windowPtr = window;

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        didWindowFailed = -1;
    }
    glfwMakeContextCurrent(windowPtr);
    glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        didWindowFailed = -1;
    }
}


/*INPUT LISTENER FUNCTIONS*/

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveCamera('z', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.moveCamera('q', Time::deltaTime);


	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveCamera('s', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.moveCamera('d', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.moveCamera('u', Time::deltaTime);

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.moveCamera('w', Time::deltaTime);

}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	double xOffset{ xPos - Mouse::lastX };
	double yOffset{ Mouse::lastY - yPos };// reversed: y ranges bottom to top


	if (Mouse::firstMouseInput)
	{
		Mouse::lastX = xPos;
		Mouse::lastY = yPos;
		Mouse::firstMouseInput = false;
	}

	Mouse::lastX = xPos;
	Mouse::lastY = yPos;

	xOffset *= Mouse::sensitivity;
	yOffset *= Mouse::sensitivity;

	camera.yawAngle += xOffset;
	camera.pitchAngle += yOffset;

	if (camera.pitchAngle > 89.0f)
		camera.pitchAngle = 89.0f;

	if (camera.pitchAngle < -89.0f)
		camera.pitchAngle = -89.0f;

	glm::vec3 direction{};

	direction.x = cos(glm::radians(camera.yawAngle)) * cos(glm::radians(camera.pitchAngle));
	direction.y = sin(glm::radians(camera.pitchAngle));
	direction.z = sin(glm::radians(camera.yawAngle)) * cos(glm::radians(camera.pitchAngle));
	camera.front = glm::normalize(direction);


}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Mouse::fov -= (float)yoffset;

	if (Mouse::fov < 1.0f)
		Mouse::fov = 1.0f;

	if (Mouse::fov > 45.0f)
		Mouse::fov = 45.0f;
}

/*WINDOW SETTINGS*/
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
