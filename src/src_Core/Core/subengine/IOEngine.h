#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <string>

// The IO Engine's handles input and display pixels from the Render Engine.
// We use GLFW.
#define IO_ENGINE_CLASS
class IOEngine {
public:
	void Init(size_t windowWidth, size_t windowHeight, const char* name);
	void Stop();
	bool ShouldClose();
	float GetCurrentWindowTime();
	void BindInput(const std::string& inputName, int button);
	void UnbindInput(const std::string& inputName);
	bool IsDown(const int keyCode);
	bool IsTapped(const int keyCode);
	bool IsDown(const std::string& inputName);
	bool IsTapped(const std::string& inputName);
	void SwapBuffers();
	void ProcessInput();
	int CreateGameWindow(size_t width, size_t height, const char* name);
	void ToggleFullscreen();

	int GetWindowWidth();
	int GetWindowHeight();


private:
	GLFWwindow* m_window;
	std::map<std::string, int> m_inputMap;

	bool m_isFullscreen{ false };

	static int m_windowWidth;
	static int m_windowHeight;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

	static bool m_keys[1024];			// A GLFW key table.
	static bool m_keysPrevious[1024];	// the previous frame's key data. Used to record single "taps" vs press-and-hold
};
