#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Console.h"

#include <math.h>

j1Pathfinding::j1Pathfinding() : j1Module(), walkability_map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)

{
	name.create("pathfinding");
}

// Destructor
j1Pathfinding::~j1Pathfinding()
{
	RELEASE_ARRAY(walkability_map);
	RELEASE_ARRAY(walk_cost_map);
	RELEASE_ARRAY(way_size_map);
}

// Called before render is available
bool j1Pathfinding::Awake(pugi::xml_node& config)
{
	LOG("Loading Pathfinding data");
	bool ret = true;
	goal.GetOut();
	return ret;
}

bool j1Pathfinding::Start() {
	bool ret = true;

	App->console->AddCommand("reset_path", this);

	return ret;
}

bool j1Pathfinding::SetPathStart(iPoint coordenate) {

	if (IsWalkable(coordenate) && close.find(coordenate) == -1) {
		start = coordenate;
		open.Push(start, 0);
		close.add(start);
		correct_path = true;
	}
	else correct_path = false;

	return correct_path;
}

bool j1Pathfinding::SetPathGoal(iPoint coordenate) {

	if (IsWalkable(coordenate)) {
		goal = coordenate;
		correct_path = true;
	}
	else correct_path = false;

	return correct_path;
}

void j1Pathfinding::SetWalkabilityMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(walkability_map);
	walkability_map = new uchar[width*height];
	memcpy(walkability_map, data, width*height);
}

void j1Pathfinding::SetWalkCostMap(uint widht, uint height, uchar * data)
{
	this->width = widht;
	this->height = height;

	RELEASE_ARRAY(walk_cost_map);
	walk_cost_map = new uchar[widht*height];
	memcpy(walk_cost_map, data, widht*height);
}

int j1Pathfinding::GetCellWaySize(const iPoint & point) const
{
	int way_size = 0;
	if (IsWalkable(point))way_size++;

	for (int k = 1; k < width; k++) {

		for (int y = 0; y < k; y++) {
			for (int x = 0; x < k; x++) {

				if (IsWalkable({ (point.x - (k - 1)) + x, (point.y - (k - 1)) + y }) == false) {
					return k;
				}

			}
		}
	}

	return width;
}

bool j1Pathfinding::CreateWaySizeMap(uint widht, uint height, uchar ** data)
{
	uchar* way_data = new uchar[widht* height];
	memset(way_data, 0, widht*height);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < widht; x++) {

			way_data[y*widht + x] = GetCellWaySize({ x, y });

		}
	}

	if (way_data == NULL)return false;

	*data = way_data;
	return true;
}

void j1Pathfinding::SetWaySizeMap(uint widht, uint height, uchar * data)
{
	this->width = widht;
	this->height = height;

	RELEASE_ARRAY(way_size_map);
	way_size_map = new uchar[widht*height];
	memcpy(way_size_map, data, widht*height);
}

int j1Pathfinding::CreatePath(const iPoint& origin, const iPoint& goal, bool diagonals, bool walk_cost) {


	//If origin or destination are not walkable, return -1
	if (!IsWalkable(origin) || !IsWalkable(goal))return -1;


	//Clean the last path
	last_path.Clear();

	//Create two lists: open, close
	PathList open_list;
	PathList close_list;

	//Origin node
	PathNode origin_node(0, origin.DistanceManhattan(goal), origin, nullptr);

	//Add the origin tile to open
	open_list.list.add(origin_node);

	//Iterate while we have tile in the open list
	while (open_list.list.count() > 0) {

		//Move the lowest score cell from open list to the closed list
		PathNode open_list_lowest = open_list.GetNodeLowestScore(goal, walk_cost)->data;

		close_list.list.add(open_list_lowest);
		open_list.list.del(open_list.Find(open_list_lowest.pos));

		//LOG("G:%i H:%i", open_list_lowest.g, open_list_lowest.h);




		//If we just added the destination, we are done!
		if (close_list.list.end->data.pos == goal) {

			//Backtrack to create the final path
			for (p2List_item<PathNode>* node = close_list.list.end; node->data.parent != nullptr; node = close_list.Find(node->data.parent->pos)) {

				last_path.PushBack(node->data.pos);

			}

			last_path.PushBack(origin_node.pos);

			last_path.Flip();

			return close_list.list.count();
		}

		//Fill a list of all adjancent nodes
		PathList adjacent_nodes;


		close_list.list.end->data.FindWalkableAdjacents(adjacent_nodes, diagonals);

		//Respect the corners 
		if (adjacent_nodes.list.count() < 8 && diagonals) {
			adjacent_nodes.list.clear();
			close_list.list.end->data.FindWalkableAdjacents(adjacent_nodes, false);
		}

		//Iterate adjancent nodes:
		for (p2List_item<PathNode>* adjacent_node = adjacent_nodes.list.start; adjacent_node != NULL; adjacent_node = adjacent_node->next) {

			// ignore nodes in the closed list
			if (close_list.Find(adjacent_node->data.pos) != NULL)
			{
				continue;
			}

			// If it is NOT found


			p2List_item<PathNode>* node = open_list.Find(adjacent_node->data.pos);

			//If it is already in the open list 
			if (node != NULL) {


				//Check if it is a better path (compare G)
				adjacent_node->data.CalculateF(goal);
				if (adjacent_node->data.g < node->data.g) {

					//If it is a better path, Update the parent
					node->data.parent = adjacent_node->data.parent;

				}
			}

			//Calculate its F and add it to the open list
			else {

				adjacent_node->data.CalculateF(goal);
				open_list.list.add(adjacent_node->data);

			}
		}

	}

	return -1;

}

bool j1Pathfinding::CanReach(const iPoint& origin, const iPoint& goal)
{
	p2List<iPoint> close_list;
	p2Queue<iPoint> open_list;
	open_list.Push(origin);
	uint distance_to_loop = origin.DistanceManhattan(goal) * 25;

	while (distance_to_loop > 0) {
		if (PropagateBFS(origin, goal, &close_list, &open_list)) {
			LOG("TRUE");
			close_list.clear();
			open_list.Clear();
			return true;
		}

		distance_to_loop--;
	}
	LOG("FALSE");
	close_list.clear();
	open_list.Clear();
	return false;
}

const p2DynArray<iPoint>* j1Pathfinding::GetLastPath() const
{
	return &last_path;
}

bool j1Pathfinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x < (int)width &&
		pos.y >= 0 && pos.y < (int)height);
}

bool j1Pathfinding::IsWalkable(const iPoint& pos, uint way_size) const
{
	uchar t = GetTileWalkability(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

uchar j1Pathfinding::GetTileWalkability(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return walk_cost_map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}



bool j1Pathfinding::ResetPath()
{
	if (last_path.Count() > 0)
	{
		last_path.Clear();
		return true;
	}
	else {
		open.Clear();
		close.clear();
		goal.GetOut();
		return false;
	}
}

uint j1Pathfinding::GetPathSize() const
{
	return last_path.Count();
}

iPoint j1Pathfinding::GetPathCell(uint cell) const
{
	return *last_path.At(cell);
}


bool j1Pathfinding::PropagateBFS(const iPoint& origin, const iPoint& goal, p2List<iPoint>* close_list, p2Queue<iPoint>* open_list)
{
	p2List<iPoint>* close_l;
	if (close_list == nullptr)close_l = &close;
	else close_l = close_list;

	p2Queue<iPoint>* open_l;
	if (open_list == nullptr)open_l = &open;
	else open_l = open_list;

	if (close_l->find(goal) != -1) {
		return true;
	}

	iPoint point;

	if (open_l->start != NULL && close_l->find(goal) == -1) {

		open_l->Pop(point);

		if (open_l->find(point) == -1)close_l->add(point);

		iPoint neighbor[4];

		neighbor[0] = { point.x - 1,point.y };
		neighbor[1] = { point.x + 1,point.y };
		neighbor[2] = { point.x,point.y - 1 };
		neighbor[3] = { point.x,point.y + 1 };

		for (uint j = 0; j < 4; j++) {

			if (close_l->find(neighbor[j]) == -1 && IsWalkable(neighbor[j])) {

				open_l->Push(neighbor[j]);
				close_l->add(neighbor[j]);

			}

		}
	}

	return false;
}

void j1Pathfinding::PropagateDijkstra() {

	if (close.find(goal) != -1) {
		return;
	}

	iPoint point;

	if (open.start != NULL) {

		open.Pop(point);

		if (open.find(point) == -1)close.add(point);

		iPoint neighbor[4];

		neighbor[0] = { point.x - 1, point.y };
		neighbor[1] = { point.x + 1, point.y };
		neighbor[2] = { point.x, point.y - 1 };
		neighbor[3] = { point.x, point.y + 1 };

		for (uint k = 0; k < 4; k++) {

			if (close.find(neighbor[k]) == -1 && IsWalkable(neighbor[k])) {
				
				close.add(neighbor[k]);
				open.Push(neighbor[k], GetTileWalkability(neighbor[k]));


			}

		}

	}

}

void j1Pathfinding::PropageteA()
{
	if (close.find(goal) != -1) return;
	if (open.start == NULL)return;

	iPoint point;

	open.Pop(point);

	if (open.find(point) == -1)close.add(point);

	iPoint neighbor[4];

	neighbor[0] = { point.x - 1, point.y };
	neighbor[1] = { point.x + 1, point.y };
	neighbor[2] = { point.x, point.y - 1 };
	neighbor[3] = { point.x, point.y + 1 };

	for (uint k = 0; k < 4; k++) {

		if (close.find(neighbor[k]) == -1 && IsWalkable(neighbor[k])) {

			close.add(neighbor[k]);
			open.Push(neighbor[k], GetTileWalkability(neighbor[k]) + neighbor[k].DistanceManhattan(goal));

		}
	}

}


void j1Pathfinding::Draw()
{

	iPoint point;

	// Draw visited
	p2List_item<iPoint>* item = close.start;

	while (item)
	{
		point = item->data;
		TileSet* tileset = App->map->GetTilesetFromTileId(27);

		SDL_Rect r = tileset->GetTileRect(27);
		iPoint pos = App->map->MapToWorld(point.x, point.y);

		App->render->Blit(tileset->texture, pos.x, pos.y, &r);

		item = item->next;
	}

	// Draw frontier
	for (uint i = 0; i < open.Count(); ++i)
	{
		point = *(open.Peek(i));
		TileSet* tileset = App->map->GetTilesetFromTileId(26);

		SDL_Rect r = tileset->GetTileRect(26);
		iPoint pos = App->map->MapToWorld(point.x, point.y);

		App->render->Blit(tileset->texture, pos.x, pos.y, &r);
	}

	//Draw goal
	if (goal.IsOut() == false) {

		iPoint pos = App->map->MapToWorld(goal.x, goal.y);
		App->render->Blit(App->scene->tex_goal, pos.x, pos.y);

	}

	//Draw Path
	const p2DynArray<iPoint>* path = GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->tex_path, pos.x, pos.y);
	}
}

// Called before quitting
bool j1Pathfinding::CleanUp()
{
	LOG("Unloading pathfinding");

	open.Clear();
	close.clear();

	RELEASE_ARRAY(walkability_map);

	return true;
}


void j1Pathfinding::Console_Command_Input(Command * command, Cvar * cvar, p2SString * input)
{
	if (*command->GetCommandStr() == "reset_path")
	{
		if (this->open.Count() == 0)App->console->GenerateConsoleLabel("-- No path data can be reset --");
		else if (ResetPath())
		{
			App->console->GenerateConsoleLabel("-- Pathfining path has been reset --");
		}
		else
		{
			App->console->GenerateConsoleLabel("-- All pathfinding data reset --");
		}
	}
}

void j1Pathfinding::Console_Cvar_Input(Cvar * cvar, Command * command_type, p2SString * input)
{

}


// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore(const iPoint& to, bool walk_cost) const
{
	p2List_item<PathNode>* ret = NULL;

	int min = 65535;
	int item_walk_cost = 0;

	p2List_item<PathNode>* item = list.end;
	while (item)
	{
		//If walk cost is checked it calculates it
		if (walk_cost)item_walk_cost = App->pathfinding->GetTileWalkability(item->data.pos);
		
		//Recalculatess
		item->data.CalculateF(to);
		if ((item->data.Score() + (item_walk_cost * WALK_COST_IMP)) < min)
		{
			min = item->data.Score() + (item_walk_cost * WALK_COST_IMP);
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill, bool diagonals) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	//diagonal able
	if (diagonals == false)return list_to_fill.list.count();

	//north-east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	//north-west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	//south-east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	//south-west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell)) {
		list_to_fill.list.add(PathNode(-1, -1, cell, this));
	}

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	if (parent == nullptr)g = 0;
	else g = parent->g + 1;
	h = pos.DistanceManhattan(destination);
	//pos.DistanceTo(destination);

	return g + h;
}