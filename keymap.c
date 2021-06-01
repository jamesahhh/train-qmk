#include QMK_KEYBOARD_H

typedef struct {
  bool is_press_action;
  int state;
} tap;

enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2,
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  TRIPLE_TAP = 5,
  TRIPLE_HOLD = 6
};

enum {
  ALT_OSL1 = 0,
  UP_VOLU= 1,
  DOWN_VOLD = 2,
  LEFT_MPRV = 3,
  RIGHT_MNXT = 4,
};

int cur_dance (qk_tap_dance_state_t *state);
void alt_finished (qk_tap_dance_state_t *state, void *user_data);
void alt_reset (qk_tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT_directional(
        KC_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_NO, KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_LCTRL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSPO, KC_Z, KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMMA,    KC_DOT, KC_SLASH,  KC_RSPC, TD(UP_VOLU), KC_RCTRL,
        KC_CAPSLOCK, KC_LGUI, TD(ALT_OSL1),                   KC_SPC,  KC_SPC,  KC_SPC,           KC_RALT, MO(1), TD(LEFT_MPRV), TD(DOWN_VOLD),   TD(RIGHT_MNXT)
    ),

    LAYOUT_directional(
        _______,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, RESET,
        _______, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______, _______, KC_NO,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_GRAVE,          _______,
        _______, _______, _______, _______, BL_DEC,  BL_TOGG, BL_INC,  BL_STEP, _______, _______, _______, KC_INSERT, KC_PGUP, KC_DELETE,
        _______, _______, _______,                   _______, _______, _______,          _______, _______, KC_HOME, KC_PGDN, KC_END
    )
};

int cur_dance (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->pressed) return SINGLE_HOLD;
    else return SINGLE_TAP;
  }
  else if (state->count == 2) {
    if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  else if (state->count == 3) {
    if (state->interrupted || !state->pressed)  return TRIPLE_TAP;
    else return TRIPLE_HOLD;
  }
  else return 8;
}

static tap alttap_state = {
  .is_press_action = true,
  .state = 0
};

void alt_finished (qk_tap_dance_state_t *state, void *user_data) {
  alttap_state.state = cur_dance(state);
  switch (alttap_state.state) {
    case SINGLE_TAP: set_oneshot_layer(1, ONESHOT_START); clear_oneshot_layer_state(ONESHOT_PRESSED); break;
    case SINGLE_HOLD: register_code(KC_LALT); break;
    case DOUBLE_TAP: set_oneshot_layer(1, ONESHOT_START); set_oneshot_layer(1, ONESHOT_PRESSED); break;
    case DOUBLE_HOLD: register_code(KC_LALT); layer_on(1); break;
    //Last case is for fast typing. Assuming your key is `f`:
    //For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
    //In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
  }
}

void alt_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (alttap_state.state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_LALT); break;
    case DOUBLE_TAP: break;
    case DOUBLE_HOLD: layer_off(1); unregister_code(KC_LALT); break;
  }
  alttap_state.state = 0;
}

qk_tap_dance_action_t tap_dance_actions[] = {
  [ALT_OSL1]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,alt_finished, alt_reset),
  [UP_VOLU] = ACTION_TAP_DANCE_DOUBLE(KC_UP, KC_VOLU),
  [DOWN_VOLD] = ACTION_TAP_DANCE_DOUBLE(KC_DOWN, KC_VOLD),
  [LEFT_MPRV] = ACTION_TAP_DANCE_DOUBLE(KC_LEFT, KC_MPRV),
  [RIGHT_MNXT] = ACTION_TAP_DANCE_DOUBLE(KC_RIGHT, KC_MNXT),
};
