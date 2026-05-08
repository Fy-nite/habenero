#pragma once
#include <raylib.h>
#include <cstring>

// ─── Fixed-size ASCII buffer (stb_textedit's "string" type) ─────────────────
struct StbTextBuf {
    static constexpr int MAX = 256;
    char chars[MAX] = {};
    int  len        = 0;

    const char* c_str() const { return chars; }

    void set(const char* s) {
        len = 0;
        if (s) while (*s && len < MAX - 1) chars[len++] = *s++;
        chars[len] = '\0';
    }
};

// Pull in STB_TexteditState struct (header mode — no implementation compiled here).
// The macros are set here so every TU that includes this header uses the same types.
#ifndef STB_TEXTEDIT_CHARTYPE
#  define STB_TEXTEDIT_CHARTYPE     char
#  define STB_TEXTEDIT_POSITIONTYPE int
#endif
#include <stb_textedit.h>

// ─── C++ wrapper ─────────────────────────────────────────────────────────────
class StbTextField {
public:
    StbTextBuf        buf;
    STB_TexteditState state{};

    explicit StbTextField(const char* initial = "");

    const char* text() const { return buf.c_str(); }
    int         len()  const { return buf.len; }

    void setText(const char* s);

    // Call once per frame while this field has keyboard focus.
    void UpdateInput();

    // Draw the field and handle mouse clicks/drag.
    // Returns true if the mouse clicked inside this frame (caller sets focus).
    bool Draw(Rectangle r, bool focused,
              int   fontSize    = 20,
              Color textCol     = WHITE,
              Color bg          = {20, 18, 30, 255},
              Color border      = {75, 65, 100, 255},
              Color focusBorder = {220, 75, 110, 255});
};
