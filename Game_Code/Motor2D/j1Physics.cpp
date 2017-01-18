#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Physics.h"
#include "p2Point.h"
#include "math.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "p2Log.h"

//#include "ModulePlayer.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif


j1Physics::j1Physics() : j1Module()
{
	name.create("physics");

	world = NULL;
	mouse_joint = NULL;
	flap_down_left_fix_joint = NULL;
	debug = false;
	name.create("physics");
}

// Destructor
j1Physics::~j1Physics()
{
}

bool j1Physics::Awake(pugi::xml_node& config) {

	LOG("ModulePhysics Configuration Loaded!");
	return true;
}

bool j1Physics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	//Flaps
	flap_down_right = App->physics->CreateRectangle(535, 784, 70, 12, MAP);
	flap_down_left = App->physics->CreateRectangle(388, 783, 70, 12, MAP);
	flap_up_left = App->physics->CreateRectangle(300, 309, 70, 12, MAP);
	flap_up_right = App->physics->CreateRectangle(630, 375, 70, 12, MAP);

	//Flaps points
	flap_down_right_point = App->physics->CreateStaticCircle(535, 784, 4, MAP);
	flap_down_left_point = App->physics->CreateStaticCircle(388, 783, 4, MAP);
	flap_up_left_point = App->physics->CreateStaticCircle(300, 309, 4, MAP);
	flap_up_right_point = App->physics->CreateStaticCircle(630, 375, 4, MAP);


	//Flaps joints
	b2RevoluteJointDef def;

	//Up Right
	def.bodyA = flap_up_right_point->body;
	def.bodyB = flap_up_right->body;
	def.lowerAngle = -55 * DEGTORAD;
	def.upperAngle = -20 * DEGTORAD;
	def.enableLimit = true;
	def.localAnchorB = { 0.500f,0.0f };
	flap_up_right_fix_joint = (b2RevoluteJoint*)world->CreateJoint(&def);

	//Down Right
	def.bodyA = flap_down_right_point->body;
	def.bodyB = flap_down_right->body;
	def.lowerAngle = -22 * DEGTORAD;
	def.upperAngle = 30 * DEGTORAD;
	def.enableLimit = true;
	def.localAnchorB = { 0.500f,0.0f };
	flap_down_right_fix_joint = (b2RevoluteJoint*)world->CreateJoint(&def);

	//Up Left
	def.bodyA = flap_up_left_point->body;
	def.bodyB = flap_up_left->body;
	def.lowerAngle = -10 * DEGTORAD;
	def.upperAngle = 35 * DEGTORAD;
	def.enableLimit = true;
	def.localAnchorB = { -0.500f,0.0f };
	flap_up_left_fix_joint = (b2RevoluteJoint*)world->CreateJoint(&def);

	//Down Left
	def.bodyA = flap_down_left_point->body;
	def.bodyB = flap_down_left->body;
	def.lowerAngle = -26 * DEGTORAD;
	def.upperAngle = 24 * DEGTORAD;
	def.enableLimit = true;
	def.localAnchorB = { -0.500f,0.0f };
	flap_down_left_fix_joint = (b2RevoluteJoint*)world->CreateJoint(&def);

	//Wheels

	//Mid
	mid_wheel_point = App->physics->CreateStaticCircle(532, 242, 4, MAP);
	mid_wheel = CreateCircle(532, 238, 25, MAP);
	def.bodyA = mid_wheel->body;
	def.bodyB = mid_wheel_point->body;
	def.motorSpeed = -8.0f;
	def.maxMotorTorque = 80.0f;
	def.localAnchorB = { 0.0f , 0.0f };
	def.enableMotor = false;
	def.enableLimit = false;
	mid_wheel_engine = (b2RevoluteJoint*)world->CreateJoint(&def);
	mid_wheel->joint = mid_wheel_engine;


	//LEFT
	left_wheel_point = App->physics->CreateStaticCircle(474, 180, 4, MAP);
	left_wheel = CreateCircle(474, 180, 25, MAP);
	def.bodyA = left_wheel->body;
	def.bodyB = left_wheel_point->body;
	def.motorSpeed = -8.0f;
	def.maxMotorTorque = 80.0f;
	def.localAnchorB = { 0.0f , 0.0f };
	def.enableMotor = true;
	def.enableMotor = false;
	def.enableLimit = false;
	left_wheel_engine = (b2RevoluteJoint*)world->CreateJoint(&def);
	left_wheel->joint = left_wheel_engine;

	//RIGHT
	right_wheel_point = App->physics->CreateStaticCircle(590, 180, 4, MAP);
	right_wheel = CreateCircle(590, 180, 25, MAP);
	def.bodyA = right_wheel->body;
	def.bodyB = right_wheel_point->body;
	def.motorSpeed = -8.0f;
	def.maxMotorTorque = 80.0f;
	def.localAnchorB = { 0.0f , 0.0f };
	def.enableMotor = true;
	def.enableMotor = false;
	def.enableLimit = false;
	right_wheel_engine = (b2RevoluteJoint*)world->CreateJoint(&def);
	right_wheel->joint = right_wheel_engine;



	return true;
}

// 
bool j1Physics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}



	return true;
}

PhysBody* j1Physics::CreateCircle(int x, int y, int radius, collision_type type)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.filter.categoryBits = type;

	if (type == BALL)
		b->SetBullet(true);
		fixture.filter.maskBits = LAUNCHER | SENSOR;
	if (type == MAP)
	{
		fixture.filter.maskBits = BALL;
		fixture.restitution = 1.0f;

	}

	b->CreateFixture(&fixture);
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* j1Physics::CreateStaticCircle(int x, int y, int radius, collision_type type)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* j1Physics::CreateRectangle(int x, int y, int width, int height, collision_type type)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.filter.categoryBits = type;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* j1Physics::CreateRectangleSensor(int x, int y, int width, int height, collision_type type, BODY_TYPE b_type)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;
	fixture.filter.categoryBits = type;
	fixture.filter.maskBits = BALL;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	pbody->collide_type = b_type;

	return pbody;
}

PhysBody* j1Physics::CreateChain(int x, int y, int* points, int size, collision_type type, uint restitution, BODY_TYPE b_type)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.filter.categoryBits = type;
	fixture.filter.maskBits = BALL;
	
	fixture.restitution = restitution;
	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->collide_type = b_type;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* j1Physics::CreateSensorChain(int x, int y, int* points, int size, collision_type type, BODY_TYPE b_type, uint restitution)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
		
	}

	shape.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.isSensor = true;
	fixture.filter.categoryBits = type;
	fixture.filter.maskBits = BALL;

	fixture.restitution = restitution;
	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->collide_type = b_type;

	return pbody;
}

// 
bool j1Physics::PostUpdate()
{
	/*
	if (delete_object)
	{

		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			if (b == App->scene_intro->ball_body->body)
			{
				b->DestroyFixture(App->scene_intro->ball_body->body->GetFixtureList());
			}
		}
		App->scene_intro->circles.clear();
		
		if (App->scene_intro->Balls_count <= (App->player->stand_lives + App->player->extra_balls)) {
						
				if (App->scene_intro->Balls_count != (App->player->stand_lives + App->player->extra_balls))
				{
					App->scene_intro->Set_lights_to_false();
					App->scene_intro->ball_body = CreateCircle(752, 725, 10, BALL);
					App->scene_intro->circles.add(App->scene_intro->ball_body);
					App->scene_intro->Balls_count++;
					App->scene_intro->circles.getLast()->data->listener = App->scene_intro;

					
				}
				else
				{
					App->scene_intro->game_state = END_GAME;
					App->scene_intro->ball_body = nullptr;
					App->scene_intro->Set_lights_to_false();
				}

		}
		delete_object = false;
	}
	*/

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return true;

	bool body_clicked = false;
	
	b2Body* click_body = nullptr;

	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->render->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->render->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{

				if (f->TestPoint({ PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()) }))
				{
					body_clicked = true;
					click_body = f->GetBody();
				}
			 }
			
		}
	}

	
	b2MouseJointDef def;

	if (body_clicked)
	{	
		def.bodyA = ground;
		def.bodyB = click_body;
		def.target = {PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY())};
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * click_body->GetMass();
		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);

	}
	
	if (mouse_joint && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		mouse_joint->SetTarget({ PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()) });
		App->render->DrawLine((App->input->GetMouseX()), (App->input->GetMouseY()), METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), 255, 0, 0);
	}

	if (mouse_joint && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
	}

	

	return true;
}


// Called before quitting
bool j1Physics::CleanUp()
{
	LOG("Destroying physics world");

	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void j1Physics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void j1Physics::If_Sensor_contact(PhysBody* bodyA, PhysBody* bodyB)
{
	/*
	b2Filter filter = bodyA->body->GetFixtureList()->GetFilterData();

	switch (bodyB->collide_type)
	{
		case DOOR:
			filter.maskBits = MAP | SENSOR | YELLOW_LIGHT | YELLOW_LIGHT_UP | ORANGE_LIGHT | ORANGE_LIGHT_UP | RED_LIGHT | RED_LIGHT_UP;
			bodyA->body->GetFixtureList()->SetFilterData(filter);
			break;
		
		case SENSOR_RAMP_A:
			App->scene_intro->level_floor = true;
			filter.maskBits = RAMP_A | FINAL_RAMP;
			bodyA->body->ApplyForce({ -50.0f, -200.0f }, bodyA->body->GetPosition(), true);
			bodyA->body->GetFixtureList()->SetFilterData(filter);
			App->audio->PlayFx(App->scene_intro->drift_1_fx);
			App->audio->PlayFx(App->scene_intro->points_fx);
			App->scene_intro->scape_light_3_on = true;
			App->player->score += 35000;
			break;

		case SENSOR_RAMP_B:
			App->scene_intro->level_floor = true;
			filter.maskBits = RAMP_B | FINAL_RAMP;
			bodyA->body->ApplyForce({ 0.0f, -200.0f }, bodyA->body->GetPosition(), true);
			bodyA->body->GetFixtureList()->SetFilterData(filter);
			App->audio->PlayFx(App->scene_intro->special_ramp_fx);
			App->audio->PlayFx(App->scene_intro->points_fx);
			App->scene_intro->scape_light_2_on = true;
			App->player->extra_balls += 1;
			App->player->score += 25000;
			break;

		case SENSOR_RAMP_C:
			bodyA->body->ApplyForce({ -50.0f, 0.0f }, bodyA->body->GetPosition(), true);
			filter.maskBits = RAMP_C | FINAL_RAMP;
			bodyA->body->GetFixtureList()->SetFilterData(filter);
			App->audio->PlayFx(App->scene_intro->drift_1_fx);
			App->audio->PlayFx(App->scene_intro->points_fx);
			App->scene_intro->scape_light_5_on = true;
			App->player->score += 9000;
			break;

		case TURBO_UP:
			bodyA->body->ApplyForce({ 0.0f, -200.0f }, bodyA->body->GetPosition(), true);
			break;

		case TURBO_DOWN:
			bodyA->body->ApplyForce({ 0.0f, 50.0f }, bodyA->body->GetPosition(), true);
			break;

		case TURBO_UP_LEFT:
			bodyA->body->ApplyForce({ -30.0f, -50.0f }, bodyA->body->GetPosition(), true);
			break;

		case WHEEL_LIGHT_LEFT:
			App->scene_intro->up_lights[0] = true;
			break;

		case WHEEL_LIGHT_MID:
			App->scene_intro->up_lights[1] = true;
			break;

		case WHEEL_LIGHT_RIGHT:
			App->scene_intro->up_lights[2] = true;
			break;

		case JACKPOT:
			App->player->score+= 5000;
			App->scene_intro->ball_into_jackpot = true;
			App->scene_intro->first_time = true;
			App->audio->PlayFx(App->scene_intro->jackpot_fx);
			break;


		case RAMP_LIGHT_RIGHT:
			App->scene_intro->scape_light_4_on = true;
			App->scene_intro->little_ramp_contact = true;
			bodyA->body->ApplyForce({ 20.0f, -50.0f }, bodyA->body->GetPosition(), true);
			App->audio->PlayFx(App->scene_intro->drift_2_fx);
			break;

		case RAMP_LIGHT_LEFT:
			App->scene_intro->scape_light_1_on = true;
			App->scene_intro->little_ramp_contact = true;
			bodyA->body->ApplyForce({ -20.0f, -50.0f }, bodyA->body->GetPosition(), true);
			App->audio->PlayFx(App->scene_intro->drift_2_fx);
			break;

		case RAMP_LIGHT_UP:
			App->scene_intro->scape_light_6_on = true;
			break;

		case RAMP_LIGHT_A:
			App->scene_intro->scape_light_3_on = true;
			break;

		case RAMP_LIGHT_B:
			App->scene_intro->scape_light_2_on = true;
			break;

		case RAMP_LIGHT_C:
			App->scene_intro->scape_light_5_on = true;
			break;

		case END:
			delete_object = true;
			App->audio->PlayFx(App->scene_intro->crash_2_fx);
			break;


	}

	switch (bodyB->body->GetFixtureList()->GetFilterData().categoryBits)
		{

	case FINAL_RAMP:

		bodyA->body->SetLinearVelocity({ 0.0f,0.0f });
		App->scene_intro->level_floor = false;

		filter.maskBits = MAP | SENSOR;

		if (!App->scene_intro->down_yellow_light_on)
			filter.maskBits = filter.maskBits | YELLOW_LIGHT;
		if (!App->scene_intro->down_orange_light_on)
			filter.maskBits = filter.maskBits | ORANGE_LIGHT;
		if (!App->scene_intro->down_red_light_on)
			filter.maskBits = filter.maskBits | RED_LIGHT;
		if (!App->scene_intro->up_yellow_light_on)
			filter.maskBits = filter.maskBits | YELLOW_LIGHT_UP;
		if (!App->scene_intro->up_orange_light_on)
			filter.maskBits = filter.maskBits | ORANGE_LIGHT_UP;
		if (!App->scene_intro->up_red_light_on)
			filter.maskBits = filter.maskBits | RED_LIGHT_UP;

		if (App->scene_intro->App->scene_intro->scape_light_3_on)
			App->scene_intro->scape_light_3_on = false;
		if (App->scene_intro->App->scene_intro->scape_light_2_on)
			App->scene_intro->scape_light_2_on = false;
		if (App->scene_intro->App->scene_intro->scape_light_5_on)
			App->scene_intro->scape_light_5_on = false;

		bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;

		case YELLOW_LIGHT:
			bodyA->body->SetLinearVelocity({ -bodyA->body->GetLinearVelocity().x * 0.5f ,bodyA->body->GetLinearVelocity().y * 0.5f });
			filter.maskBits -= YELLOW_LIGHT;
			{
				App->scene_intro->down_yellow_light_on = true;
				App->audio->PlayFx(App->scene_intro->jackpot_fx);
				App->player->score += 250;
			}
			bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;

		case ORANGE_LIGHT:
			bodyA->body->SetLinearVelocity({ -bodyA->body->GetLinearVelocity().x * 1.1f ,bodyA->body->GetLinearVelocity().y * 0.5f });
			filter.maskBits -= ORANGE_LIGHT;
			if (App->scene_intro->down_orange_light_on == false)
			{
				App->scene_intro->down_orange_light_on = true;
				App->audio->PlayFx(App->scene_intro->jackpot_fx);
				App->player->score += 250;
			}
			bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;

		case RED_LIGHT:
			bodyA->body->SetLinearVelocity({ -bodyA->body->GetLinearVelocity().x * 1.1f ,bodyA->body->GetLinearVelocity().y * 0.5f });
			filter.maskBits -= RED_LIGHT;
			if (App->scene_intro->down_red_light_on == false) 
			{
				App->scene_intro->down_red_light_on = true;
				App->audio->PlayFx(App->scene_intro->jackpot_fx);
				App->player->score += 250;
			}
			bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;

		case YELLOW_LIGHT_UP:
			bodyA->body->SetLinearVelocity({ -bodyA->body->GetLinearVelocity().x * 1.1f,bodyA->body->GetLinearVelocity().y * 0.5f });
			filter.maskBits -= YELLOW_LIGHT_UP;
			if (App->scene_intro->up_yellow_light_on == false) 
			{
				App->scene_intro->up_yellow_light_on = true;
				App->audio->PlayFx(App->scene_intro->jackpot_fx);
				App->player->score += 250;
			}
			bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;

		case ORANGE_LIGHT_UP:
			bodyA->body->SetLinearVelocity({ -bodyA->body->GetLinearVelocity().x * 1.1f,bodyA->body->GetLinearVelocity().y * 0.5f });
			filter.maskBits -= ORANGE_LIGHT_UP;
			if (App->scene_intro->up_orange_light_on == false)
			{
				App->scene_intro->up_orange_light_on = true;
				App->audio->PlayFx(App->scene_intro->jackpot_fx);
				App->player->score += 250;
			}
			bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;

		case RED_LIGHT_UP:
			bodyA->body->SetLinearVelocity({ -bodyA->body->GetLinearVelocity().x * 1.1f,bodyA->body->GetLinearVelocity().y * 0.5f });
			filter.maskBits -= RED_LIGHT_UP;
			if (App->scene_intro->up_red_light_on == false) 
			{
				App->scene_intro->up_red_light_on = true;
				App->audio->PlayFx(App->scene_intro->jackpot_fx);
				App->player->score += 250;
			}
			bodyA->body->GetFixtureList()->SetFilterData(filter);
		break;

		case STOP_SENSOR:
			bodyA->body->SetLinearVelocity({ 0.0f,0.0f });
			filter.maskBits = MAP | SENSOR;

			//set mask bits depending on if the lights are on or not
			if (!App->scene_intro->down_yellow_light_on)
				filter.maskBits = filter.maskBits | YELLOW_LIGHT;
			if (!App->scene_intro->down_orange_light_on)
				filter.maskBits = filter.maskBits | ORANGE_LIGHT;
			if (!App->scene_intro->down_red_light_on)
				filter.maskBits = filter.maskBits | RED_LIGHT;
			if (!App->scene_intro->up_yellow_light_on)
				filter.maskBits = filter.maskBits | YELLOW_LIGHT_UP;
			if (!App->scene_intro->up_orange_light_on)
				filter.maskBits = filter.maskBits | ORANGE_LIGHT_UP;
			if (!App->scene_intro->up_red_light_on)
				filter.maskBits = filter.maskBits | RED_LIGHT_UP;

			//turn the light off
			if (App->scene_intro->scape_light_4_on)
				App->scene_intro->scape_light_4_on = false;
			if (App->scene_intro->scape_light_1_on)
				App->scene_intro->scape_light_1_on = false;

			bodyA->body->GetFixtureList()->SetFilterData(filter);
			break;
	}

	*/
}