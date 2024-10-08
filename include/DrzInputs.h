#pragma once

namespace drz::inputs {

  ///input key enum
  enum Key {
    NONE,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    UP, DOWN, LEFT, RIGHT,
    SPACE, TAB, SHIFT, CTRL, ALT, INS, DEL, HOME, END, PGUP, PGDN,
    BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
    NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
    NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD,
    EQUALS, COMMA, MINUS,
    OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8,
    CAPS_LOCK, ENUM_END
  };

  ///struct to hold the state of a hardware button
  struct HardwareButton {
    bool isPressed;
    bool isReleased;
    bool isHeld;
  };

} // namespace drz::inputs


namespace drz {

using namespace inputs;

/***
 * @brief IDrzInputs: interface for DrzInputs platform libraries
 */
class IDrzInputs {
  public:
    virtual HardwareButton GetKey(Key key) = 0;
};

/***
 * @brief DrzInputs is the main static inputs class to acces IDrzInputs implementations
*/
class DrzInputs {
  public:
    static IDrzInputs* Get();
    static void Set(IDrzInputs* inputs);
  private:
    inline static IDrzInputs* instance = nullptr;
};

} // namespace drz