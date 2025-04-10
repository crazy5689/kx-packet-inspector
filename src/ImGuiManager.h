#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class ImGuiManager {
public:
    static bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd);
    static void NewFrame();
    static void Render(ID3D11DeviceContext* context, ID3D11RenderTargetView* mainRenderTargetView);
    static void RenderUI();
    static void Shutdown();
private:
    static void RenderPacketInspectorWindow(); // Main window function
    // Helper functions for RenderPacketInspectorWindow sections
    static void RenderHints();
    static void RenderInfoSection();
    static void RenderStatusControlsSection();
    static void RenderFilteringSection();
    static void RenderPacketLogSection();
};
