#include <vector>

#include "glfw_key_input.h"

using namespace phenyl;

long graphics::GLFWKeyInput::getInputNum (const std::string& inputStr) {
    if (keyMap.contains(inputStr)) {
        return keyMap.at(inputStr);
    }
    return -1;
}

bool graphics::GLFWKeyInput::isDown (long inputNum) {
    if (consumed.contains(inputNum)) {
        return false;
    }
    return glfwGetKey(window, (int)inputNum) == GLFW_PRESS;
}

void graphics::GLFWKeyInput::consume (long inputNum) {
    consumed.insert(inputNum);
}


void graphics::GLFWKeyInput::update () {
    std::vector<long> toRemove;
    for (auto k : consumed) {
        if (glfwGetKey(window, (int)k) == GLFW_RELEASE) {
            toRemove.push_back(k);
        }
    }

    for (auto l : toRemove) {
        consumed.remove(l);
    }
}

std::size_t graphics::GLFWKeyInput::getStateNum (long inputNum) {
    if (!stateNums.contains(inputNum)) {
        return 0;
    } else {
        
    }
    return 0;
}


void graphics::GLFWKeyInput::setupKeys () {
    keyMap["key_space"] = GLFW_KEY_SPACE;
    keyMap["key_apostrophe"] = GLFW_KEY_APOSTROPHE;
    keyMap["key_comma"] = GLFW_KEY_COMMA;
    keyMap["key_minus"] = GLFW_KEY_MINUS;
    keyMap["key_period"] = GLFW_KEY_PERIOD;
    keyMap["key_slash"] = GLFW_KEY_SLASH;

    keyMap["key_0"] = GLFW_KEY_0;
    keyMap["key_1"] = GLFW_KEY_1;
    keyMap["key_2"] = GLFW_KEY_2;
    keyMap["key_3"] = GLFW_KEY_3;
    keyMap["key_4"] = GLFW_KEY_4;
    keyMap["key_5"] = GLFW_KEY_5;
    keyMap["key_6"] = GLFW_KEY_6;
    keyMap["key_7"] = GLFW_KEY_7;
    keyMap["key_8"] = GLFW_KEY_8;
    keyMap["key_9"] = GLFW_KEY_9;

    keyMap["key_semicolon"] = GLFW_KEY_SEMICOLON;
    keyMap["key_equal"] = GLFW_KEY_EQUAL;

    keyMap["key_a"] = GLFW_KEY_A;
    keyMap["key_b"] = GLFW_KEY_B;
    keyMap["key_c"] = GLFW_KEY_C;
    keyMap["key_d"] = GLFW_KEY_D;
    keyMap["key_e"] = GLFW_KEY_E;
    keyMap["key_f"] = GLFW_KEY_F;
    keyMap["key_g"] = GLFW_KEY_G;
    keyMap["key_h"] = GLFW_KEY_H;
    keyMap["key_i"] = GLFW_KEY_I;
    keyMap["key_j"] = GLFW_KEY_J;
    keyMap["key_k"] = GLFW_KEY_K;
    keyMap["key_l"] = GLFW_KEY_L;
    keyMap["key_m"] = GLFW_KEY_M;
    keyMap["key_n"] = GLFW_KEY_N;
    keyMap["key_o"] = GLFW_KEY_O;
    keyMap["key_p"] = GLFW_KEY_P;
    keyMap["key_q"] = GLFW_KEY_Q;
    keyMap["key_r"] = GLFW_KEY_R;
    keyMap["key_s"] = GLFW_KEY_S;
    keyMap["key_t"] = GLFW_KEY_T;
    keyMap["key_u"] = GLFW_KEY_U;
    keyMap["key_v"] = GLFW_KEY_V;
    keyMap["key_w"] = GLFW_KEY_W;
    keyMap["key_x"] = GLFW_KEY_X;
    keyMap["key_y"] = GLFW_KEY_Y;
    keyMap["key_z"] = GLFW_KEY_Z;

    keyMap["key_lbracket"] = GLFW_KEY_LEFT_BRACKET;
    keyMap["key_backslash"] = GLFW_KEY_BACKSLASH;
    keyMap["key_rbracket"] = GLFW_KEY_RIGHT_BRACKET;
    keyMap["key_grave"] = GLFW_KEY_GRAVE_ACCENT;
    keyMap["key_world1"] = GLFW_KEY_WORLD_1;
    keyMap["key_world2"] = GLFW_KEY_WORLD_2;
    keyMap["key_escape"] = GLFW_KEY_ESCAPE;
    keyMap["key_enter"] = GLFW_KEY_ENTER;
    keyMap["key_tab"] = GLFW_KEY_TAB;
    keyMap["key_backspace"] = GLFW_KEY_BACKSPACE;
    keyMap["key_insert"] = GLFW_KEY_INSERT;
    keyMap["key_delete"] = GLFW_KEY_DELETE;

    keyMap["key_right"] = GLFW_KEY_RIGHT;
    keyMap["key_left"] = GLFW_KEY_LEFT;
    keyMap["key_down"] = GLFW_KEY_DOWN;
    keyMap["key_up"] = GLFW_KEY_UP;

    keyMap["key_page_up"] = GLFW_KEY_PAGE_UP;
    keyMap["key_page_down"] = GLFW_KEY_PAGE_DOWN;
    keyMap["key_home"] = GLFW_KEY_HOME;
    keyMap["key_end"] = GLFW_KEY_END;
    keyMap["key_caps_lock"] = GLFW_KEY_CAPS_LOCK;
    keyMap["key_scroll_lock"] = GLFW_KEY_SCROLL_LOCK;
    keyMap["key_num_lock"] = GLFW_KEY_NUM_LOCK;
    keyMap["key_print_screen"] = GLFW_KEY_PRINT_SCREEN;
    keyMap["key_pause"] = GLFW_KEY_PAUSE;

    keyMap["key_f1"] = GLFW_KEY_F1;
    keyMap["key_f2"] = GLFW_KEY_F2;
    keyMap["key_f3"] = GLFW_KEY_F3;
    keyMap["key_f4"] = GLFW_KEY_F4;
    keyMap["key_f5"] = GLFW_KEY_F5;
    keyMap["key_f6"] = GLFW_KEY_F6;
    keyMap["key_f7"] = GLFW_KEY_F7;
    keyMap["key_f8"] = GLFW_KEY_F8;
    keyMap["key_f9"] = GLFW_KEY_F9;
    keyMap["key_f10"] = GLFW_KEY_F10;
    keyMap["key_f11"] = GLFW_KEY_F11;
    keyMap["key_f12"] = GLFW_KEY_F12;
    keyMap["key_f13"] = GLFW_KEY_F13;
    keyMap["key_f14"] = GLFW_KEY_F14;
    keyMap["key_f15"] = GLFW_KEY_F15;
    keyMap["key_f16"] = GLFW_KEY_F16;
    keyMap["key_f17"] = GLFW_KEY_F17;
    keyMap["key_f18"] = GLFW_KEY_F18;
    keyMap["key_f19"] = GLFW_KEY_F19;
    keyMap["key_f20"] = GLFW_KEY_F20;
    keyMap["key_f21"] = GLFW_KEY_F21;
    keyMap["key_f22"] = GLFW_KEY_F22;
    keyMap["key_f23"] = GLFW_KEY_F23;
    keyMap["key_f24"] = GLFW_KEY_F24;
    keyMap["key_f25"] = GLFW_KEY_F25;

    keyMap["key_kp_0"] = GLFW_KEY_KP_0;
    keyMap["key_kp_1"] = GLFW_KEY_KP_1;
    keyMap["key_kp_2"] = GLFW_KEY_KP_2;
    keyMap["key_kp_3"] = GLFW_KEY_KP_3;
    keyMap["key_kp_4"] = GLFW_KEY_KP_4;
    keyMap["key_kp_5"] = GLFW_KEY_KP_5;
    keyMap["key_kp_6"] = GLFW_KEY_KP_6;
    keyMap["key_kp_7"] = GLFW_KEY_KP_7;
    keyMap["key_kp_8"] = GLFW_KEY_KP_8;
    keyMap["key_kp_9"] = GLFW_KEY_KP_9;
    keyMap["key_kp_dec"] = GLFW_KEY_KP_DECIMAL;
    keyMap["key_kp_div"] = GLFW_KEY_KP_DIVIDE;
    keyMap["key_kp_mul"] = GLFW_KEY_KP_MULTIPLY;
    keyMap["key_kp_sub"] = GLFW_KEY_KP_SUBTRACT;
    keyMap["key_kp_add"] = GLFW_KEY_KP_ADD;
    keyMap["key_kp_enter"] = GLFW_KEY_KP_ENTER;
    keyMap["key_kp_equal"] = GLFW_KEY_KP_EQUAL;

    keyMap["key_lshift"] = GLFW_KEY_LEFT_SHIFT;
    keyMap["key_lcontrol"] = GLFW_KEY_LEFT_CONTROL;
    keyMap["key_lalt"] = GLFW_KEY_LEFT_ALT;
    keyMap["key_lsuper"] = GLFW_KEY_LEFT_SUPER;
    keyMap["key_rshift"] = GLFW_KEY_RIGHT_SHIFT;
    keyMap["key_rcontrol"] = GLFW_KEY_RIGHT_CONTROL;
    keyMap["key_ralt"] = GLFW_KEY_RIGHT_ALT;
    keyMap["key_rsuper"] = GLFW_KEY_RIGHT_SUPER;
    keyMap["key_menu"] = GLFW_KEY_MENU;
}

graphics::GLFWKeyInput2::GLFWKeyInput2 () {
    addButton("key_space", glfwGetKeyScancode(GLFW_KEY_SPACE));
    addButton("key_apostrophe", glfwGetKeyScancode(GLFW_KEY_APOSTROPHE));
    addButton("key_comma", glfwGetKeyScancode(GLFW_KEY_COMMA));
    addButton("key_minus", glfwGetKeyScancode(GLFW_KEY_MINUS));
    addButton("key_period", glfwGetKeyScancode(GLFW_KEY_PERIOD));
    addButton("key_slash", glfwGetKeyScancode(GLFW_KEY_SLASH));

    addButton("key_0", glfwGetKeyScancode(GLFW_KEY_0));
    addButton("key_1", glfwGetKeyScancode(GLFW_KEY_1));
    addButton("key_2", glfwGetKeyScancode(GLFW_KEY_2));
    addButton("key_3", glfwGetKeyScancode(GLFW_KEY_3));
    addButton("key_4", glfwGetKeyScancode(GLFW_KEY_4));
    addButton("key_5", glfwGetKeyScancode(GLFW_KEY_5));
    addButton("key_6", glfwGetKeyScancode(GLFW_KEY_6));
    addButton("key_7", glfwGetKeyScancode(GLFW_KEY_7));
    addButton("key_8", glfwGetKeyScancode(GLFW_KEY_8));
    addButton("key_9", glfwGetKeyScancode(GLFW_KEY_9));

    addButton("key_semicolon", glfwGetKeyScancode(GLFW_KEY_SEMICOLON));
    addButton("key_equal", glfwGetKeyScancode(GLFW_KEY_EQUAL));

    addButton("key_a", glfwGetKeyScancode(GLFW_KEY_A));
    addButton("key_b", glfwGetKeyScancode(GLFW_KEY_B));
    addButton("key_c", glfwGetKeyScancode(GLFW_KEY_C));
    addButton("key_d", glfwGetKeyScancode(GLFW_KEY_D));
    addButton("key_e", glfwGetKeyScancode(GLFW_KEY_E));
    addButton("key_f", glfwGetKeyScancode(GLFW_KEY_F));
    addButton("key_g", glfwGetKeyScancode(GLFW_KEY_G));
    addButton("key_h", glfwGetKeyScancode(GLFW_KEY_H));
    addButton("key_i", glfwGetKeyScancode(GLFW_KEY_I));
    addButton("key_j", glfwGetKeyScancode(GLFW_KEY_J));
    addButton("key_k", glfwGetKeyScancode(GLFW_KEY_K));
    addButton("key_l", glfwGetKeyScancode(GLFW_KEY_L));
    addButton("key_m", glfwGetKeyScancode(GLFW_KEY_M));
    addButton("key_n", glfwGetKeyScancode(GLFW_KEY_N));
    addButton("key_o", glfwGetKeyScancode(GLFW_KEY_O));
    addButton("key_p", glfwGetKeyScancode(GLFW_KEY_P));
    addButton("key_q", glfwGetKeyScancode(GLFW_KEY_Q));
    addButton("key_r", glfwGetKeyScancode(GLFW_KEY_R));
    addButton("key_s", glfwGetKeyScancode(GLFW_KEY_S));
    addButton("key_t", glfwGetKeyScancode(GLFW_KEY_T));
    addButton("key_u", glfwGetKeyScancode(GLFW_KEY_U));
    addButton("key_v", glfwGetKeyScancode(GLFW_KEY_V));
    addButton("key_w", glfwGetKeyScancode(GLFW_KEY_W));
    addButton("key_x", glfwGetKeyScancode(GLFW_KEY_X));
    addButton("key_y", glfwGetKeyScancode(GLFW_KEY_Y));
    addButton("key_z", glfwGetKeyScancode(GLFW_KEY_Z));

    addButton("key_lbracket", glfwGetKeyScancode(GLFW_KEY_LEFT_BRACKET));
    addButton("key_backslash", glfwGetKeyScancode(GLFW_KEY_BACKSLASH));
    addButton("key_rbracket", glfwGetKeyScancode(GLFW_KEY_RIGHT_BRACKET));
    addButton("key_grave", glfwGetKeyScancode(GLFW_KEY_GRAVE_ACCENT));
    addButton("key_world1", glfwGetKeyScancode(GLFW_KEY_WORLD_1));
    addButton("key_world2", glfwGetKeyScancode(GLFW_KEY_WORLD_2));
    addButton("key_escape", glfwGetKeyScancode(GLFW_KEY_ESCAPE));
    addButton("key_enter", glfwGetKeyScancode(GLFW_KEY_ENTER));
    addButton("key_tab", glfwGetKeyScancode(GLFW_KEY_TAB));
    addButton("key_backspace", glfwGetKeyScancode(GLFW_KEY_BACKSPACE));
    addButton("key_insert", glfwGetKeyScancode(GLFW_KEY_INSERT));
    addButton("key_delete", glfwGetKeyScancode(GLFW_KEY_DELETE));

    addButton("key_right", glfwGetKeyScancode(GLFW_KEY_RIGHT));
    addButton("key_left", glfwGetKeyScancode(GLFW_KEY_LEFT));
    addButton("key_down", glfwGetKeyScancode(GLFW_KEY_DOWN));
    addButton("key_up", glfwGetKeyScancode(GLFW_KEY_UP));

    addButton("key_page_up", glfwGetKeyScancode(GLFW_KEY_PAGE_UP));
    addButton("key_page_down", glfwGetKeyScancode(GLFW_KEY_PAGE_DOWN));
    addButton("key_home", glfwGetKeyScancode(GLFW_KEY_HOME));
    addButton("key_end", glfwGetKeyScancode(GLFW_KEY_END));
    addButton("key_caps_lock", glfwGetKeyScancode(GLFW_KEY_CAPS_LOCK));
    addButton("key_scroll_lock", glfwGetKeyScancode(GLFW_KEY_SCROLL_LOCK));
    addButton("key_num_lock", glfwGetKeyScancode(GLFW_KEY_NUM_LOCK));
    addButton("key_print_screen", glfwGetKeyScancode(GLFW_KEY_PRINT_SCREEN));
    addButton("key_pause", glfwGetKeyScancode(GLFW_KEY_PAUSE));

    addButton("key_f1", glfwGetKeyScancode(GLFW_KEY_F1));
    addButton("key_f2", glfwGetKeyScancode(GLFW_KEY_F2));
    addButton("key_f3", glfwGetKeyScancode(GLFW_KEY_F3));
    addButton("key_f4", glfwGetKeyScancode(GLFW_KEY_F4));
    addButton("key_f5", glfwGetKeyScancode(GLFW_KEY_F5));
    addButton("key_f6", glfwGetKeyScancode(GLFW_KEY_F6));
    addButton("key_f7", glfwGetKeyScancode(GLFW_KEY_F7));
    addButton("key_f8", glfwGetKeyScancode(GLFW_KEY_F8));
    addButton("key_f9", glfwGetKeyScancode(GLFW_KEY_F9));
    addButton("key_f10", glfwGetKeyScancode(GLFW_KEY_F10));
    addButton("key_f11", glfwGetKeyScancode(GLFW_KEY_F11));
    addButton("key_f12", glfwGetKeyScancode(GLFW_KEY_F12));
    addButton("key_f13", glfwGetKeyScancode(GLFW_KEY_F13));
    addButton("key_f14", glfwGetKeyScancode(GLFW_KEY_F14));
    addButton("key_f15", glfwGetKeyScancode(GLFW_KEY_F15));
    addButton("key_f16", glfwGetKeyScancode(GLFW_KEY_F16));
    addButton("key_f17", glfwGetKeyScancode(GLFW_KEY_F17));
    addButton("key_f18", glfwGetKeyScancode(GLFW_KEY_F18));
    addButton("key_f19", glfwGetKeyScancode(GLFW_KEY_F19));
    addButton("key_f20", glfwGetKeyScancode(GLFW_KEY_F20));
    addButton("key_f21", glfwGetKeyScancode(GLFW_KEY_F21));
    addButton("key_f22", glfwGetKeyScancode(GLFW_KEY_F22));
    addButton("key_f23", glfwGetKeyScancode(GLFW_KEY_F23));
    addButton("key_f24", glfwGetKeyScancode(GLFW_KEY_F24));
    addButton("key_f25", glfwGetKeyScancode(GLFW_KEY_F25));

    addButton("key_kp_0", glfwGetKeyScancode(GLFW_KEY_KP_0));
    addButton("key_kp_1", glfwGetKeyScancode(GLFW_KEY_KP_1));
    addButton("key_kp_2", glfwGetKeyScancode(GLFW_KEY_KP_2));
    addButton("key_kp_3", glfwGetKeyScancode(GLFW_KEY_KP_3));
    addButton("key_kp_4", glfwGetKeyScancode(GLFW_KEY_KP_4));
    addButton("key_kp_5", glfwGetKeyScancode(GLFW_KEY_KP_5));
    addButton("key_kp_6", glfwGetKeyScancode(GLFW_KEY_KP_6));
    addButton("key_kp_7", glfwGetKeyScancode(GLFW_KEY_KP_7));
    addButton("key_kp_8", glfwGetKeyScancode(GLFW_KEY_KP_8));
    addButton("key_kp_9", glfwGetKeyScancode(GLFW_KEY_KP_9));
    addButton("key_kp_dec", glfwGetKeyScancode(GLFW_KEY_KP_DECIMAL));
    addButton("key_kp_div", glfwGetKeyScancode(GLFW_KEY_KP_DIVIDE));
    addButton("key_kp_mul", glfwGetKeyScancode(GLFW_KEY_KP_MULTIPLY));
    addButton("key_kp_sub", glfwGetKeyScancode(GLFW_KEY_KP_SUBTRACT));
    addButton("key_kp_add", glfwGetKeyScancode(GLFW_KEY_KP_ADD));
    addButton("key_kp_enter", glfwGetKeyScancode(GLFW_KEY_KP_ENTER));
    addButton("key_kp_equal", glfwGetKeyScancode(GLFW_KEY_KP_EQUAL));

    addButton("key_lshift", glfwGetKeyScancode(GLFW_KEY_LEFT_SHIFT));
    addButton("key_lcontrol", glfwGetKeyScancode(GLFW_KEY_LEFT_CONTROL));
    addButton("key_lalt", glfwGetKeyScancode(GLFW_KEY_LEFT_ALT));
    addButton("key_lsuper", glfwGetKeyScancode(GLFW_KEY_LEFT_SUPER));
    addButton("key_rshift", glfwGetKeyScancode(GLFW_KEY_RIGHT_SHIFT));
    addButton("key_rcontrol", glfwGetKeyScancode(GLFW_KEY_RIGHT_CONTROL));
    addButton("key_ralt", glfwGetKeyScancode(GLFW_KEY_RIGHT_ALT));
    addButton("key_rsuper", glfwGetKeyScancode(GLFW_KEY_RIGHT_SUPER));
    addButton("key_menu", glfwGetKeyScancode(GLFW_KEY_MENU));
}
