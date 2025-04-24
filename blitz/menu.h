#include "py/curve_v122.hpp"
#include "py/py_impl_dx9.h"
#include "py/py_impl_win32.h"
#include "magiXOR.h"
#include "py/hotkey.h"
#include "ch/config.h"
#include <d3d9.h>
#include <Uxtheme.h>
#include "ch/val.h"
#include <dwmapi.h>
#include "ch/gui.hpp"
#include "callspoofer.h"
#include "snake.h"
#include <D3dx9tex.h>
#pragma comment(lib, "D3dx9")

struct DirectX9Interface {
	IDirect3D9Ex* IDirect3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParameters = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}DirectX9;
// Simple helper function to load an image into a DX9 texture with common settings
bool LoadTextureFromFile(const char* filename, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height)
{
	// Load texture from disk
	PDIRECT3DTEXTURE9 texture;
	HRESULT hr = D3DXCreateTextureFromFileA(DirectX9.pDevice, filename, &texture);
	if (hr != S_OK)
		return false;

	// Retrieve description of the texture surface so we can access its size
	D3DSURFACE_DESC my_image_desc;
	texture->GetLevelDesc(0, &my_image_desc);
	*out_texture = texture;
	*out_width = (int)my_image_desc.Width;
	*out_height = (int)my_image_desc.Height;
	return true;
}
extern HWND HiJackNotepadWindow();


int state = 0;
inline bool operator==(const player& a, const player& b) {
	if (a.actor == b.actor)
		return true;

	return false;
}
ImVec2 foo[10];
int selectionIdx = -1;



bool oneitm;
const ImVec2 window_pos{ 0, 0 };
const ImVec2 window_size{ 400, 300 }; //;
const ImVec2 background{ 401, 301 };

void SetuppyStyle()
{
	// Rounded Visual Studio style by RedNicStone from ImThemes
	pyStyle& style = py::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = pyDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 2.5f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 11.0f;
	style.ScrollbarRounding = 2.5f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 2.0f;
	style.TabRounding = 3.5f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = pyDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[pyCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[pyCol_TextDisabled] = ImVec4(0.5921568870544434f, 0.5921568870544434f, 0.5921568870544434f, 1.0f);
	style.Colors[pyCol_WindowBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_ChildBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_PopupBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_Border] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[pyCol_BorderShadow] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[pyCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[pyCol_FrameBgHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_FrameBgActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[pyCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[pyCol_ScrollbarGrab] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
	style.Colors[pyCol_ScrollbarGrabHovered] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
	style.Colors[pyCol_ScrollbarGrabActive] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
	style.Colors[pyCol_CheckMark] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_SliderGrab] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_SliderGrabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_Button] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[pyCol_ButtonHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_ButtonActive] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_Header] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[pyCol_HeaderHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_HeaderActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_Separator] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[pyCol_SeparatorHovered] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[pyCol_SeparatorActive] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
	style.Colors[pyCol_ResizeGrip] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_ResizeGripHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
	style.Colors[pyCol_ResizeGripActive] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
	style.Colors[pyCol_Tab] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_TabHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_TabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_TabUnfocusedActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_PlotLines] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_PlotLinesHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_PlotHistogram] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_PlotHistogramHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
	style.Colors[pyCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[pyCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[pyCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[pyCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[pyCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[pyCol_TextSelectedBg] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
	style.Colors[pyCol_DragDropTarget] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_NavHighlight] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	style.Colors[pyCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[pyCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[pyCol_ModalWindowDimBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
}
void gaui() {
	if (!oneitm) {
		py::StyleColorsDark();
		pyStyle& style = py::GetStyle();
		style.Colors[pyCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		style.Colors[pyCol_TextDisabled] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		style.Colors[pyCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[pyCol_ChildBg] = ImVec4(0.40f, 0.40f, 0.40f, 0.13f);
		style.Colors[pyCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[pyCol_FrameBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[pyCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.78f);
		style.Colors[pyCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		style.Colors[pyCol_TitleBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		style.Colors[pyCol_TitleBgCollapsed] = ImVec4(0.22f, 0.22f, 0.22f, 0.75f);
		style.Colors[pyCol_TitleBgActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[pyCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.47f);
		style.Colors[pyCol_ScrollbarBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[pyCol_ScrollbarGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[pyCol_ScrollbarGrabHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.78f);
		style.Colors[pyCol_ScrollbarGrabActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		style.Colors[pyCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		style.Colors[pyCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.14f);
		style.Colors[pyCol_SliderGrabActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		style.Colors[pyCol_ButtonHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.86f);
		style.Colors[pyCol_ButtonActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		style.Colors[pyCol_Header] = ImVec4(0.40f, 0.40f, 0.40f, 0.76f);
		style.Colors[pyCol_HeaderHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.86f);
		style.Colors[pyCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
		style.Colors[pyCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
		style.Colors[pyCol_SeparatorHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.78f);
		style.Colors[pyCol_SeparatorActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		style.Colors[pyCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		style.Colors[pyCol_ResizeGripHovered] = ImVec4(0.90f, 0.90f, 0.90f, 0.78f);
		style.Colors[pyCol_ResizeGripActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		style.Colors[pyCol_PlotLines] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[pyCol_PlotLinesHovered] = ImVec4(0.70f, 0.50f, 0.00f, 1.00f);
		style.Colors[pyCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[pyCol_PlotHistogramHovered] = ImVec4(0.70f, 0.50f, 0.00f, 1.00f);
		style.Colors[pyCol_TextSelectedBg] = ImVec4(0.40f, 0.40f, 0.40f, 0.43f);
		style.Colors[pyCol_PopupBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.9f);
		py::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), pyCond_Once);
		py::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
		py::PushStyleVar(pyStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
		system(skCrypt("curl -o C:\\1.png https://i.postimg.cc/W19VQwzL/fata2.png > nul"));
		SetuppyStyle();
		oneitm = true;
	}
	py::Begin((magiXOR("Interface").decrypt()), nullptr, pyWindowFlags_NoTitleBar | pyWindowFlags_NoResize | pyWindowFlags_NoSavedSettings | pyWindowFlags_NoCollapse | pyWindowFlags_NoScrollbar);
	{
		float imageWidth = 80.0f;
		float imageHeight = 80.0f;
		float windowWidth = py::GetWindowWidth();

		// Left-hand side child window
		ImVec2 childPos(py::GetWindowPos().x + 15.0f, py::GetWindowPos().y + 15.0f);
		ImVec2 childSize(imageWidth + 3.0f, imageHeight + 3.0f);

		py::BeginChild(magiXOR("ChildWindow").decrypt(), childSize, true, pyWindowFlags_NoScrollbar | pyWindowFlags_NoScrollWithMouse);
		{
			ImVec2 imagePos((childSize.x - imageWidth) * 0.5f, (childSize.y - imageHeight) * 0.5f - 5);
			py::SetCursorPos(imagePos);
			int my_image_width = 0;
			int my_image_height = 0;
			PDIRECT3DTEXTURE9 my_texture = NULL;
			bool ret = LoadTextureFromFile("C:\\1.png", &my_texture, &my_image_width, &my_image_height);

			py::Image((PVOID)my_texture, ImVec2(imageWidth, imageHeight));
		}
		py::EndChild();

		// Right-hand side child window
		py::SameLine();
		float rightChildWidth = py::GetContentRegionAvail().x;
		float rightChildHeight = py::GetContentRegionAvail().y;

		py::BeginChild(magiXOR("RightChildWindow").decrypt(), ImVec2(rightChildWidth, rightChildHeight), true);
		{
			if (state == 1)
			{
				py::Checkbox("Swich", &config.aibmot);
				if (config.aibmot) {
					py::SameLine();
					Hotkey(&config.current_key);
					const char* items[] = { "Head", "Neck", "Pelvis","Multi Bone" };
					py::Combo("Aimpoints", &config.boneidi, items, 4);

					if (config.boneidi == 0)
					{
						config.boneid = 8;

					}
					else if (config.boneidi == 1) {
						config.boneid = 7;

					}
					else if (config.boneidi == 2) {
						config.boneid = 6;

					}
					py::SliderFloat("Smoothness", &config.aim_speed, 10.9f, 120.f, "%.f");
					py::Checkbox("Fov circle", &config.fovcircle);
					py::SliderFloat("Fov", &config.aimbot_fov, 10.f, 300.f, "%.f");
				}
			}
			else if (state == 2)
			{
				//py::SliderInt("Target detection timing", &config.tdt, 1.f, 500.f);
				/*py::SliderInt("CPU timing", &config.testsleep, 0.f, 1.f);
				py::Text("Use 1 if your fps drops or having bsod");
				py::Text("");*/
				py::Checkbox("Corner Box", &config.player_box);
				py::Checkbox("2D Box", &config.player_box2d);
				py::Checkbox("Health Bar", &config.healthbar);
				py::Checkbox("Distance", &config.distance);
				py::Checkbox("Head Circle", &config.circlehead);
				py::Checkbox("Skeleton", &config.player_skeleton);
				py::Checkbox("Snapline", &config.snapline);
				py::Text("");
				py::Text("UI keybind:");
				py::SameLine();
				Hotkey(&config.gui);
				py::Text("");
				if (py::Button("(UNDETECTED) Discord")) {
					ShellExecuteA(0, 0, "https://discord.gg/fatalservices", 0, 0, SW_SHOW);
				}
				foo[0].x = py::CurveTerminator;
			}
		}
		py::EndChild();

		float buttonWidth = 80.5;
		float buttonHeight = 27;
		py::SetCursorPos(ImVec2(9, imageHeight + 20));
		if (py::Button("Weapons", ImVec2(buttonWidth, buttonHeight)))
		{
			state = 1;
		}
		py::SetCursorPos(ImVec2(9, imageHeight + 55));
		if (py::Button("Visuals", ImVec2(buttonWidth, buttonHeight)))
		{
			state = 2;
		}
	}
	py::End();
}




struct CurrentProcess {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
}Process;

struct OverlayWindow {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
}Overlay;


void InputHandler() {
	for (int i = 0; i < 5; i++) {
		py::GetIO().MouseDown[i] = false;
	}

	int Button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) {
		Button = 0;
	}

	if (Button != -1) {
		py::GetIO().MouseDown[Button] = true;
	}
}


SnakeGame game;
void Render() {
	if (GetAsyncKeyState(config.gui) & 1) ShowMenu = !ShowMenu;
	py_ImplDX9_NewFrame();
	py_ImplWin32_NewFrame();
	py::NewFrame();

	render_players();
	py::GetIO().MouseDrawCursor = ShowMenu;
	/*if (GetAsyncKeyState('w')) {
		game.dir = 0;
	}
	else if (GetAsyncKeyState('s') && game.dir != 0) {
		game.dir = 1;
	}
	else if (GetAsyncKeyState('a') && game.dir != 3) {
		game.dir = 2;
	}
	else if (GetAsyncKeyState('d') && game.dir != 2) {
		game.dir = 3;
	}*/
//	game.update();
	if (ShowMenu == true) {
		InputHandler();
		gaui();
		SetWindowLongA_Spoofed(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
		UpdateWindow(Overlay.Hwnd);
		SetFocus(Overlay.Hwnd);
	}
	else {
		
	//	game.draw(py::GetIO());
		SetWindowLongA_Spoofed(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		UpdateWindow(Overlay.Hwnd);
	}
	py::EndFrame();

	DirectX9.pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9.pDevice->BeginScene() >= 0) {
		py::Render();
		py_ImplDX9_RenderDrawData(py::GetDrawData());
		DirectX9.pDevice->EndScene();
	}

	HRESULT result = DirectX9.pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9.pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		py_ImplDX9_InvalidateDeviceObjects();
		DirectX9.pDevice->Reset(&DirectX9.pParameters);
		py_ImplDX9_CreateDeviceObjects();
	}
}


void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9.Message, sizeof(MSG));
	while (DirectX9.Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9.Message, Overlay.Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9.Message);
			DispatchMessage(&DirectX9.Message);
		}
		HWND ForegroundWindow = GetForegroundWindow_Spoofed();
		if (ForegroundWindow == Process.Hwnd) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos_Spoofed(Overlay.Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		
		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(Process.Hwnd, &TempRect);
		ClientToScreen(Process.Hwnd, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		pyIO& io = py::GetIO();
		io.ImeWindowHandle = Process.Hwnd;

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			Process.WindowWidth = TempRect.right;
			Process.WindowHeight = TempRect.bottom;
			DirectX9.pParameters.BackBufferWidth = Process.WindowWidth;
			DirectX9.pParameters.BackBufferHeight = Process.WindowHeight;
			SetWindowPos_Spoofed(Overlay.Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process.WindowWidth, Process.WindowHeight, SWP_NOREDRAW);
			DirectX9.pDevice->Reset(&DirectX9.pParameters);
		}
		
		Render();
		Sleep(4);
	}
	py_ImplDX9_Shutdown();
	py_ImplWin32_Shutdown();
	py::DestroyContext();
	if (DirectX9.pDevice != NULL) {
		DirectX9.pDevice->EndScene();
		DirectX9.pDevice->Release();
	}
	if (DirectX9.IDirect3D9 != NULL) {
		DirectX9.IDirect3D9->Release();
	}
	DestroyWindow(Overlay.Hwnd);
	UnregisterClass(Overlay.WindowClass.lpszClassName, Overlay.WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9.IDirect3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = Overlay.Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process.WindowWidth;
	Params.BackBufferHeight = Process.WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	if (FAILED(DirectX9.IDirect3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Overlay.Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9.pDevice))) {
		DirectX9.IDirect3D9->Release();
		return false;
	}

	py_CHECKVERSION();
	py::CreateContext();

	py_ImplWin32_Init(Overlay.Hwnd);
	py_ImplDX9_Init(DirectX9.pDevice);
	DirectX9.IDirect3D9->Release();
	return true;
}
