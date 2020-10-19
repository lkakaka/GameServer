#include "DetourMgr.h"
#include "Logger.h"

SceneDetourMgr::SceneDetourMgr() : m_mesh(NULL)
{

}

SceneDetourMgr::~SceneDetourMgr()
{
	freeNaveMesh();
}

bool SceneDetourMgr::initNavMesh() 
{
	#ifdef WIN32
	FILE* fp = fopen("../../res/all_tiles_navmesh.bin", "rb");
	#else
	FILE* fp = fopen("../res/all_tiles_navmesh.bin", "rb");
	#endif
	if (!fp) {
		Logger::logError("$not found scene navmesh file£¡£¡£¡");
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

	fclose(fp);
	Logger::logInfo("$init scene navmesh success");
	return true;
}

void SceneDetourMgr::freeNaveMesh()
{
	dtFreeNavMesh(m_mesh);
	m_mesh = NULL;
}
