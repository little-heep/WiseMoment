#include "AcrylicBlur_win.h"

#ifdef _WIN32
#  include <windows.h>

// Minimal Acrylic/Blur helper using SetWindowCompositionAttribute (undocumented but widely used).
// If it fails, we simply keep translucent background without OS blur.

enum ACCENT_STATE
{
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5
};

struct ACCENT_POLICY
{
    int AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA
{
    int Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

static int argb(int a, int r, int g, int b)
{
    return (a << 24) | (b << 16) | (g << 8) | r; // ABGR in GradientColor
}

bool AcrylicBlurWin::enable(QWidget* w)
{
    if (!w)
        return false;

    const HWND hwnd = reinterpret_cast<HWND>(w->winId());
    if (!hwnd)
        return false;

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32)
        return false;

    auto fn = reinterpret_cast<SetWindowCompositionAttribute_t>(GetProcAddress(user32, "SetWindowCompositionAttribute"));
    if (!fn)
        return false;

    ACCENT_POLICY policy;
    policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
    policy.AccentFlags = 2; // draw all borders
    policy.GradientColor = argb(180, 14, 18, 24); // tint (dark glass)
    policy.AnimationId = 0;

    WINDOWCOMPOSITIONATTRIBDATA data;
    data.Attrib = 19; // WCA_ACCENT_POLICY
    data.pvData = &policy;
    data.cbData = sizeof(policy);

    return fn(hwnd, &data) == TRUE;
}

#else
bool AcrylicBlurWin::enable(QWidget*) { return false; }
#endif

