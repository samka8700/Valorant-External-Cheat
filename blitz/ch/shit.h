bool IsVisible;

float silent_smooth = 5.f;



DWORD_PTR LocalPawn;
uintptr_t playerstate;
uintptr_t teamComponent;
int teamID;
uintptr_t world;
uintptr_t game_instance;
uintptr_t persistent_level;
uintptr_t local_player;
uintptr_t camera_manager;
INT32 actor_count;

int customX;
int customY;

namespace offsets
{
	std::ptrdiff_t uworld_pointer = 0x60; //UWorld*
	std::ptrdiff_t uworld_state = 0x9ed7ac0	; 			// game_base (Pointer: 0x60)
	std::ptrdiff_t uworld_key = uworld_state + 0x38;		// game_base
	std::ptrdiff_t g_object_state = 0x8FEFDC0;			// game_base
	std::ptrdiff_t g_object_key = 0x8FEFDC038;				// game_base
	std::ptrdiff_t line_of_sight = 0x4ABCA90;			// game_base
	std::ptrdiff_t bone_matrix = 0x4A893B0;		// game_base

	// veh deref
	std::ptrdiff_t pakman_offset = 0x1B67F90;                     // deref_pointer_in_game_space_fn - https://www.unknowncheats.me/forum/valorant/503616-dumping-valorant-perfect-results-easy.html

	// world
	std::ptrdiff_t persistent_level = 0x38;			// world > persistent_level
	std::ptrdiff_t game_instance = 0x1A0;				// world > game_instance

	// player
	std::ptrdiff_t localplayers_array = 0x40;			// world > game_instance > localplayers_array
	std::ptrdiff_t localplayer = 0x40;				// world > game_instance > localplayers_array[0]
	std::ptrdiff_t player_controller = 0x38;			// world > game_instance > localplayers_array[0] > playercontroller
	std::ptrdiff_t apawn = 0x468;					// world > game_instance > localplayers_array[0] > playercontroller > apawn									// aactor > apawn

	// vector
	std::ptrdiff_t root_component = 0x230;			// world > game_instance > localplayers_array[0] > playercontroller > apawn > root_component				// aactor > root_component
	std::ptrdiff_t root_position = 0x164;				// world > game_instance > localplayers_array[0] > playercontroller > apawn > root_component > root_position		// aactor > root_component > position

	// controllers
	std::ptrdiff_t damage_handler = 0xa10;			// world > game_instance > localplayers_array[0] > playercontroller > apawn > damage_handler			// aactor > damage_controller
	std::ptrdiff_t camera_controller = 0x440;			// world > game_instance > localplayers_array[0] > playercontroller > camera_controller

	// camera
	std::ptrdiff_t camera_position = 0x1280;			// world > game_instance > localplayers_array[0] > playercontroller > camera_controller > camera_position
	std::ptrdiff_t camera_rotation = 0x128C;		        // world > game_instance > localplayers_array[0] > playercontroller > camera_controller > camera_rotation
	std::ptrdiff_t camera_fov = 0x1298;				// world > game_instance > localplayers_array[0] > playercontroller > camera_controller > camera_fov
	std::ptrdiff_t camera_manager = 0x478;			        // world > game_instance > localplayers_array[0] > playercontroller > camera_manager


	// level > actors
	std::ptrdiff_t actor_array = 0xA0;				// world > persistent_level > actor_array
	std::ptrdiff_t actors_count = 0x18;				// world > persistent_level > actors_count

	// level > actors info
	std::ptrdiff_t actor_id = 0x18; 		                  // world > persistent_level > aactor > actor_id
	std::ptrdiff_t unique_id = 0x38;		                  // world > persistent_level > aactor > unique_id
	std::ptrdiff_t team_component = 0x628;		                  // world > persistent_level > aactor > player_state > team_component
	std::ptrdiff_t team_id = 0xF8;			                  // world > persistent_level > aactor > team_component > team_id
	std::ptrdiff_t health = 0x1B0;		                          // world > persistent_level > aactor > damage_controller > health
	std::ptrdiff_t dormant = 0x100;			                  // world > persistent_level > aactor > dormant
	std::ptrdiff_t player_state = 0x3F0;				  // world > persistent_level > aactor > player_state

	// mesh
	std::ptrdiff_t control_rotation = 0x440;
	std::ptrdiff_t mesh = 0x430;					// world > persistent_level > aactor > mesh
	std::ptrdiff_t component_to_world = 0x250;			// world > persistent_level > aactor > mesh > component_to_world
	std::ptrdiff_t bone_array = 0x5D8;				// world > persistent_level > aactor > mesh > bone_array
	std::ptrdiff_t bone_count = 0x5E0;				// world > persistent_level > aactor > mesh > bone_array + (index * bone_count)
	std::ptrdiff_t last_submit_time = 0x378;			// world > persistent_level > aactor -> mesh -> last_submit_time
	std::ptrdiff_t last_render_time = 0x37C;			// world > persistent_level > aactor -> mesh -> last_render_time

	// chams / glow chams : method1 - BlackMax97
	std::ptrdiff_t outline_modee = 0x330;				// mesh > outline_mode
	std::ptrdiff_t attach_children = 0x110;			// mesh > attach_children
	std::ptrdiff_t attach_children_count = 0x118;			// mesh > attach_children + 0x8

	// chams / glow chams : method2 - unknownjunks
	std::ptrdiff_t outline_component = 0x1128;			// actor > outline_component3p
	std::ptrdiff_t outline_mode = 0x2B1;				// outline_component3p > outline_mode

	// chams / glow chams colors
	std::ptrdiff_t outline_ally_color = 0x9044340;		// game_base
	std::ptrdiff_t outline_enemy_color = 0x9045C20;		// game_base

	// minimap
	std::ptrdiff_t portrait_minimap_component = 0x10c0;		// actor > portrait_minimap_component
	std::ptrdiff_t character_minimap_component = 0x10c8;		// actor > character_minimap_component
	std::ptrdiff_t local_observer = 0x530;			// minimap_component > local_observer
	std::ptrdiff_t is_visible = 0x501;				// minimap_component > is_visible
}

