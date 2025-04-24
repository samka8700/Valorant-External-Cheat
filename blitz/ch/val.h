#include <vector>
#include <Windows.h>
#include <string>

#include "drv.h"
#include "shit.h"	
#include "config.h"
#include <d3d9.h>
#include "../py/py.h"
#include "string.h"
#include <chrono>
#include <unordered_map>
#include <random>
#include <ctime>
#include <cstdlib>
#include <cmath>



#define UWORLD 0x60
#define GAME_INSTANCE 0x1A0
#define LOCALPLAYERS_ARRAY 0x40
#define PLAYER_CONTROLER 0x38
#define ROOT_COMPONENT 0x238
#define PERSISTENT_LEVEL 0x38
#define LOCALPLAYERPAWN 0x448
#define ROOT_POS 0x164
#define DAMAGE_HANDLER 0x9E8 //0xA08
#define CONTROL_ROT 0x448
#define CAMERA_MANAGER 0x460
#define ACTOR_ARRAY 0xA0
#define ACTOR_COUNT 0xB8
#define UNIQUE_ID 0x38
#define TEAM_COMP 0x610
#define TEAM_ID 0xf8
#define HEALTH 0x1B0
#define DORMANT 0x100
#define PLAYER_STATE 0x3D8
#define MESH 0x418
#define COMPONENT 0x250
#define BONE_ARRAY 0x5D8
#define BONE_COUNT 0x5E0
#define BONE_ARRAY_CACHED 0x5C0 + 0x10
#define LAST_SUMBIT_TIME 0x380
#define LAST_RENDER_TIME 0x384
#define attach_children 0x110
#define attach_children_count 0x118
#define outline_component 0x1208
#define fresnel_component 0x8a8
#define fresnel_glow 0x1c4
#define fresnel_dirty 0xf0
#define outline_mode 0x330
#define AllyOutline 0x2b0
#define EnemyOutline 0x2b1
#define NeutralOutline 0x2b2
#define DeadAllyOutline 0x2b3
#define DeadEnemyOutline 0x2b4
#define CAMERA_POS 0x1260
#define CAMERA_ROT 0x126C
#define CAMERA_FOV 0x1278
#define FOV_CHANGE 0x3f4
#define portrait_minimap_component 0x1198
#define character_minimap_component 0x11a0
#define local_observer 0x530
#define local_visible 0x501



using namespace std;

#include "defs.h"
#include "Overlay.h"

int center_xa = GetSystemMetrics(0) / 2;
int center_ya = GetSystemMetrics(1) / 2;
//ue4 fvector class
float Moruk = 0;


class fvector {
public:
	fvector() : x(0.f), y(0.f), z(0.f) {}
	fvector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	~fvector() {}

	float x;
	float y;
	float z;

	inline float Dot(fvector v) {
		return x * v.x + y * v.y + z * v.z;
	}

	inline float distance(fvector v) {
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	fvector operator+(fvector v) {
		return fvector(x + v.x, y + v.y, z + v.z);
	}

	fvector operator-(fvector v) {
		return fvector(x - v.x, y - v.y, z - v.z);
	}
	fvector operator/(float flDiv) {
		return fvector(x / flDiv, y / flDiv, z / flDiv);
	}

	fvector operator*(float Scale) {

		return fvector(x * Scale, y * Scale, z * Scale);
	}
	inline float Length() {
		return sqrtf((x * x) + (y * y) + (z * z));
	}
	fvector operator-=(fvector v) {

		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
};


struct FVector2D
{
public:
	float x;
	float y;

	inline FVector2D() : x(0), y(0) {}
	inline FVector2D(float x, float y) : x(x), y(y) {}

	inline float Distance(FVector2D v) {
		return sqrtf(((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y)));
	}

	inline FVector2D operator+(const FVector2D& v) const {
		return FVector2D(x + v.x, y + v.y);
	}

	inline FVector2D operator-(const FVector2D& v) const {
		return FVector2D(x - v.x, y - v.y);
	}

};



struct FMinimalViewInfo {
	struct fvector Location;
	struct fvector Rotation;
	float FOV;
};


//ue4 fquat class
struct fquat {
	float x;
	float y;
	float z;
	float w;
};

//ue4 ftransform class
struct ftransform {
	fquat rot;
	fvector translation;
	char pad[4];
	fvector scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale() {
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
fvector ControlRotation;

namespace guarded_pointers
{
	uintptr_t guard_local_pawn = 0x0;
	uintptr_t guard_controller = 0x0;
}

//maybe we need some pointers everywhere so we just store them here lmao (global)
namespace pointer
{
	uintptr_t local_pawn;
	uintptr_t local_pawn_old;
	uintptr_t player_controller;
	uintptr_t camera_manager;
	fvector camera_position;
}

//we need camera everywhere. lets store it here
namespace camera
{
	fvector location;
	fvector rotation;
	float fov;
}

//hell yeah we want different fonts for everything #COOL-ESP
namespace fonts {
	ImFont* standard_font;
	ImFont* intro_font;
}

//we store the following info for each player so we are cooler
struct player
{
	uintptr_t for_actor;
	uintptr_t for_mesh;

	uintptr_t actor;
	uintptr_t mesh;
	uintptr_t bone_array;
	uintptr_t root_component;
	uintptr_t damage_handler;

	INT32 bone_count;
	INT32 ammo_count;

	std::string weapon_name;
	std::string agent_name;
	std::string player_name;

	float distance;
	float health;
	float shield;

	bool is_valid;
	bool is_damage_handler_guarded;
	bool is_mesh_guarded;
};





std::vector<player> player_pawns;



float Health;

uintptr_t root_comp;

uintptr_t pawn;
uintptr_t baseee;
uintptr_t actor;
int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);


auto isVisible(DWORD_PTR mesh) -> bool
{
	float fLastSubmitTime = read<float>(mesh + 0x380);
	float fLastRenderTimeOnScreen = read<float>(mesh + 0x384);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;

	return bVisible;
}
#define M_PI (double)3.1415926535
namespace math
{
	D3DMATRIX matrix(fvector rot, fvector origin = fvector(0, 0, 0)) {
		float radPitch = (rot.x * float(M_PI) / 180.f);
		float radYaw = (rot.y * float(M_PI) / 180.f);
		float radRoll = (rot.z * float(M_PI) / 180.f);

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
	{
		D3DMATRIX pOut;
		pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
		pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
		pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
		pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
		pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
		pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
		pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
		pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
		pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
		pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
		pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
		pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
		pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
		pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
		pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
		pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

		return pOut;
	}


	

	

}



fvector bone_matrix(int index, player _player)
{
	size_t size = sizeof(ftransform);
	ftransform first_bone, comp_to_world;

	first_bone = read<ftransform>(_player.bone_array + (size * index));
	comp_to_world = read<ftransform>(_player.mesh + COMPONENT);


	D3DMATRIX matrix = math::MatrixMultiplication(first_bone.ToMatrixWithScale(), comp_to_world.ToMatrixWithScale());
	return fvector(matrix._41, matrix._42, matrix._43);
}
//returns true if player is active
bool is_dormant(player _player)
{
	return read<bool>(_player.for_actor + _player.actor + DORMANT);
}



//worldtoscreen what do i say xD
fvector w2s(fvector world_location) {
	fvector ScreenLocation;
	D3DMATRIX tempMatrix = math::matrix(camera::rotation);
	fvector vAxisX, vAxisY, vAxisZ;
	vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	fvector vDelta = world_location - camera::location;
	fvector vTransformed = fvector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.z < .1f)
		vTransformed.z = .1f;
	float ScreenCenterX = center_xa;
	float ScreenCenterY = center_ya;

//	camera::fov = 103;
	
	
	ScreenLocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(camera::fov * (float)M_PI / 360.f)) / vTransformed.z;
	ScreenLocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(camera::fov * (float)M_PI / 360.f)) / vTransformed.z;
	return ScreenLocation;
}


int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
float screenCenterX = screenWidth / 2.0f;
float screenCenterY = screenHeight / 2.0f;

float targetX = 0.0f;
float targetY = 0.0f;

void aimbot(float x, float y, int aimSpeed) {
	float deltaX = x - screenCenterX;
	float deltaY = y - screenCenterY;

	if (deltaX != 0.0f) {
		targetX = deltaX / aimSpeed;
		if (std::abs(targetX) > screenCenterX) targetX = 0.0f;
	}

	if (deltaY != 0.0f) {
		targetY = deltaY / aimSpeed;
		if (std::abs(targetY) > screenCenterY) targetY = 0.0f;
	}

	mem::move(targetX, targetY);
}



ImColor headboxcolor = ImColor(75, 0, 130);
ImColor espcolor = ImColor(255, 255, 255);
ImVec4 snapcolor = { 0.0f, 1.0f, 1.0f, 1.0f }; // snapline renk
ImVec4 skeleton = { 1.0f, 1.0f, 1.0f, 1.0f }; // skeleton renk

void DrawNormalBox(int x, int y, int w, int h, int borderPx, ImVec4 color, int thickness) {
    RGBA rgbaColor = {
        (unsigned char)(255.f * color.x),
        (unsigned char)(255.f * color.y),
        (unsigned char)(255.f * color.z),
        (unsigned char)(255.f * color.w)
    };

    int startX = x + borderPx;
    int startY = y;
    int endX = x + w - borderPx;
    int endY = y + h - borderPx;

    DrawFilledRect(startX, startY, w - borderPx * 2, borderPx, &rgbaColor); // top
    DrawFilledRect(startX, startY + h - borderPx, w - borderPx * 2, borderPx, &rgbaColor); // bottom
    DrawFilledRect(startX, startY, borderPx, h - borderPx * 2, &rgbaColor); // left
    DrawFilledRect(endX, startY, borderPx, h - borderPx * 2, &rgbaColor); // right
}


struct bone_info {
	int id;
	fvector coord;
};

void draw_skeleton(player _player, ImVec4 col, float thickness)
{
	// Define mappings for female, npc, and male bones
	std::vector<bone_info> bone_mapping;

	fvector bone_head = bone_matrix(8, _player);
	fvector bone_chest = bone_matrix(6, _player);
	fvector bone_pelvis = bone_matrix(3, _player);

	if (_player.bone_count == 101) //female
	{
		bone_mapping = {
			{21, {}}, {23, {}}, {24, {}}, {25, {}},
			{49, {}}, {50, {}}, {51, {}}, {75, {}}, {76, {}}, {78, {}},
			{82, {}}, {83, {}}, {85, {}}
		};
	}
	else if (_player.bone_count == 103) //npc
	{
		bone_mapping = {
			{9, {}}, {33, {}}, {30, {}}, {32, {}},
			{58, {}}, {55, {}}, {57, {}}, {63, {}}, {65, {}}, {69, {}},
			{77, {}}, {79, {}}, {83, {}}
		};
	}
	else if (_player.bone_count == 104) //male
	{
		bone_mapping = {
			{21, {}}, {23, {}}, {24, {}}, {25, {}},
			{49, {}}, {50, {}}, {51, {}}, {77, {}}, {78, {}}, {80, {}},
			{84, {}}, {85, {}}, {87, {}}
		};
	}
	else {
		// Handle case where bone mapping is not found
		return;
	}

	for (auto& bone : bone_mapping)
	{
		bone.coord = bone_matrix(bone.id, _player);
	}

	// Perform the w2s transformation after all bones are assigned
	bone_head = w2s(bone_head);
	bone_chest = w2s(bone_chest);
	bone_pelvis = w2s(bone_pelvis);

	for (auto& bone : bone_mapping)
	{
		bone.coord = w2s(bone.coord);
	}

	ImDrawList* draw = py::GetForegroundDrawList();
	ImColor line_color = ImColor(config.player_skeleton_color);

	// Top skeleton lines
	draw->AddLine(ImVec2(bone_head.x, bone_head.y), ImVec2(bone_mapping[0].coord.x, bone_mapping[0].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[0].coord.x, bone_mapping[0].coord.y), ImVec2(bone_chest.x, bone_chest.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_chest.x, bone_chest.y), ImVec2(bone_pelvis.x, bone_pelvis.y), line_color, thickness);

	// Right arm
	draw->AddLine(ImVec2(bone_chest.x, bone_chest.y), ImVec2(bone_mapping[4].coord.x, bone_mapping[4].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[4].coord.x, bone_mapping[4].coord.y), ImVec2(bone_mapping[5].coord.x, bone_mapping[5].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[5].coord.x, bone_mapping[5].coord.y), ImVec2(bone_mapping[6].coord.x, bone_mapping[6].coord.y), line_color, thickness);

	// Left arm
	draw->AddLine(ImVec2(bone_chest.x, bone_chest.y), ImVec2(bone_mapping[1].coord.x, bone_mapping[1].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[1].coord.x, bone_mapping[1].coord.y), ImVec2(bone_mapping[2].coord.x, bone_mapping[2].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[2].coord.x, bone_mapping[2].coord.y), ImVec2(bone_mapping[3].coord.x, bone_mapping[3].coord.y), line_color, thickness);

	// Right leg
	draw->AddLine(ImVec2(bone_pelvis.x, bone_pelvis.y), ImVec2(bone_mapping[9].coord.x, bone_mapping[9].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[9].coord.x, bone_mapping[9].coord.y), ImVec2(bone_mapping[10].coord.x, bone_mapping[10].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[10].coord.x, bone_mapping[10].coord.y), ImVec2(bone_mapping[11].coord.x, bone_mapping[11].coord.y), line_color, thickness);

	// Left leg
	draw->AddLine(ImVec2(bone_pelvis.x, bone_pelvis.y), ImVec2(bone_mapping[7].coord.x, bone_mapping[7].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[7].coord.x, bone_mapping[7].coord.y), ImVec2(bone_mapping[8].coord.x, bone_mapping[8].coord.y), line_color, thickness);
	draw->AddLine(ImVec2(bone_mapping[8].coord.x, bone_mapping[8].coord.y), ImVec2(bone_mapping[9].coord.x, bone_mapping[9].coord.y), line_color, thickness);
}
D3DMATRIX Matrix(fvector rot, fvector origin)
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

fvector update_camera(fvector WorldLocation)
{

	fvector Screenlocation = fvector(0, 0, 0);

	auto ViewInfo = read<FMinimalViewInfo>(pointer::camera_manager + 0x1F90);
	camera::location = ViewInfo.Location;
	camera::rotation = ViewInfo.Rotation;
	camera::fov = ViewInfo.FOV;

	return Screenlocation;

}


float CalculateDistance(int p1x, int p1y, int p2x, int p2y)
{
	float diffY = p1y - p2y;
	float diffX = p1x - p2x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

void draw_corner_box1(int x, int y, int w, int h, ImVec4 Color, int thickness) {
	float lineW = (w / 4.f);
	float lineH = (h / 4.f);
	ImDrawList* p = py::GetForegroundDrawList();

	//oben links nach unten
	p->AddLine(ImVec2(x, y), ImVec2(x, y + lineH), ImColor(Color), thickness);

	//oben links nach rechts (l-mittig)
	p->AddLine(ImVec2(x, y), ImVec2(x + lineW, y), ImColor(Color), thickness);

	//oben rechts (r-mittig) nach rechts
	p->AddLine(ImVec2(x + w - lineW, y), ImVec2(x + w, y), ImColor(Color), thickness);

	//oben rechts nach vert-rechts (oberhalb)
	p->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + lineH), ImColor(Color), thickness);

	//unten vert-links (unterhalb) nach unten links
	p->AddLine(ImVec2(x, y + h - lineH), ImVec2(x, y + h), ImColor(Color), thickness);

	//unten links nach rechts (mittig)
	p->AddLine(ImVec2(x, y + h), ImVec2(x + lineW, y + h), ImColor(Color), thickness);

	//unten rechts (mittig) nach rechts
	p->AddLine(ImVec2(x + w - lineW, y + h), ImVec2(x + w, y + h), ImColor(Color), thickness);

	//unten rechts nach vert-rechts (unterhalb)
	p->AddLine(ImVec2(x + w, y + h - lineH), ImVec2(x + w, y + h), ImColor(Color), thickness);
}
void DrawRightProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x + (w / 2) - 25, y, thick, (h)*m_health / 100, &healthcol);
}


uintptr_t unique_id;

void render_players() {
	
	//Moruk = read<float>(pointer::camera_manager + 0x3f4);

	float x = 100.f; // Replace with your desired value
	float y = 200.f; // Replace with your desired value
	float z = 50.f; // Replace with your desired value

	fvector WorldLocation(x, y, z);

	fvector result = update_camera(WorldLocation);

	static std::unordered_map<int, std::vector<ImVec2>> bone_cache;

	if (config.fovcircle)
	{
		py::GetForegroundDrawList()->AddCircle(ImVec2(py::GetIO().DisplaySize.x / 2, py::GetIO().DisplaySize.y / 2), config.aimbot_fovyes, ImColor(0, 0, 0, 255), 100, 1.0f);
	}
	/*if (config.testfov > 0)
	{
		py::GetForegroundDrawList()->AddCircle(ImVec2(py::GetIO().DisplaySize.x / 2, py::GetIO().DisplaySize.y / 2), config.testfov, ImColor(160, 0, 0, 128), 100, 1.0f);
	}*/

	int closestplayer = 1337;
	float closest_distance = FLT_MAX;
	for (int x = 0; x < player_pawns.size(); x++)
	{
		player& this_player = player_pawns[x];

		float health = 0;
		health = read<float>(this_player.damage_handler + HEALTH);
		
		if (health <= 0.f || health > 999.f)
		{
			player_pawns[x].is_valid = false;
		}
		
		if (!this_player.is_valid)
		{
			auto erase_player = std::find(player_pawns.begin(), player_pawns.end(), this_player);
			player_pawns.erase(erase_player);
			continue;

		}
		
		if (this_player.actor == pointer::local_pawn)
			continue;
		
		fvector zero = bone_matrix(0, this_player);
		fvector head = bone_matrix(8, this_player);

		fvector head1 = bone_matrix(23, this_player);

		fvector bottom = bone_matrix(0, this_player);

		if (!(this_player.bone_count == 101 || this_player.bone_count == 104  || this_player.bone_count == 103) || !zero.Length() || !head.Length())
			continue;
		
		fvector zero_r = fvector(zero.x, zero.y, zero.z - 5);

		fvector vBaseBoneOut = w2s(fvector(zero.x, zero.y, zero.z));
		fvector vBaseBoneOut2 = w2s(fvector(zero.x, zero.y, zero.z - 15));

		fvector vHeadBoneOut = w2s(fvector(head.x, head.y, head.z));

		fvector bottom2 = w2s(fvector(bottom.x, bottom.y, bottom.z));


		fvector vHeadBoneOut2 = w2s(fvector(head.x, head.y, head.z + 15));

		float BoxHeight = abs(vHeadBoneOut2.y - vBaseBoneOut.y);
		float BoxWidth = BoxHeight * 0.55;

		fvector head_r = fvector(head.x, head.y, head.z + 20);
		fvector head_r_2 = fvector(head.x, head.y, head.z + 30);

		fvector zero_w2s = w2s(zero);
		fvector head_w2s = w2s(head);

		fvector zero_w2s_r = w2s(zero_r);
		fvector head_w2s_r = w2s(head_r);

		fvector head_w2s_r_2 = w2s(head_r_2);

		int Width = GetSystemMetrics(SM_CXSCREEN);
		int Height = GetSystemMetrics(SM_CYSCREEN);


		
		float distance = camera::location.distance(zero) / 100.f;

		if (distance > 300)
			continue;
		
		bool active = is_dormant(this_player);
		if (!active)
			continue;
		
		if (isVisible(this_player.mesh)) {

			config.player_box_color = ImColor(136, 255, 0);
		}
		else {
			config.player_box_color = ImColor(255, 0, 0);
		}
		if (config.player_box)
		{
			float BoxHeight = zero_w2s_r.y - head_w2s_r.y;
			float BoxWidth = BoxHeight / 2.f;
			draw_corner_box1(zero_w2s_r.x - (BoxWidth / 2), head_w2s_r.y, BoxWidth, BoxHeight, config.player_box_color, 1.f); // use ImColor instead of ImVec4
		}


		if (config.player_box2d)
		{
			DrawNormalBox(zero_w2s_r.x - BoxWidth / 2 + 1, head_w2s_r.y, BoxWidth, BoxHeight, 1, config.player_box_color, 0.5f);
			DrawNormalBox(zero_w2s_r.x - BoxWidth / 2 - 1, head_w2s_r.y, BoxWidth, BoxHeight, 1, config.player_box_color, 0.5f);
			DrawNormalBox(zero_w2s_r.x - BoxWidth / 2, head_w2s_r.y + 1, BoxWidth, BoxHeight, 1, config.player_box_color, 0.5f);
			DrawNormalBox(zero_w2s_r.x - BoxWidth / 2, head_w2s_r.y - 1, BoxWidth, BoxHeight, 1, config.player_box_color, 0.5f);
			DrawNormalBox(zero_w2s_r.x - (BoxWidth / 2), head_w2s_r.y, BoxWidth, BoxHeight, 1, config.player_box_color, 0.5f);
		}

		if (config.circlehead)
		{
			py::GetForegroundDrawList()->AddCircle(ImVec2(vHeadBoneOut.x, vHeadBoneOut.y), BoxWidth / 8, ImColor(255, 255, 255), 64, 1.f);
		}



		if (config.player_skeleton)
		{

			draw_skeleton(this_player, config.player_skeleton_color, 0.5f);

		}

		/*for (int i = 0; i < 100; i++)
		{
			auto a = bone_matrix(i, this_player);
			auto b = w2s(a);
			char name[64];
			sprintf_s(name, "[%d]", static_cast<int>(i));
			ImVec2 textSize = py::CalcTextSize(name);
			ImVec2 position(b.x - textSize.x / 2, b.y);
			RGBA White = { 255,255,255,255 };
			DrawStrokeText((int)position.x, (int)position.y, &White, name);
		}
	*/

		if (config.distance)
		{
			char name[64];
			sprintf_s(name, "[%dm]", static_cast<int>(distance));
			ImVec2 textSize = py::CalcTextSize(name);
			ImVec2 position(bottom2.x - textSize.x / 3, bottom2.y);
			RGBA White = { 255,255,255,255 };
			DrawStrokeText((int)position.x, (int)position.y, &White, name);
		}



		auto ee = ImColor(255, 255, 255);

		if (config.snapline)
		{
			DrawLine(ImVec2(Width / 2, Height - 1075), ImVec2(vHeadBoneOut.x, vHeadBoneOut.y - 20), ee, 0.5f);
		}

		if (config.healthbar)
		{
			int G1 = (255 * health / 100);
			int R1 = 255 - G1;
			ImColor healthcol = { R1, G1, 0, 255 };

			DrawRightProgressBar(zero_w2s_r.x + 30, head_w2s_r.y, BoxWidth, BoxHeight, 2, health);
		}
		if (isVisible(this_player.mesh))
			config.aimbot_fovyes = config.aimbot_fov;

		std::vector<int> bonesToScan;

		if (this_player.bone_count == 101) //female
		{
			bonesToScan = { 8,7,6,5 };
		}
		else if (this_player.bone_count == 103) //npc
		{
			bonesToScan = { 8,7,6,5 };
		}
		else if (this_player.bone_count == 104) //male
		{
			bonesToScan = { 8,7,6,5 };
		}
		if (config.boneidi == 3) {
			for (int bone : bonesToScan)
			{

				fvector head = bone_matrix(bone, this_player);
				fvector head_w2s = w2s(head);

				if (head_w2s.x != 0 || head_w2s.y != 0)
				{
					float delta_x = head_w2s.x - (Width / 2.f);
					float delta_y = head_w2s.y - (Height / 2.f);
					float dist = sqrtf(delta_x * delta_x + delta_y * delta_y);
					float fovdist = CalculateDistance(Width / 2, Height / 2, head_w2s.x, head_w2s.y);
					if ((dist < closest_distance) && fovdist < (config.aimbot_fovyes)) {
						closest_distance = dist;
						closestplayer = x;
						config.boneid_ = bone;
					}
				}
			}


		}
		else
		{
			fvector head = bone_matrix(config.boneid, this_player);
			fvector head_w2s = w2s(head);
			float delta_x = head_w2s.x - (Width / 2.f);
			float delta_y = head_w2s.y - (Height / 2.f);
			float dist = sqrtf(delta_x * delta_x + delta_y * delta_y);
			float fovdist = CalculateDistance(Width / 2, Height / 2, head_w2s.x, head_w2s.y);
			if ((dist < closest_distance) && fovdist < (config.aimbot_fovyes)) {
				closest_distance = dist;
				closestplayer = x;
				config.boneid_ = config.boneid;
			}
		}
	}
	if (closestplayer != 1337) {
		player& this_player = player_pawns[closestplayer];
		if (this_player.actor) {
			fvector rootpos = read<fvector>(this_player.root_component + ROOT_POS);
			fvector head = bone_matrix(config.boneid_, this_player);
			fvector head_w2s = w2s(head);
				if (config.isVisible ? isVisible(this_player.mesh) : true) {
					if (GetAsyncKeyState(config.current_key) & 0x8000) {
						if (config.aibmot) {
							aimbot(head_w2s.x, head_w2s.y, config.aim_speed / 10);
						}
					}
				}
		}
	}
}
bool ShowMenu = true;
struct State {
	uintptr_t keys[7];
};
void finda()
{
	
	static std::vector<player> player_cache;

	uint64_t world;
	uint64_t game_instance;
	uint64_t persistent_level;
	uint64_t local_player;
	uintptr_t pawn;
	uintptr_t playerstate;
	uintptr_t teamComponent;
	int teamID;
	uint64_t root_comp;
	uint64_t actor;
	uint64_t Health;
	uint64_t actor_count;

	while (true) {

		player_cache.clear();
		world = read2<uintptr_t>(virtualaddy + 0x60);
		
		world = validate_pointer(world);
		if (!world) continue;
		
		game_instance = read2<uintptr_t>(world + GAME_INSTANCE);
		if (!game_instance) continue;
		
		persistent_level = read2<uintptr_t>(world + PERSISTENT_LEVEL);
		persistent_level = validate_pointer(persistent_level);
		if (!persistent_level) continue;
		
		uintptr_t local_players = read2<uintptr_t>(game_instance + LOCALPLAYERS_ARRAY);
		if (!local_players) continue;
		
		local_player = read2<uintptr_t>(local_players);
		if (!local_player) continue;
		
		pointer::player_controller = read2<uintptr_t>(local_player + PLAYER_CONTROLER);
		
	
		pawn = read2<DWORD_PTR>(pointer::player_controller + LOCALPLAYERPAWN);
		playerstate = read2<uintptr_t>(pawn + PLAYER_STATE);
		teamComponent = read2<uintptr_t>(playerstate + TEAM_COMP);
		teamID = read2<int>(teamComponent + TEAM_ID);
		
	
		uintptr_t local_pawn = read2<uintptr_t>(pointer::player_controller + LOCALPLAYERPAWN);

		pointer::local_pawn = local_pawn;

		pointer::camera_manager = read2<uintptr_t>(pointer::player_controller + CAMERA_MANAGER);
		pointer::camera_position = read<fvector>(pointer::camera_manager + CAMERA_POS);


		uintptr_t actor_array = read2<uintptr_t>(persistent_level + ACTOR_ARRAY);
		if (!actor_array) continue;
		
		actor_count = read2<INT32>(persistent_level + ACTOR_COUNT);
		if (!actor_count) continue;


		for (int x = 0; x < actor_count; x++)
		{
			actor = read2<uintptr_t>(actor_array + (x * 0x8));
			if (!actor) continue;
			
			uintptr_t mesh = read2<uintptr_t>(actor + MESH);
			if (!mesh) continue;
			
			IsVisible = isVisible(mesh);

			uintptr_t playerstate = read2<uintptr_t>(actor + PLAYER_STATE);
		
			uintptr_t team_component = read2<uintptr_t>(playerstate + TEAM_COMP);
			int team_id = read2<int>(team_component + TEAM_ID);
			int mybone_count = read2<int>(mesh + BONE_COUNT);
			bool is_bot = mybone_count == 103;
			if (team_id == teamID && !is_bot) {
				continue;
			}
			
			root_comp = read2<uintptr_t>(actor + ROOT_COMPONENT);
			if (!root_comp) continue;
			
			uintptr_t damage_handler = read2<uintptr_t>(actor + DAMAGE_HANDLER);
			if (!damage_handler) continue;
			
			Health = read2<float>(damage_handler + HEALTH);
			if (Health <= 0) continue;
			
			uintptr_t bone_array = read2<uintptr_t>(mesh + BONE_ARRAY);
			if (!bone_array) continue;
			bool active = read<bool>(actor + DORMANT);
			
			if (!active)
				continue;
			
			INT32 bone_count = read2<INT32>(mesh + BONE_COUNT);
			if (!bone_count) continue;
			{
				uintptr_t actor;
				uintptr_t mesh;
				uintptr_t bone_array;
				uintptr_t root_component;
				uintptr_t damage_handler;

				INT32 bone_count;
				INT32 ammo_count;

				std::string weapon_name;
				std::string agent_name;
				std::string player_name;

				float distance;
				float health;
				float shield;

				bool is_valid;
				bool is_damage_handler_guarded;
				bool is_mesh_guarded;
			};

			uintptr_t for_actor = 0x1, for_mesh = 0x1;
			bool is_damage_handler_guarded = false, is_mesh_guarded = false;
			//define player
			player this_player{
				for_actor, //guarded region ptr
				for_mesh, //guarded region ptr
				actor,
				mesh,
				bone_array,
				root_comp,
				damage_handler,
				bone_count,
				0, //ammo count
				"", //weapon name
				"", //agent name
				"", //player name
				0.f, //distance
				Health, //health
				0.f, //shleid
				true,
				is_damage_handler_guarded,
				is_mesh_guarded
			};

			if (!player_pawns.empty()) {
				auto found_player = std::find(player_pawns.begin(), player_pawns.end(), this_player);
				if (found_player == player_pawns.end())
				{

					player_pawns.push_back(this_player);
				}


			}
			else
			{

				player_pawns.push_back(this_player);
			}
		}

	

		Sleep(350);
	}
}



