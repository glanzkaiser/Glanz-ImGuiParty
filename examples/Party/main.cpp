//-----------------------------------------------------------------------------
// ImDrawList coding Party!
// v0.23
// Paste this anywhere in an imgui app/example and call FxTestBed()
//-----------------------------------------------------------------------------

// Rules:
// - Your source file (fx.inl) must be <= 1024 bytes.
// - Effect should be reasonably portable (not relying on specific render backend callback)
// - OK to use preprocessor define or other tricks as long as they are reasonably portable.
// - Included are: math.h, imgui.h, imgui_internal.h with ImVec2 maths operators
// - The effect should not use ImGui:: functions, only use ImDrawList facilities. No ImGui:: function!
// - Canvas ratio is expected to be 16/9, canvas size expected to be 320 by 180.
// - For simplicity we assume you can encode a color as 0xAAGGBBRR instead of using the IM_COL32() macro,
//   therefore IMGUI_USE_BGRA_PACKED_COLOR config option is not supported!

// Changelog:
// 0.23 - fix mouse y position again (broken in 0.22)
// 0.22 - use InvisibleButton instead of Dummy to capture inputs
// 0.21 - fix mouse y position
// 0.20 - added mouse buttons, added math operators

#define IMGUI_DEFINE_MATH_OPERATORS // Access to math operators
#include <math.h>
#include "imgui.h"
#include "imgui_internal.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

// Function signature:
//  void FX(ImDrawList* d, ImVec2 a, ImVec2 b, ImVec2 sz, ImVec4 mouse, float t);
//     d : draw list
//     a : upper-left corner
//     b : lower-right corner
//    sz : size (== b - a)
// mouse : x,y = mouse position (normalized so 0,0 over 'a'; 1,1 is over 'b', not clamped)
//         z,w = left/right button held. <-1.0f not pressed, 0.0f just pressed, >0.0f time held.
//    t  : time
// If not using a given parameter, you can omit its name in your function to save a few characters.

// Insert your code in fx.inl
// This is the file which size we are measuring, and should be kept <1024 bytes
#include "fx.inl" // <--- your effect

// Shared testbed

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 330
	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 Party Time", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
#ifdef __EMSCRIPTEN__
	io.IniFilename = nullptr;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (!glfwWindowShouldClose(window))
#endif
	{
	glfwPollEvents();

        // Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		ImGuiIO& io = ImGui::GetIO();
    		ImGui::Begin("FX", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    		ImVec2 size(320.0f, 180.0f);
    		ImGui::InvisibleButton("canvas", size);
    		ImVec2 p0 = ImGui::GetItemRectMin();
    		ImVec2 p1 = ImGui::GetItemRectMax();
    		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRect(p0, p1);

		ImVec4 mouse_data;
		mouse_data.x = (io.MousePos.x - p0.x) / size.x;
    		mouse_data.y = (io.MousePos.y - p0.y) / size.y;
    		mouse_data.z = io.MouseDownDuration[0];
    		mouse_data.w = io.MouseDownDuration[1];

    		FX(draw_list, p0, p1, size, mouse_data, (float)ImGui::GetTime());
    		draw_list->PopClipRect();
    		ImGui::End();
	}

	{
		ImGuiIO& io = ImGui::GetIO();
    		ImGui::Begin("FX2", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    		ImVec2 size(320.0f, 180.0f);
    		ImGui::InvisibleButton("canvas", size);
    		ImVec2 p0 = ImGui::GetItemRectMin();
    		ImVec2 p1 = ImGui::GetItemRectMax();
    		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRect(p0, p1);

		ImVec4 mouse_data;
		mouse_data.x = (io.MousePos.x - p0.x) / size.x;
    		mouse_data.y = (io.MousePos.y - p0.y) / size.y;
    		mouse_data.z = io.MouseDownDuration[0];
    		mouse_data.w = io.MouseDownDuration[1];

    		FX2(draw_list, p0, p1, size, mouse_data, (float)ImGui::GetTime());
    		draw_list->PopClipRect();
    		ImGui::End();
	}

	{
		ImGuiIO& io = ImGui::GetIO();
    		ImGui::Begin("FX3", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    		ImVec2 size(320.0f, 180.0f);
    		ImGui::InvisibleButton("canvas", size);
    		ImVec2 p0 = ImGui::GetItemRectMin();
    		ImVec2 p1 = ImGui::GetItemRectMax();
    		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRect(p0, p1);

		ImVec4 mouse_data;
		mouse_data.x = (io.MousePos.x - p0.x) / size.x;
    		mouse_data.y = (io.MousePos.y - p0.y) / size.y;
    		mouse_data.z = io.MouseDownDuration[0];
    		mouse_data.w = io.MouseDownDuration[1];

    		FX3(draw_list, p0, p1, size, mouse_data, (float)ImGui::GetTime());
    		draw_list->PopClipRect();
    		ImGui::End();
	} 
	
	{
		ImGuiIO& io = ImGui::GetIO();
    		ImGui::Begin("FX4", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    		ImVec2 size(320.0f, 180.0f);
    		ImGui::InvisibleButton("canvas", size);
    		ImVec2 p0 = ImGui::GetItemRectMin();
    		ImVec2 p1 = ImGui::GetItemRectMax();
    		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRect(p0, p1);

		ImVec4 mouse_data;
		mouse_data.x = (io.MousePos.x - p0.x) / size.x;
    		mouse_data.y = (io.MousePos.y - p0.y) / size.y;
    		mouse_data.z = io.MouseDownDuration[0];
    		mouse_data.w = io.MouseDownDuration[1];

    		FX4(draw_list, p0, p1, size, mouse_data, (float)ImGui::GetTime());
    		draw_list->PopClipRect();
    		ImGui::End();
	} 

	{
		ImGuiIO& io = ImGui::GetIO();
    		ImGui::Begin("FX5", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    		ImVec2 size(320.0f, 180.0f);
    		ImGui::InvisibleButton("canvas", size);
    		ImVec2 p0 = ImGui::GetItemRectMin();
    		ImVec2 p1 = ImGui::GetItemRectMax();
    		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->PushClipRect(p0, p1);

		ImVec4 mouse_data;
		mouse_data.x = (io.MousePos.x - p0.x) / size.x;
    		mouse_data.y = (io.MousePos.y - p0.y) / size.y;
    		mouse_data.z = io.MouseDownDuration[0];
    		mouse_data.w = io.MouseDownDuration[1];

    		FX5(draw_list, p0, p1, size, mouse_data, (float)ImGui::GetTime());
    		draw_list->PopClipRect();
    		ImGui::End();
	} 

	// Rendering 
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	}
#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#endif

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
//-----------------------------------------------------------------------------
