#include "glfw/glfw_headers.h"
#include "glfw_key_input.h"

#include "logging/logging.h"

using namespace phenyl::glfw;

static phenyl::Logger LOGGER{"GLFW_KEY_INPUT", phenyl::graphics::detail::GRAPHICS_LOGGER};

GLFWKeyInput::GLFWKeyInput (GLFWwindow* window) : m_window{window} {
    m_buttonIds["key_space"] = GLFW_KEY_SPACE;
    m_buttonIds["key_apostrophe"] = GLFW_KEY_APOSTROPHE;
    m_buttonIds["key_comma"] = GLFW_KEY_COMMA;
    m_buttonIds["key_minus"] = GLFW_KEY_MINUS;
    m_buttonIds["key_period"] = GLFW_KEY_PERIOD;
    m_buttonIds["key_slash"] = GLFW_KEY_SLASH;

    m_buttonIds["key_0"] = GLFW_KEY_0;
    m_buttonIds["key_1"] = GLFW_KEY_1;
    m_buttonIds["key_2"] = GLFW_KEY_2;
    m_buttonIds["key_3"] = GLFW_KEY_3;
    m_buttonIds["key_4"] = GLFW_KEY_4;
    m_buttonIds["key_5"] = GLFW_KEY_5;
    m_buttonIds["key_6"] = GLFW_KEY_6;
    m_buttonIds["key_7"] = GLFW_KEY_7;
    m_buttonIds["key_8"] = GLFW_KEY_8;
    m_buttonIds["key_9"] = GLFW_KEY_9;

    m_buttonIds["key_semicolon"] = GLFW_KEY_SEMICOLON;
    m_buttonIds["key_equal"] = GLFW_KEY_EQUAL;

    m_buttonIds["key_a"] = GLFW_KEY_A;
    m_buttonIds["key_b"] = GLFW_KEY_B;
    m_buttonIds["key_c"] = GLFW_KEY_C;
    m_buttonIds["key_d"] = GLFW_KEY_D;
    m_buttonIds["key_e"] = GLFW_KEY_E;
    m_buttonIds["key_f"] = GLFW_KEY_F;
    m_buttonIds["key_g"] = GLFW_KEY_G;
    m_buttonIds["key_h"] = GLFW_KEY_H;
    m_buttonIds["key_i"] = GLFW_KEY_I;
    m_buttonIds["key_j"] = GLFW_KEY_J;
    m_buttonIds["key_k"] = GLFW_KEY_K;
    m_buttonIds["key_l"] = GLFW_KEY_L;
    m_buttonIds["key_m"] = GLFW_KEY_M;
    m_buttonIds["key_n"] = GLFW_KEY_N;
    m_buttonIds["key_o"] = GLFW_KEY_O;
    m_buttonIds["key_p"] = GLFW_KEY_P;
    m_buttonIds["key_q"] = GLFW_KEY_Q;
    m_buttonIds["key_r"] = GLFW_KEY_R;
    m_buttonIds["key_s"] = GLFW_KEY_S;
    m_buttonIds["key_t"] = GLFW_KEY_T;
    m_buttonIds["key_u"] = GLFW_KEY_U;
    m_buttonIds["key_v"] = GLFW_KEY_V;
    m_buttonIds["key_w"] = GLFW_KEY_W;
    m_buttonIds["key_x"] = GLFW_KEY_X;
    m_buttonIds["key_y"] = GLFW_KEY_Y;
    m_buttonIds["key_z"] = GLFW_KEY_Z;

    m_buttonIds["key_lbracket"] = GLFW_KEY_LEFT_BRACKET;
    m_buttonIds["key_backslash"] = GLFW_KEY_BACKSLASH;
    m_buttonIds["key_rbracket"] = GLFW_KEY_RIGHT_BRACKET;
    m_buttonIds["key_grave"] = GLFW_KEY_GRAVE_ACCENT;
    m_buttonIds["key_world1"] = GLFW_KEY_WORLD_1;
    m_buttonIds["key_world2"] = GLFW_KEY_WORLD_2;
    m_buttonIds["key_escape"] = GLFW_KEY_ESCAPE;
    m_buttonIds["key_enter"] = GLFW_KEY_ENTER;
    m_buttonIds["key_tab"] = GLFW_KEY_TAB;
    m_buttonIds["key_backspace"] = GLFW_KEY_BACKSPACE;
    m_buttonIds["key_insert"] = GLFW_KEY_INSERT;
    m_buttonIds["key_delete"] = GLFW_KEY_DELETE;

    m_buttonIds["key_right"] = GLFW_KEY_RIGHT;
    m_buttonIds["key_left"] = GLFW_KEY_LEFT;
    m_buttonIds["key_down"] = GLFW_KEY_DOWN;
    m_buttonIds["key_up"] = GLFW_KEY_UP;

    m_buttonIds["key_page_up"] = GLFW_KEY_PAGE_UP;
    m_buttonIds["key_page_down"] = GLFW_KEY_PAGE_DOWN;
    m_buttonIds["key_home"] = GLFW_KEY_HOME;
    m_buttonIds["key_end"] = GLFW_KEY_END;
    m_buttonIds["key_caps_lock"] = GLFW_KEY_CAPS_LOCK;
    m_buttonIds["key_scroll_lock"] = GLFW_KEY_SCROLL_LOCK;
    m_buttonIds["key_num_lock"] = GLFW_KEY_NUM_LOCK;
    m_buttonIds["key_print_screen"] = GLFW_KEY_PRINT_SCREEN;
    m_buttonIds["key_pause"] = GLFW_KEY_PAUSE;

    m_buttonIds["key_f1"] = GLFW_KEY_F1;
    m_buttonIds["key_f2"] = GLFW_KEY_F2;
    m_buttonIds["key_f3"] = GLFW_KEY_F3;
    m_buttonIds["key_f4"] = GLFW_KEY_F4;
    m_buttonIds["key_f5"] = GLFW_KEY_F5;
    m_buttonIds["key_f6"] = GLFW_KEY_F6;
    m_buttonIds["key_f7"] = GLFW_KEY_F7;
    m_buttonIds["key_f8"] = GLFW_KEY_F8;
    m_buttonIds["key_f9"] = GLFW_KEY_F9;
    m_buttonIds["key_f10"] = GLFW_KEY_F10;
    m_buttonIds["key_f11"] = GLFW_KEY_F11;
    m_buttonIds["key_f12"] = GLFW_KEY_F12;
    m_buttonIds["key_f13"] = GLFW_KEY_F13;
    m_buttonIds["key_f14"] = GLFW_KEY_F14;
    m_buttonIds["key_f15"] = GLFW_KEY_F15;
    m_buttonIds["key_f16"] = GLFW_KEY_F16;
    m_buttonIds["key_f17"] = GLFW_KEY_F17;
    m_buttonIds["key_f18"] = GLFW_KEY_F18;
    m_buttonIds["key_f19"] = GLFW_KEY_F19;
    m_buttonIds["key_f20"] = GLFW_KEY_F20;
    m_buttonIds["key_f21"] = GLFW_KEY_F21;
    m_buttonIds["key_f22"] = GLFW_KEY_F22;
    m_buttonIds["key_f23"] = GLFW_KEY_F23;
    m_buttonIds["key_f24"] = GLFW_KEY_F24;
    m_buttonIds["key_f25"] = GLFW_KEY_F25;

    m_buttonIds["key_kp_0"] = GLFW_KEY_KP_0;
    m_buttonIds["key_kp_1"] = GLFW_KEY_KP_1;
    m_buttonIds["key_kp_2"] = GLFW_KEY_KP_2;
    m_buttonIds["key_kp_3"] = GLFW_KEY_KP_3;
    m_buttonIds["key_kp_4"] = GLFW_KEY_KP_4;
    m_buttonIds["key_kp_5"] = GLFW_KEY_KP_5;
    m_buttonIds["key_kp_6"] = GLFW_KEY_KP_6;
    m_buttonIds["key_kp_7"] = GLFW_KEY_KP_7;
    m_buttonIds["key_kp_8"] = GLFW_KEY_KP_8;
    m_buttonIds["key_kp_9"] = GLFW_KEY_KP_9;
    m_buttonIds["key_kp_dec"] = GLFW_KEY_KP_DECIMAL;
    m_buttonIds["key_kp_div"] = GLFW_KEY_KP_DIVIDE;
    m_buttonIds["key_kp_mul"] = GLFW_KEY_KP_MULTIPLY;
    m_buttonIds["key_kp_sub"] = GLFW_KEY_KP_SUBTRACT;
    m_buttonIds["key_kp_add"] = GLFW_KEY_KP_ADD;
    m_buttonIds["key_kp_enter"] = GLFW_KEY_KP_ENTER;
    m_buttonIds["key_kp_equal"] = GLFW_KEY_KP_EQUAL;

    m_buttonIds["key_lshift"] = GLFW_KEY_LEFT_SHIFT;
    m_buttonIds["key_lcontrol"] = GLFW_KEY_LEFT_CONTROL;
    m_buttonIds["key_lalt"] = GLFW_KEY_LEFT_ALT;
    m_buttonIds["key_lsuper"] = GLFW_KEY_LEFT_SUPER;
    m_buttonIds["key_rshift"] = GLFW_KEY_RIGHT_SHIFT;
    m_buttonIds["key_rcontrol"] = GLFW_KEY_RIGHT_CONTROL;
    m_buttonIds["key_ralt"] = GLFW_KEY_RIGHT_ALT;
    m_buttonIds["key_rsuper"] = GLFW_KEY_RIGHT_SUPER;
    m_buttonIds["key_menu"] = GLFW_KEY_MENU;
}

const phenyl::core::ButtonInputSource* GLFWKeyInput::getButtonSource (std::string_view sourcePath) {
    auto idIt = m_buttonIds.find(sourcePath);
    if (idIt == m_buttonIds.end()) {
        PHENYL_LOGE(LOGGER, "Invalid key: \"{}\"", sourcePath);
        return nullptr;
    }

    auto sourceIt = m_sources.find(idIt->second);
    if (sourceIt != m_sources.end()) {
        return &sourceIt->second;
    }

    auto it = m_sources.emplace(idIt->second, core::ButtonInputSource{}).first;
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
    for (auto& [id, source] : m_sources) {
        source.setState(glfwGetKey(m_window, id) == GLFW_PRESS);
    }
}
