#pragma once

#include <GLFW/glfw3.h>

#include <stdio.h>

struct Window
{
    static void glfw_error_callback(int error, const char* description)
    {
        printf("GLFW error %i: %s", error, description);
    }
    static void glfw_window_size_callback(GLFWwindow* handle, int width, int height)
    {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
        window->width  = width;
        window->height = height;
    }
    static void glfw_mouse_btn_callback(GLFWwindow* handle, int button, int action, int mods) {}
    static void glfw_cursor_pos_callback(GLFWwindow* handle, double xpos, double ypos) {}
    static void glfw_scroll_callback(GLFWwindow* handle, double xoffset, double yoffset) {}
    static void glfw_key_callback(GLFWwindow* handle, int key, int scancode, int action, int mods) {}
    static void glfw_charCallback(GLFWwindow* handle, unsigned int c) {}
    static void glfw_dropCallback(GLFWwindow* handle, int numFiles, const char** names) {}
    Window()
    {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            puts("Failed to initialize window");
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        width  = 512;
        height = 512;
        handle = glfwCreateWindow(width, height, "R5R", nullptr, nullptr);
        if (!handle) {
            puts("Failed to create glfw window");
            glfwTerminate();
            return;
        }
        glfwSetWindowSizeCallback(handle, glfw_window_size_callback);
        glfwSetFramebufferSizeCallback(handle, glfw_window_size_callback);
        glfwSetMouseButtonCallback(handle, glfw_mouse_btn_callback);
        glfwSetCursorPosCallback(handle, glfw_cursor_pos_callback);
        glfwSetScrollCallback(handle, glfw_scroll_callback);
        glfwSetKeyCallback(handle, glfw_key_callback);
        glfwSetCharCallback(handle, glfw_charCallback);
        glfwSetDropCallback(handle, glfw_dropCallback);
        glfwMakeContextCurrent(handle);
        glfwSetWindowUserPointer(handle, this);
    }
    ~Window()
    {
        glfwDestroyWindow(handle);
        glfwTerminate();
    }
    void refresh()
    {
        glfwPollEvents();
        glfwSwapBuffers(handle);
    }

    GLFWwindow* handle;
    int         width;
    int         height;
};