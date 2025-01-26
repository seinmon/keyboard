/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_POINTER,
    LAYER_NAVIGATION,
    LAYER_MEDIA,
    LAYER_NUMERAL,
    LAYER_SYMBOLS,
    LAYER_FUNCTION,
};

// Automatically enable sniping-mode on the pointer layer.
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#define _L_PTR(KC) LT(LAYER_POINTER, KC)
#define ENT_NAV LT(LAYER_NAVIGATION, KC_ENT)
#define TAB_MED LT(LAYER_MEDIA, KC_TAB)
#define BSP_NUM LT(LAYER_NUMERAL, KC_BSPC)
#define SPC_SYM LT(LAYER_SYMBOLS, KC_SPC)
#define ESC_FUN LT(LAYER_FUNCTION, KC_ESC)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

// clang-format off
/** \brief QWERTY layout (3 rows, 10 columns). */
#define LAYOUT_LAYER_BASE                                                                     \
       KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, \
       KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, \
       KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, \
                      ESC_FUN, SPC_SYM, BSP_NUM, TAB_MED, ENT_NAV

/** Convenience row shorthands. */
#define _______________DEAD_HALF_ROW_______________ XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
#define ______________HOME_ROW_SCAG_L______________ KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI, XXXXXXX
#define ______________HOME_ROW_SCAG_R______________ XXXXXXX, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT

#define LAYOUT_LAYER_FUNCTION                                                                  \
    _______________DEAD_HALF_ROW_______________, _______________DEAD_HALF_ROW_______________,  \
    KC_F1,   KC_F2,   KC_F3,    KC_F4,    KC_F5, KC_F6,   KC_F7,   KC_F8,   KC_F9,    KC_F10,  \
    XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, KC_F11, KC_F12,  XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, \
                        _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX

#define LAYOUT_LAYER_MEDIA                                                                      \
    _______________DEAD_HALF_ROW_______________, _______________DEAD_HALF_ROW_______________,   \
    XXXXXXX, KC_VOLD, KC_MUTE, KC_VOLU, XXXXXXX, XXXXXXX,  KC_VOLD, KC_MUTE, KC_VOLU, XXXXXXX,  \
    _______________DEAD_HALF_ROW_______________, XXXXXXX,  KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX,  \
                      KC_MPRV, KC_MPLY, KC_MNXT, _______, XXXXXXX

#define LAYOUT_LAYER_POINTER                                                                     \
    DPI_MOD, S_D_MOD, XXXXXXX, QK_BOOT, EE_CLR,    EE_CLR, QK_BOOT, XXXXXXX, S_D_MOD, DPI_MOD, \
    ______________HOME_ROW_SCAG_L______________,    ______________HOME_ROW_SCAG_R______________,   \
    XXXXXXX, SNIPING, DRGSCRL, _______, KC_BTN2,   KC_BTN3, _______, DRGSCRL, SNIPING, XXXXXXX,  \
                      KC_BTN4, KC_BTN1, KC_BTN5,   KC_BTN1, KC_BTN2

#define LAYOUT_LAYER_NAVIGATION                                                                 \
    _______________DEAD_HALF_ROW_______________,   KC_DOWN, KC_RGHT, XXXXXXX, XXXXXXX, XXXXXXX, \
    ______________HOME_ROW_SCAG_L______________,   KC_LEFT, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, \
    _______________DEAD_HALF_ROW_______________,   KC_UP,   KC_HOME, KC_PGDN, KC_PGUP,  KC_END, \
                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______

#define LAYOUT_LAYER_SYMBOLS                                                                     \
     XXXXXXX, XXXXXXX, XXXXXXX, KC_QUOT, KC_DQT,    KC_LCBR, KC_AMPR, KC_ASTR, KC_UNDS, KC_RCBR, \
     ______________HOME_ROW_SCAG_L______________,        KC_LBRC, KC_DLR,  KC_PERC, KC_CIRC, KC_RBRC, \
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, LSFT(KC_GRV),   KC_LPRN, KC_EXLM, KC_AT,   KC_HASH, KC_RPRN, \
                      XXXXXXX, XXXXXXX,  _______, KC_SLSH, KC_BSLS

#define LAYOUT_LAYER_NUMERAL                                                                       \
     _______________DEAD_HALF_ROW_______________,    KC_GRV,   KC_7,    KC_8,    KC_9,   KC_MINS,  \
     ______________HOME_ROW_SCAG_L______________,    KC_QUOT,  KC_4,    KC_5,    KC_6,   KC_PLUS,  \
     _______________DEAD_HALF_ROW_______________,    KC_SLSH,  KC_1,    KC_2,    KC_3,   KC_EQL,   \
                       XXXXXXX, _______, XXXXXXX, KC_DOT, KC_0

/**
 * \brief Add Home Row mod to a layout.
 *
 * Expects a 10-key per row layout.  Adds support for SCAG (Shift, Ctl, Alt, Gui)
 * home row. The layout passed in parameter must contain at least 20 keycodes.
 *
 * This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
 *
 *     MOD_SCAG(LAYER_ALPHAS_QWERTY)
 */
#define _MOD_SCAG(                                                              \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                           \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                           \
    L20, L21, L22, L23, L24, R25, R26, R27, R28, R29,                           \
    ...)                                                                        \
             L00,         L01,         L02,          L03,         L04,          \
             R05,         R06,         R07,          R08,         R09,          \
             LSFT_T(L10), LCTL_T(L11), LALT_T(L12),  LGUI_T(L13), L14,          \
             R15,  RGUI_T(R16), RALT_T(R17), LCTL_T(R18),         RSFT_T(R19),  \
             L20,         L21,         L22,          L23,         L24,          \
             R25,         R26,         R27,          R28,         R29,          \
      __VA_ARGS__

#define MOD_SCAG(...) _MOD_SCAG(__VA_ARGS__)

/**
 * \brief Add pointer layer keys to a layout.
 *
 * Expects a 10-key per row layout.  The layout passed in parameter must contain
 * at least 30 keycodes.
 *
 * This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
 *
 *     POINTER_MOD(LAYER_ALPHAS_QWERTY)
 */
#define _POINTER_MOD(                                                  \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                  \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                  \
    L20, L21, L22, L23, L24, R25, R26, R27, R28, R29,                  \
    ...)                                                               \
             L00,         L01,         L02,         L03,         L04,  \
             R05,         R06,         R07,         R08,         R09,  \
             L10,         L11,         L12,         L13,         L14,  \
             R15,         R16,         R17,         R18,         R19,  \
             L20,         L21,         L22, _L_PTR(L23),         L24,  \
             R25, _L_PTR(R26),         R27,         R28,         R29,  \
      __VA_ARGS__

#define POINTER_MOD(...) _POINTER_MOD(__VA_ARGS__)

#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_wrapper(
    POINTER_MOD(MOD_SCAG(LAYOUT_LAYER_BASE))
  ),
  [LAYER_FUNCTION] = LAYOUT_wrapper(LAYOUT_LAYER_FUNCTION),
  [LAYER_NAVIGATION] = LAYOUT_wrapper(LAYOUT_LAYER_NAVIGATION),
  [LAYER_MEDIA] = LAYOUT_wrapper(LAYOUT_LAYER_MEDIA),
  [LAYER_NUMERAL] = LAYOUT_wrapper(LAYOUT_LAYER_NUMERAL),
  [LAYER_POINTER] = LAYOUT_wrapper(LAYOUT_LAYER_POINTER),
  [LAYER_SYMBOLS] = LAYOUT_wrapper(LAYOUT_LAYER_SYMBOLS),
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in
// rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif
