///
/// Created by ice1000 on 18-4-30.
///

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include "impl/GL/glcorearb.h" // avoid conflicting with system include
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <org_ice1000_jimgui_JImGui.h>

#include "basics.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

static void glfw_error_callback(int error, Ptr<const char> description) {
	fprintf(stderr, "ImGui Error %d: %s\n", error, description);
}

JNIEXPORT auto JNICALL
Java_org_ice1000_jimgui_JImGui_allocateNativeObjects(
		JNIEnv *env, jclass, jint width, jint height, jbyteArray _title) -> jlong {
	__JNI__FUNCTION__INIT__
	__get(Byte, title);
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) return 0L;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	Ptr<GLFWwindow> window = glfwCreateWindow(width, height, STR_J2C(title), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	// Enable vsync
	glfwSwapInterval(1);
	gl3wInit();

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void) io;
	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplGlfwGL3_Init(window, true);
	__release(Byte, title);
	__JNI__FUNCTION__CLEAN__
	return reinterpret_cast<jlong> (window);
}

JNIEXPORT void JNICALL
Java_org_ice1000_jimgui_JImGui_deallocateNativeObjects(JNIEnv *, jclass, jlong nativeObjectPtr) {
	auto *window = PTR_J2C(GLFWwindow, nativeObjectPtr);
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

JNIEXPORT auto JNICALL
Java_org_ice1000_jimgui_JImGui_windowShouldClose(JNIEnv *, jclass, jlong nativeObjectPtr) -> jboolean {
	return static_cast<jboolean>(glfwWindowShouldClose(PTR_J2C(GLFWwindow, nativeObjectPtr)) ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT void JNICALL
Java_org_ice1000_jimgui_JImGui_initNewFrame(JNIEnv *, jclass, jlong) {
	glfwPollEvents();
	ImGui_ImplGlfwGL3_NewFrame();
}

JNIEXPORT void JNICALL
Java_org_ice1000_jimgui_JImGui_render(JNIEnv *, jclass, jlong nativeObjectPtr, jlong colorPtr) {
	auto *window = PTR_J2C(GLFWwindow, nativeObjectPtr);
	auto *clear_color = PTR_J2C(ImVec4, colorPtr);
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

#pragma clang diagnostic pop
