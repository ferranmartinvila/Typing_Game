#ifndef _J1PHYSICS_H_
#define _J1PHYSICS_H_

#include "j1Module.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -9.8f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum collision_type
{
	BALL = 1,
	RAMP_A = 2,
	RAMP_B = 4,
	RAMP_C = 8,
	MAP = 16,
	LAUNCHER = 32,
	SENSOR = 64,
	YELLOW_LIGHT = 128,
	ORANGE_LIGHT = 256,
	RED_LIGHT = 512,
	YELLOW_LIGHT_UP = 1024,
	ORANGE_LIGHT_UP = 2048,
	RED_LIGHT_UP = 4096,
	FINAL_RAMP = 8192,
	STOP_SENSOR = 16384
	
};

enum BODY_TYPE 
{
	NONE,
	SENSOR_RAMP_A,
	SENSOR_RAMP_B,
	SENSOR_RAMP_C,
	TURBO_UP,
	TURBO_UP_LEFT,
	TURBO_DOWN,
	WHEEL_LIGHT_LEFT,
	WHEEL_LIGHT_MID,
	WHEEL_LIGHT_RIGHT,
	RAMP_LIGHT_LEFT,
	RAMP_LIGHT_RIGHT,
	RAMP_LIGHT_UP,
	RAMP_LIGHT_A,
	RAMP_LIGHT_B,
	RAMP_LIGHT_C,
	LEFT_LUNG,
	RIGHT_LUNG,
	END,
	JACKPOT,
	DOOR
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	

public:

	int width, height;
	b2Body* body;
	j1Module* listener;
	BODY_TYPE collide_type;
	b2RevoluteJoint* joint = nullptr;
};

// Module --------------------------------------
class j1Physics : public j1Module, public b2ContactListener
{
public:

	j1Physics();
	~j1Physics();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, collision_type type);
	PhysBody* CreateStaticCircle(int x, int y, int radius, collision_type type);
	PhysBody* CreateRectangle(int x, int y, int width, int height, collision_type type);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, collision_type type, BODY_TYPE b_type = NONE);
	PhysBody* CreateChain(int x, int y, int* points, int size, collision_type type,  uint restitution = 0, BODY_TYPE b_type = NONE);
	PhysBody* CreateSensorChain(int x, int y, int* points, int size, collision_type type, BODY_TYPE b_type = NONE, uint restitution = 0);
	
	

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	void If_Sensor_contact(PhysBody* bodyA, PhysBody* bodyB);

private:

	bool debug;
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* ground;

public:

	PhysBody* flap_up_right;
	PhysBody* flap_down_right;
	PhysBody* flap_up_left;
	PhysBody* flap_down_left;

	PhysBody* left_wheel;
	PhysBody* mid_wheel;
	PhysBody* right_wheel;




private:


	PhysBody* flap_up_right_point;
	PhysBody* flap_down_right_point;
	PhysBody* flap_up_left_point;
	PhysBody* flap_down_left_point;

	b2RevoluteJoint* flap_up_right_fix_joint;
	b2RevoluteJoint* flap_down_right_fix_joint;
	b2RevoluteJoint* flap_up_left_fix_joint;
	b2RevoluteJoint* flap_down_left_fix_joint;

	PhysBody* mid_wheel_point;
	PhysBody* left_wheel_point;
	PhysBody* right_wheel_point;

	b2RevoluteJoint* mid_wheel_engine;
	b2RevoluteJoint* left_wheel_engine;
	b2RevoluteJoint* right_wheel_engine;

	bool delete_object = false;
};

#endif