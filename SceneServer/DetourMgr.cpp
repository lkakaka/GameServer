#include "DetourMgr.h"
#include "Logger.h"

#define MAX_POLY_PATH 100
#define MAX_STRAIGHT_PATH 100

SceneDetourMgr::SceneDetourMgr() : m_mesh(NULL), m_query(NULL)
{

}

SceneDetourMgr::~SceneDetourMgr()
{
	freeNaveMesh();
	dtFreeNavMeshQuery(m_query);
}

bool SceneDetourMgr::initNavMesh(char* meshFileName) 
{
	FILE* fp = fopen(meshFileName, "rb");
	if (!fp) {
		Logger::logError("$not found scene navmesh file; %s£¡£¡£¡", meshFileName);
		return false;
	}

	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		Logger::logError("$init scene navmesh error, readlen:%d", readLen);
		return false;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		Logger::logError("$init scene navmesh error, header.magic:%d", header.magic);
		return false;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		Logger::logError("$init scene navmesh error, header.version:%d", header.version);
		return false;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		Logger::logError("$init scene navmesh error, alloc dtNavMesh failed");
		return false;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		Logger::logError("$init scene navmesh error, mesh init failed");
		return false;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			Logger::logError("$init scene navmesh error, NavMeshTileHeader readLen:%d", readLen);
			return false;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			Logger::logError("$init scene navmesh error, tileHeader readLen:%d", readLen);
			return false;
		}

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	m_mesh = mesh;
	m_query = dtAllocNavMeshQuery();
	status = m_query->init(m_mesh, 1000);
	if (dtStatusFailed(status)) {
		Logger::logError("$init scene navmesh error, navmesh query init failed");
		fclose(fp);
		return false;
	}

	fclose(fp);
	Logger::logInfo("$init scene navmesh success");
	return true;
}

void SceneDetourMgr::freeNaveMesh()
{
	dtFreeNavMesh(m_mesh);
	m_mesh = NULL;
}

void SceneDetourMgr::findPath(float* sPos, float* ePos, std::vector<float>* path) {
	float halfExtents[3]{1, 1, 1};
	dtQueryFilter filter;
	dtPolyRef startRef;
	m_query->findNearestPoly(sPos, halfExtents, &filter, &startRef, 0);
	dtPolyRef endRef;
	m_query->findNearestPoly(ePos, halfExtents, &filter, &endRef, 0);
	dtPolyRef polyPath[MAX_POLY_PATH];
	int pathCount = 0;
	m_query->findPath(startRef, endRef, sPos, ePos,
		&filter, polyPath, &pathCount, MAX_POLY_PATH);
	if (pathCount > 0) {
		float straightPath[MAX_STRAIGHT_PATH * 3];
		int nstraightPath;
		m_query->findStraightPath(sPos, ePos, polyPath, pathCount,
			straightPath, NULL, NULL, &nstraightPath, MAX_STRAIGHT_PATH);

		//m_path_len = nstraightPath;
		for (int i = 0; i < nstraightPath; ++i)
		{
			path->push_back(straightPath[i * 3]);
			path->push_back(straightPath[i * 3 + 1]);
			path->push_back(straightPath[i * 3 + 2]);
		}
	}
}
