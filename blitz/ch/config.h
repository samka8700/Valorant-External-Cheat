#pragma once
#include <json.hpp>
#include "../py/py.h"
#include <WinUser.h>
struct Config {
	bool player_box = false;
	bool player_skeleton = false;
	bool circlehead = false;
	bool distance = false;
	bool fovcircle = false;
	bool isVisible = true;
	bool snapline = false;
	bool player_box2d = false;
	bool healthbar = false;
	int current_key = VK_XBUTTON2;
	int gui = VK_INSERT;
	ImVec4 player_box_color = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);
	ImVec4 player_skeleton_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	bool aibmot = true;
	int boneidi = 3;
	int boneid = 5;
	int boneid_ = 5;
	float aim_speed = 30;
	float aimbot_fov = 20.f;
	float aimbot_fovyes = 0.f;
	int testsleep = 1;
}; Config config;