#include "glfw/glfw_headers.h"
#include "glfw_key_input.h"

#include "logging/logging.h"

using namespace phenyl::glfw;

static phenyl::Logger LOGGER{"GLFW_KEY_INPUT", phenyl::graphics::detail::GRAPHICS_LOGGER};

GLFWKeyInput::GLFWKeyInput (GLFWwindow* window) : window{window} {
    buttonIds["key_space"] = GLFW_KEY_SPACE;
    buttonIds["key_apostrophe"] = GLFW_KEY_APOSTROPHE;
    buttonIds["key_comma"] = GLFW_KEY_COMMA;
    buttonIds["key_minus"] = GLFW_KEY_MINUS;
    buttonIds["key_period"] = GLFW_KEY_PERIOD;
    buttonIds["key_slash"] = GLFW_KEY_SLASH;

    buttonIds["key_0"] = GLFW_KEY_0;
    buttonIds["key_1"] = GLFW_KEY_1;
    buttonIds["key_2"] = GLFW_KEY_2;
    buttonIds["key_3"] = GLFW_KEY_3;
    buttonIds["key_4"] = GLFW_KEY_4;
    buttonIds["key_5"] = GLFW_KEY_5;
    buttonIds["key_6"] = GLFW_KEY_6;
    buttonIds["key_7"] = GLFW_KEY_7;
    buttonIds["key_8"] = GLFW_KEY_8;
    buttonIds["key_9"] = GLFW_KEY_9;

    buttonIds["key_semicolon"] = GLFW_KEY_SEMICOLON;
    buttonIds["key_equal"] = GLFW_KEY_EQUAL;

    buttonIds["key_a"] = GLFW_KEY_A;
    buttonIds["key_b"] = GLFW_KEY_B;
    buttonIds["key_c"] = GLFW_KEY_C;
    buttonIds["key_d"] = GLFW_KEY_D;
    buttonIds["key_e"] = GLFW_KEY_E;
    buttonIds["key_f"] = GLFW_KEY_F;
    buttonIds["key_g"] = GLFW_KEY_G;
    buttonIds["key_h"] = GLFW_KEY_H;
    buttonIds["key_i"] = GLFW_KEY_I;
    buttonIds["key_j"] = GLFW_KEY_J;
    buttonIds["key_k"] = GLFW_KEY_K;
    buttonIds["key_l"] = GLFW_KEY_L;
    buttonIds["key_m"] = GLFW_KEY_M;
    buttonIds["key_n"] = GLFW_KEY_N;
    buttonIds["key_o"] = GLFW_KEY_O;
    buttonIds["key_p"] = GLFW_KEY_P;
    buttonIds["key_q"] = GLFW_KEY_Q;
    buttonIds["key_r"] = GLFW_KEY_R;
    buttonIds["key_s"] = GLFW_KEY_S;
    buttonIds["key_t"] = GLFW_KEY_T;
    buttonIds["key_u"] = GLFW_KEY_U;
    buttonIds["key_v"] = GLFW_KEY_V;
    buttonIds["key_w"] = GLFW_KEY_W;
    buttonIds["key_x"] = GLFW_KEY_X;
    buttonIds["key_y"] = GLFW_KEY_Y;
    buttonIds["key_z"] = GLFW_KEY_Z;

    buttonIds["key_lbracket"] = GLFW_KEY_LEFT_BRACKET;
    buttonIds["key_backslash"] = GLFW_KEY_BACKSLASH;
    buttonIds["key_rbracket"] = GLFW_KEY_RIGHT_BRACKET;
    buttonIds["key_grave"] = GLFW_KEY_GRAVE_ACCENT;
    buttonIds["key_world1"] = GLFW_KEY_WORLD_1;
    buttonIds["key_world2"] = GLFW_KEY_WORLD_2;
    buttonIds["key_escape"] = GLFW_KEY_ESCAPE;
    buttonIds["key_enter"] = GLFW_KEY_ENTER;
    buttonIds["key_tab"] = GLFW_KEY_TAB;
    buttonIds["key_backspace"] = GLFW_KEY_BACKSPACE;
    buttonIds["key_insert"] = GLFW_KEY_INSERT;
    buttonIds["key_delete"] = GLFW_KEY_DELETE;

    buttonIds["key_right"] = GLFW_KEY_RIGHT;
    buttonIds["key_left"] = GLFW_KEY_LEFT;
    buttonIds["key_down"] = GLFW_KEY_DOWN;
    buttonIds["key_up"] = GLFW_KEY_UP;

    buttonIds["key_page_up"] = GLFW_KEY_PAGE_UP;
    buttonIds["key_page_down"] = GLFW_KEY_PAGE_DOWN;
    buttonIds["key_home"] = GLFW_KEY_HOME;
    buttonIds["key_end"] = GLFW_KEY_END;
    buttonIds["key_caps_lock"] = GLFW_KEY_CAPS_LOCK;
    buttonIds["key_scroll_lock"] = GLFW_KEY_SCROLL_LOCK;
    buttonIds["key_num_lock"] = GLFW_KEY_NUM_LOCK;
    buttonIds["key_print_screen"] = GLFW_KEY_PRINT_SCREEN;
    buttonIds["key_pause"] = GLFW_KEY_PAUSE;

    buttonIds["key_f1"] = GLFW_KEY_F1;
    buttonIds["key_f2"] = GLFW_KEY_F2;
    buttonIds["key_f3"] = GLFW_KEY_F3;
    buttonIds["key_f4"] = GLFW_KEY_F4;
    buttonIds["key_f5"] = GLFW_KEY_F5;
    buttonIds["key_f6"] = GLFW_KEY_F6;
    buttonIds["key_f7"] = GLFW_KEY_F7;
    buttonIds["key_f8"] = GLFW_KEY_F8;
    buttonIds["key_f9"] = GLFW_KEY_F9;
    buttonIds["key_f10"] = GLFW_KEY_F10;
    buttonIds["key_f11"] = GLFW_KEY_F11;
    buttonIds["key_f12"] = GLFW_KEY_F12;
    buttonIds["key_f13"] = GLFW_KEY_F13;
    buttonIds["key_f14"] = GLFW_KEY_F14;
    buttonIds["key_f15"] = GLFW_KEY_F15;
    buttonIds["key_f16"] = GLFW_KEY_F16;
    buttonIds["key_f17"] = GLFW_KEY_F17;
    buttonIds["key_f18"] = GLFW_KEY_F18;
    buttonIds["key_f19"] = GLFW_KEY_F19;
    buttonIds["key_f20"] = GLFW_KEY_F20;
    buttonIds["key_f21"] = GLFW_KEY_F21;
    buttonIds["key_f22"] = GLFW_KEY_F22;
    buttonIds["key_f23"] = GLFW_KEY_F23;
    buttonIds["key_f24"] = GLFW_KEY_F24;
    buttonIds["key_f25"] = GLFW_KEY_F25;

    buttonIds["key_kp_0"] = GLFW_KEY_KP_0;
    buttonIds["key_kp_1"] = GLFW_KEY_KP_1;
    buttonIds["key_kp_2"] = GLFW_KEY_KP_2;
    buttonIds["key_kp_3"] = GLFW_KEY_KP_3;
    buttonIds["key_kp_4"] = GLFW_KEY_KP_4;
    buttonIds["key_kp_5"] = GLFW_KEY_KP_5;
    buttonIds["key_kp_6"] = GLFW_KEY_KP_6;
    buttonIds["key_kp_7"] = GLFW_KEY_KP_7;
    buttonIds["key_kp_8"] = GLFW_KEY_KP_8;
    buttonIds["key_kp_9"] = GLFW_KEY_KP_9;
    buttonIds["key_kp_dec"] = GLFW_KEY_KP_DECIMAL;
    buttonIds["key_kp_div"] = GLFW_KEY_KP_DIVIDE;
    buttonIds["key_kp_mul"] = GLFW_KEY_KP_MULTIPLY;
    buttonIds["key_kp_sub"] = GLFW_KEY_KP_SUBTRACT;
    buttonIds["key_kp_add"] = GLFW_KEY_KP_ADD;
    buttonIds["key_kp_enter"] = GLFW_KEY_KP_ENTER;
    buttonIds["key_kp_equal"] = GLFW_KEY_KP_EQUAL;

    buttonIds["key_lshift"] = GLFW_KEY_LEFT_SHIFT;
    buttonIds["key_lcontrol"] = GLFW_KEY_LEFT_CONTROL;
    buttonIds["key_lalt"] = GLFW_KEY_LEFT_ALT;
    buttonIds["key_lsuper"] = GLFW_KEY_LEFT_SUPER;
    buttonIds["key_rshift"] = GLFW_KEY_RIGHT_SHIFT;
    buttonIds["key_rcontrol"] = GLFW_KEY_RIGHT_CONTROL;
    buttonIds["key_ralt"] = GLFW_KEY_RIGHT_ALT;
    buttonIds["key_rsuper"] = GLFW_KEY_RIGHT_SUPER;
    buttonIds["key_menu"] = GLFW_KEY_MENU;
}

const phenyl::core::ButtonInputSource* GLFWKeyInput::getButtonSource (std::string_view sourcePath) {
    auto idIt = buttonIds.find(sourcePath);
    if (idIt == buttonIds.end()) {
        PHENYL_LOGE(LOGGER, "Invalid key: \"{}\"", sourcePath);
        return nullptr;
    }

    auto sourceIt = sources.find(idIt->second);
    if (sourceIt != sources.end()) {
        return &sourceIt->second;
    }

    auto it = sources.emplace(idIt->second, core::ButtonInputSource{}).first;
    return &it->second;
}

const phenyl::core::Axis2DInputSource* GLFWKeyInput::getAxis2DSource (std::string_view sourcePath) {
    PHENYL_LOGE(LOGGER, "Attempted to get Axis2D source, but keyboard input does not support axis sources");
    return nullptr;
}

std::string_view GLFWKeyInput::getDeviceId () const noexcept {
    return "keyboard";
}

void GLFWKeyInput::poll () {
    for (auto& [id, source] : sources) {
        source.setState(glfwGetKey(window, id) == GLFW_PRESS);
    }
}
