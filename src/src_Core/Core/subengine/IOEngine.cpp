#include "IOEngine.h"

#include <Core/Assert.h>

bool IOEngine::m_keys[1024];
bool IOEngine::m_keysPrevious[1024];
int IOEngine::m_windowWidth = 0;
int IOEngine::m_windowHeight = 0;


void IOEngine::Init(size_t windowWidth, size_t windowHeight, const char* name) {
	CreateGameWindow(windowWidth, windowHeight, name);
}

void IOEngine::ProcessInput() {
	memcpy(m_keysPrevious, m_keys, sizeof(m_keys));
	glfwPollEvents();
}

int IOEngine::CreateGameWindow(size_t width, size_t height, const char* name) {
	// Init and setup GLFW
	RELEASE_ASSERT_FATAL(glfwInit(), "bad glfw");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GFLW window
	m_window = glfwCreateWindow(width, height, name, NULL, NULL);
	if (m_window == NULL) {
		glfwTerminate();
		ASSERT_BREAK_FATAL("Error: Window creation failed");
	}

	glfwMakeContextCurrent(m_window);

	// Init GLAD
	RELEASE_ASSERT_FATAL(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Error: GLAD initialization failed");
	
	m_windowWidth = width;
	m_windowHeight = height;
	glViewport(0, 0, width, height);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
	glfwSetKeyCallback(m_window, key_callback);

	return 0;
}

void IOEngine::BindInput(const std::string& inputName, int button) {
	m_inputMap[inputName] = button;
}
void IOEngine::UnbindInput(const std::string& inputName) {
	m_inputMap.erase(inputName);
}

bool IOEngine::IsDown(const int keyCode) {
	// TODO: some error handling.
	if (m_keys[keyCode])
		return true;
	else
		return false;
}

bool IOEngine::IsTapped(const int keyCode) {
	// TODO: some error handling.
	if (m_keys[keyCode] && !m_keysPrevious[keyCode])
		return true;
	else
		return false;
}

bool IOEngine::IsDown(const std::string& inputName) {
	return IsDown(m_inputMap[inputName]);
}

bool IOEngine::IsTapped(const std::string& inputName) {
	return IsTapped(m_inputMap[inputName]);
}

void IOEngine::Stop() {
	glfwTerminate();
}

bool IOEngine::ShouldClose() {
	return glfwWindowShouldClose(m_window);
}

float IOEngine::GetCurrentWindowTime() {
	return static_cast<float>(glfwGetTime());
}

int IOEngine::GetWindowWidth() {
	return m_windowWidth;
}
int IOEngine::GetWindowHeight() {
	return m_windowHeight;
}

void IOEngine::SwapBuffers() {
	glfwSwapBuffers(m_window);
}

void IOEngine::ToggleFullscreen() {
	static int prevWidth = m_windowWidth;
	static int prevHeight = m_windowHeight;
	static int prevPosX = m_windowWidth;
	static int prevPosY = m_windowHeight;
	

	if (m_isFullscreen) {
		glfwSetWindowMonitor(m_window, nullptr, prevPosX, prevPosY, prevWidth, prevHeight, 0);
		m_isFullscreen = false;
	}
	else {
		glfwGetWindowSize(m_window, &prevWidth, &prevHeight);
		glfwGetWindowPos(m_window, &prevPosX, &prevPosY);
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
		m_isFullscreen = true;
	}
	glfwSwapInterval(1);
}


void IOEngine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	m_windowWidth = width;
	m_windowHeight = height;
	glViewport(0, 0, width, height);
}

void IOEngine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			m_keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
			m_keys[key] = GL_FALSE;
	}
}
