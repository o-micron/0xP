/// --------------------------------------------------------------------------------------
/// Copyright 2025 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// --------------------------------------------------------------------------------------

#include <Renderer/WebGPU/XPWGPUWindow.h>

#include <Renderer/WebGPU/XPWGPURenderer.h>
#include <Utilities/XPLogger.h>

// --------------------------------------------------------
// FILE INTERNAL FUNCTIONS DECLARATIONS
// --------------------------------------------------------
static bool
XPInitWgpu(XPWGPURenderer* renderer);
static int
XPWindowMin(int A, int B);
static int
XPWindowMax(int A, int B);
static bool
XPWindowGlfwSetWindowCenter(GLFWwindow* window);
static void
XPWindowGlfwErrorCallback(int error, const char* description);
static void
XPWindowGlfwWindowSizeCallback(GLFWwindow* handle, int width, int height);
static void
XPWindowGlfwMouseBtnCallback(GLFWwindow* handle, int button, int action, int mods);
static void
XPWindowGlfwCursorPosCallback(GLFWwindow* handle, double x, double y);
static void
XPWindowGlfwScrollCallback(GLFWwindow* handle, double x, double y);
static void
XPWindowGlfwKeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods);
static void
XPWindowGlfwCharCallback(GLFWwindow* handle, unsigned char c);
static void
XPWindowGlfwDropCallback(GLFWwindow* handle, int numFiles, const char** names);
static void
XPWindowGlfwIconifyCallback(GLFWwindow* handle, int status);
static void
XPWindowGlfwMaximizeCallback(GLFWwindow* handle, int status);
static void
print_wgpu_error(WGPUErrorType error_type, const char* message, void*);
// --------------------------------------------------------

XPWGPUWindow::XPWGPUWindow(XPWGPURenderer* const renderer)
  : _renderer(renderer)
{
}

XPWGPUWindow::~XPWGPUWindow() {}

void
XPWGPUWindow::initialize()
{
    glfwSetErrorCallback(XPWindowGlfwErrorCallback);
    if (!glfwInit()) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to initialize glfw %s:%i", __FILE__, __LINE__);
        return;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    _window = glfwCreateWindow(
      (int)_size.x, (int)_size.y, "XPENGINE [" XP_PLATFORM_NAME "][" XP_CONFIG_NAME "]", nullptr, nullptr);
    if (!_window) {
        glfwTerminate();
        XP_LOGV(XPLoggerSeverityFatal, "Failed to create a window %s:%i", __FILE__, __LINE__);
        return;
    }

    if (!XPInitWgpu(_renderer)) {
        glfwDestroyWindow(_window);
        glfwTerminate();
        XP_LOGV(XPLoggerSeverityFatal, "Failed to initialize web gpu %s:%i", __FILE__, __LINE__);
        return;
    }
    glfwShowWindow(_window);

    XPWindowGlfwSetWindowCenter(_window);
    glfwSetFramebufferSizeCallback(_window, XPWindowGlfwWindowSizeCallback);
    glfwSetMouseButtonCallback(_window, XPWindowGlfwMouseBtnCallback);
    glfwSetCursorPosCallback(_window, XPWindowGlfwCursorPosCallback);
    glfwSetScrollCallback(_window, XPWindowGlfwScrollCallback);
    glfwSetKeyCallback(_window, XPWindowGlfwKeyCallback);
    glfwSetDropCallback(_window, XPWindowGlfwDropCallback);
    glfwSetWindowIconifyCallback(_window, XPWindowGlfwIconifyCallback);
    // glfwSetWindowMaximizeCallback(_window, XPWindowGlfwMaximizeCallback);
    glfwSetWindowUserPointer(_window, this);

    _deltaTimeInMilliseconds = 1.0f / 60.0f;
    _timeInMilliseconds      = glfwGetTime();

    return;
}

void
XPWGPUWindow::finalize()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void
XPWGPUWindow::pollEvents()
{
    _deltaTimeInMilliseconds = glfwGetTime() - _timeInMilliseconds;
    _timeInMilliseconds      = glfwGetTime();

    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)_window, &width, &height);

    // React to changes in screen size
    if (_size.x != width && _size.y != height) {
        _renderer->invalidateDeviceObjects();

        if (_renderer->getSwapchain()) { wgpuSwapChainRelease(_renderer->getSwapchain()); }

        _size.x = width;
        _size.y = height;

        WGPUSwapChainDescriptor swap_chain_desc = {};
        swap_chain_desc.usage                   = WGPUTextureUsage_RenderAttachment;
        swap_chain_desc.format                  = WGPUTextureFormat_BGRA8Unorm;
        swap_chain_desc.width                   = _size.x;
        swap_chain_desc.height                  = _size.y;
        swap_chain_desc.presentMode             = WGPUPresentMode_Fifo;
        _renderer->setSwapchain(
          wgpuDeviceCreateSwapChain(_renderer->getDevice(), _renderer->getSurface(), &swap_chain_desc));

        _renderer->createDeviceObjects();
    }
}

void
XPWGPUWindow::refresh()
{
}

void
XPWGPUWindow::onResizingEvents()
{
}

XPWGPURenderer*
XPWGPUWindow::getRenderer() const
{
    return _renderer;
}

GLFWwindow*
XPWGPUWindow::getWindow() const
{
    return _window;
}

uint32_t
XPWGPUWindow::getState() const
{
    return _state;
}

float
XPWGPUWindow::getRendererScale() const
{
    return _rendererScale;
}

XPVec2<int>
XPWGPUWindow::getWindowSize() const
{
    return _size;
}

XPVec2<int>
XPWGPUWindow::getRendererSize() const
{
    auto rendererSize = _size;
    rendererSize.x *= _rendererScale;
    rendererSize.y *= _rendererScale;
    return rendererSize;
}

XPVec2<float>
XPWGPUWindow::getMouseLocation() const
{
    return _mouseLocation;
}

float
XPWGPUWindow::getDeltaTime() const
{
    return _deltaTimeInMilliseconds;
}

float
XPWGPUWindow::getTime() const
{
    return _timeInMilliseconds;
}

void
XPWGPUWindow::setRendererScale(float rendererScale)
{
    _rendererScale = rendererScale;
}

void
XPWGPUWindow::setWindowSize(XPVec2<int> size)
{
    _size = size;

    glfwSetWindowSize(_window, _size.x, _size.y);
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);

    _size.x = width;
    _size.y = height;

    _renderer->invalidateDeviceObjects();

    if (_renderer->getSwapchain()) { wgpuSwapChainRelease(_renderer->getSwapchain()); }

    _size.x = width;
    _size.y = height;

    WGPUSwapChainDescriptor swap_chain_desc = {};
    swap_chain_desc.usage                   = WGPUTextureUsage_RenderAttachment;
    swap_chain_desc.format                  = WGPUTextureFormat_BGRA8Unorm;
    swap_chain_desc.width                   = _size.x;
    swap_chain_desc.height                  = _size.y;
    swap_chain_desc.presentMode             = WGPUPresentMode_Fifo;
    _renderer->setSwapchain(
      wgpuDeviceCreateSwapChain(_renderer->getDevice(), _renderer->getSurface(), &swap_chain_desc));

    _renderer->createDeviceObjects();
}

void
XPWGPUWindow::simulateCopy(const char* text)
{
    glfwSetClipboardString(_window, text);
}

std::string
XPWGPUWindow::simulatePaste()
{
    return std::string(glfwGetClipboardString(_window));
}

void
XPWGPUWindow::onMouseFn(int button, int action, int mods)
{
}

void
XPWGPUWindow::onMouseMoveFn(double x, double y)
{
}

void
XPWGPUWindow::onMouseScrollFn(double x, double y)
{
}

void
XPWGPUWindow::onKeyboardFn(int key, int scancode, int action, int mods)
{
}

void
XPWGPUWindow::onWindowResizeFn(int width, int height)
{
}

void
XPWGPUWindow::onWindowDragDropFn(int numFiles, const char** names)
{
}

void
XPWGPUWindow::onWindowIconifyFn()
{
}

void
XPWGPUWindow::onWindowIconifyRestoreFn()
{
}

void
XPWGPUWindow::onWindowMaximizeFn()
{
}

void
XPWGPUWindow::onWindowMaximizeRestoreFn()
{
}

// --------------------------------------------------------
// FILE INTERNAL FUNCTIONS DEFINITIONS
// --------------------------------------------------------
static bool
XPInitWgpu(XPWGPURenderer* renderer)
{
    renderer->setDevice(emscripten_webgpu_get_device());
    if (!renderer->getDevice()) {
        XP_LOGV(XPLoggerSeverityFatal, "Failed to get webgpu device %s:%i", __FILE__, __LINE__);
        return false;
    }

    wgpuDeviceSetUncapturedErrorCallback(renderer->getDevice(), print_wgpu_error, NULL);

    // Use C++ wrapper due to misbehavior in Emscripten.
    // Some offset computation for wgpuInstanceCreateSurface in JavaScript
    // seem to be inline with struct alignments in the C++ structure
    wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc = {};
    html_surface_desc.selector                                      = "#canvas";

    wgpu::SurfaceDescriptor surface_desc = {};
    surface_desc.nextInChain             = &html_surface_desc;

    // Use 'null' instance
    wgpu::Instance instance = {};
    renderer->setSurface(instance.CreateSurface(&surface_desc).Release());

    WGPUSwapChainDescriptor swap_chain_desc = {};
    swap_chain_desc.usage                   = WGPUTextureUsage_RenderAttachment;
    swap_chain_desc.format                  = WGPUTextureFormat_BGRA8Unorm;
    swap_chain_desc.width                   = renderer->getWindowSize().x;
    swap_chain_desc.height                  = renderer->getWindowSize().y;
    swap_chain_desc.presentMode             = WGPUPresentMode_Fifo;
    renderer->setSwapchain(wgpuDeviceCreateSwapChain(renderer->getDevice(), renderer->getSurface(), &swap_chain_desc));

    return true;
}

static int
XPWindowMin(int A, int B)
{
    return (A <= B ? A : B);
}

static int
XPWindowMax(int A, int B)
{
    return (A >= B ? A : B);
}

static bool
XPWindowGlfwSetWindowCenter(GLFWwindow* window)
{
    if (!window) { return false; }
    int sx = 0, sy = 0;
    int px = 0, py = 0;
    int mx = 0, my = 0;
    int monitor_count = 0;
    int best_area     = 0;
    int final_x = 0, final_y = 0;
    glfwGetWindowSize(window, &sx, &sy);
    glfwGetWindowPos(window, &px, &py);
    GLFWmonitor** m = glfwGetMonitors(&monitor_count);
    if (!m) { return false; }
    for (int j = 0; j < monitor_count; ++j) {
        glfwGetMonitorPos(m[j], &mx, &my);
        const GLFWvidmode* mode = glfwGetVideoMode(m[j]);
        if (!mode) { continue; }
        int minX = XPWindowMax(mx, px);
        int minY = XPWindowMax(my, py);
        int maxX = XPWindowMin(mx + mode->width, px + sx);
        int maxY = XPWindowMin(my + mode->height, py + sy);
        int area = XPWindowMax(maxX - minX, 0) * XPWindowMax(maxY - minY, 0);
        if (area > best_area) {
            final_x   = mx + (mode->width - sx) / 2;
            final_y   = my + (mode->height - sy) / 2;
            best_area = area;
        }
    }
    if (best_area) {
        glfwSetWindowPos(window, final_x, final_y);
    } else {
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        if (primary) {
            const GLFWvidmode* desktop = glfwGetVideoMode(primary);
            if (desktop) {
                glfwSetWindowPos(window, (desktop->width - sx) / 2, (desktop->height - sy) / 2);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

static void
XPWindowGlfwErrorCallback(int error, const char* description)
{
    XP_LOGV(XPLoggerSeverityFatal, "window error: %s\nfile: %s:%i", description, __FILE__, __LINE__);
}

static void
XPWindowGlfwWindowSizeCallback(GLFWwindow* handle, int width, int height)
{
    if (width < 1 || height < 1) { return; }
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    window->onWindowResizeFn(width, height);
}

static void
XPWindowGlfwMouseBtnCallback(GLFWwindow* handle, int button, int action, int mods)
{
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    window->onMouseFn(button, action, mods);
}

static void
XPWindowGlfwCursorPosCallback(GLFWwindow* handle, double xpos, double ypos)
{
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    window->onMouseMoveFn(xpos, ypos);
}

static void
XPWindowGlfwScrollCallback(GLFWwindow* handle, double xoffset, double yoffset)
{
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    window->onMouseScrollFn(xoffset, yoffset);
}

static void
XPWindowGlfwKeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods)
{
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    window->onKeyboardFn(key, scancode, action, mods);
}

static void
XPWindowGlfwCharCallback(GLFWwindow* handle, unsigned char c)
{
}

static void
XPWindowGlfwDropCallback(GLFWwindow* handle, int numFiles, const char** names)
{
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    window->onWindowDragDropFn(numFiles, names);
}

static void
XPWindowGlfwIconifyCallback(GLFWwindow* handle, int status)
{
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    if (status == GLFW_TRUE) {
        window->onWindowIconifyFn();
    } else if (status == GLFW_FALSE) {
        window->onWindowIconifyRestoreFn();
    }
}

static void
XPWindowGlfwMaximizeCallback(GLFWwindow* handle, int status)
{
    XPWGPUWindow* window = static_cast<XPWGPUWindow*>(glfwGetWindowUserPointer(handle));
    if (status == GLFW_TRUE) {
        window->onWindowMaximizeFn();
    } else if (status == GLFW_FALSE) {
        window->onWindowMaximizeRestoreFn();
    }
}

static void
print_wgpu_error(WGPUErrorType error_type, const char* message, void*)
{
    const char* error_type_lbl = "";
    switch (error_type) {
        case WGPUErrorType_Validation: error_type_lbl = "Validation"; break;
        case WGPUErrorType_OutOfMemory: error_type_lbl = "Out of memory"; break;
        case WGPUErrorType_Unknown: error_type_lbl = "Unknown"; break;
        case WGPUErrorType_DeviceLost: error_type_lbl = "Device lost"; break;
        default: error_type_lbl = "Unknown";
    }
    printf("%s error: %s\n", error_type_lbl, message);
}

// --------------------------------------------------------