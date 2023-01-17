#ifndef __GRID_H__
#define __GRID_H__

#include <vector>
#include <world/collision/CollisionObject.h>

using namespace std;

typedef vector<vector<vector<CollisionObject*>>> GRID;

class Grid{

public:
	Grid();
	virtual ~Grid();

	virtual vector<CollisionObject*> get_objects_in_cell(int x, int y){
		if(m_cells.empty()) return {};
		if(y < 0 || y >= m_cells.size()) return {};
		if(x < 0 || x >= m_cells.at(y).size()) return {};
		return m_cells.at(y).at(x);
	}
	virtual void insert_object(CollisionObject* object){
		
	}

private:
	int m_cell_width;
	int m_cell_height;
	GRID m_cells;
};

#endif // !__GRID_H__
