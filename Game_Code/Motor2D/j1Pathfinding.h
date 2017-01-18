#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Queue.h"
#include "p2Point.h"
#include "j1Module.h"
#include "p2DynArray.h"
#include "j1PerfTimer.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255
#define WALK_COST_IMP 5

// ----------------------------------------------------
class j1Pathfinding : public j1Module
{
public:

	//Constructor
	j1Pathfinding();

	// Destructor
	virtual ~j1Pathfinding();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before game loop
	bool Start();

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	//Path functions
	bool SetPathStart(iPoint coordenate);
	bool SetPathGoal(iPoint coordenate);

	//create a path from a to b
	int CreatePath(const iPoint& origin, const iPoint& destination, bool diagonals, bool walk_cost = false);

	//Calculate the path cost
	bool CanReach(const iPoint& origin, const iPoint& goal);

	const p2DynArray<iPoint>* GetLastPath() const;

	//Return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	//BFS functions
	bool PropagateBFS(const iPoint& origin,const iPoint& goal, p2List<iPoint>* close_list = nullptr, p2Queue<iPoint>* open_list = nullptr);

	//Dijkstra functions
	void PropagateDijkstra();

	//A* functions
	void PropageteA();

	bool ResetPath();

	iPoint start;
	iPoint goal;

private:

	bool correct_path = false;

	p2Queue<iPoint>		open;
	p2List<iPoint>		close;



	//All map walkability data
	uchar* walkability_map;

	//All map walk cost data
	uchar* walk_cost_map;

	//All map way size data
	uchar* way_size_map;

public:

	//Set walkability map
	void SetWalkabilityMap(uint width, uint height, uchar* data);

	//Set walk cost map
	void SetWalkCostMap(uint widht, uint height, uchar* data);

	//Get the cell X way size
	int GetCellWaySize(const iPoint& point)const;

	//Create way size map
	bool CreateWaySizeMap(uint widht, uint height, uchar** data);

	//Set way size map
	void SetWaySizeMap(uint widht, uint height, uchar* data);

	//Returns true if the tile is walkable
	bool IsWalkable(const iPoint& pos, uint way_size = 1) const;

	//Return the walkability value of a tile
	uchar GetTileWalkability(const iPoint& pos) const;

private:

	// size of the map
	uint width;
	uint height;

	// we store the created path here
	p2DynArray<iPoint> last_path;

public:

	//Get the size of the last path
	uint GetPathSize()const;

	//Get X cell of the last path
	iPoint GetPathCell(uint cell)const;

	public:

		//Handle Console Input ----------------------
		void Console_Command_Input(Command* command, Cvar* cvar, p2SString* input);

		void Console_Cvar_Input(Cvar* cvar, Command* command_type, p2SString* input);

};


struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill, bool diagonals) const;

	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	// -----------
	int g;
	int h;
	iPoint pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	p2List_item<PathNode>* Find(const iPoint& point) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	p2List_item<PathNode>* GetNodeLowestScore(const iPoint& to, bool walk_cost) const;

	// -----------
	// The list itself, note they are not pointers!
	p2List<PathNode> list;
};


#endif // __j1PATHFINDING_H__