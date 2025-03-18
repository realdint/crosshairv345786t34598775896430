#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <string>
#include <sstream>
#include <vector>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "gdiplus.lib")

// g++ -g main.cpp -lgdiplus -ldwmapi -o Main.exe -mwindows
// Main.exe 35

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

std::vector<std::wstring> cornerTexts;

int ParseRadius(const std::string& cmdLine) {
    std::istringstream iss(cmdLine);
    std::string token;
    int radius = 350; // default radius

    while (iss >> token) {
        if (token == "-radius") {
            if (iss >> token) {
                try {
                    radius = std::stoi(token);
                } catch (std::invalid_argument& e) {
                    // handle error if needed
                }
            }
        }
    }
    return radius;
}

void AddTextToCorner(const std::wstring& text) {
    cornerTexts.push_back(text);
}

// void hacks cheat AddCrossHairToGame(const roblox::hacks)
// {
//     Roblox.injecthacksmainframe (hacks);
//     Roblox.aimbot()
//     roblox.hack()
//     roblox.alterUI()
//     roblox.ruinMarineAcademysUI()
//     roblox.ddos()
//     roblox.addAdvantages()
// }

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // radius
    int radius = ParseRadius(lpCmdLine);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    const wchar_t CLASS_NAME[] = L"OverlayWindowClass";

    WNDCLASSW wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        CLASS_NAME,
        NULL,
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    ShowWindow(hwnd, nCmdShow);

    SetWindowLongPtr(hwnd, GWLP_USERDATA, radius);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

void DrawTextToCorner(Gdiplus::Graphics& graphics, const Gdiplus::Font& font, const Gdiplus::Brush& brush) {
    Gdiplus::RectF layoutRect;
    Gdiplus::RectF boundRect;
    Gdiplus::StringFormat format;

    float yOffset = 0;
    for (const auto& text : cornerTexts) {
        graphics.MeasureString(text.c_str(), -1, &font, layoutRect, &boundRect);
        float textWidth = boundRect.Width;
        graphics.DrawString(text.c_str(), -1, &font, Gdiplus::PointF(GetSystemMetrics(SM_CXSCREEN) - textWidth - 1, yOffset), &brush);
        yOffset += boundRect.Height + 1;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            Gdiplus::Graphics graphics(hdc);
            Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0), 2);
            Gdiplus::Font font(L"Arial", 16);
            Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));

            int centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
            int centerY = GetSystemMetrics(SM_CYSCREEN) / 2;

            int radius = static_cast<int>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

            graphics.DrawEllipse(&pen, centerX - radius, centerY - radius, radius * 2, radius * 2);

            //std::wstring radiusText = L"Aimbot radius: " + std::to_wstring(radius);

            Gdiplus::RectF layoutRect;
            Gdiplus::RectF boundRect;
            Gdiplus::StringFormat format;
            //graphics.MeasureString(radiusText.c_str(), -1, &font, layoutRect, &boundRect);

            float textWidth = boundRect.Width;

            //graphics.DrawString(radiusText.c_str(), -1, &font, Gdiplus::PointF(centerX - textWidth / 2, centerY - radius - boundRect.Height - 10), &brush);

            AddTextToCorner(L"Aimbot radius: " + std::to_wstring(radius));
            AddTextToCorner(L"Silent aim");
            AddTextToCorner(L"Lagswitch");
            AddTextToCorner(L"dint's sprintbot");

            DrawTextToCorner(graphics, font, brush);

            EndPaint(hwnd, &ps);
        } break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
