#pragma once

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "Vector.h"
#include <vector>

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

class SceneDetourMgr {
private:
	dtNavMesh* m_mesh;
	dtNavMeshQuery* m_query;
public:
	SceneDetourMgr();
	~SceneDetourMgr();
	bool initNavMesh(const char* meshFileName);
	void freeNaveMesh();

	void findPath(float* sPos, float* ePos, std::vector<float>* path);
};
