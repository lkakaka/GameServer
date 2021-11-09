#include "LuaScene.h"
#include "Logger.h"
#include "MyBuffer.h"
#include "ServiceType.h"
#include "ServiceInfo.h"
#include "GameScene.h"
#include "SceneMgr.h"
#include "LuaPlugin.h"
#include "LuaRegistryObj.h"

static void callSceneScripFunc(void* ptr, ...) {
	//LOG_DEBUG("callSceneScripFunc, %d", scriptEvent);
	va_list args;
	va_start(args, ptr);
	int scriptEvent = va_arg(args, int);
	GameScene* gameScene = (GameScene*)ptr;
	std::shared_ptr<sol::state> luaPtr = LuaPlugin::getSingleton()->getLua();
	sol::table sceneScriptObj = luaPtr->registry()[gameScene->getLuaRef()];

	switch (scriptEvent) {
		case SceneScriptEvent::AFTER_ENTITY_ENTER: {
			int eid = va_arg(args, int);
			std::set<int> enterIds = va_arg(args, std::set<int>);
			sol::table tbl = sol::table::create_with(luaPtr->lua_state());
			for (int eeid : enterIds) {
				tbl.add(eeid);
			}
			sol::function funcObj = sceneScriptObj["after_entity_enter"];
			LuaPlugin::callLuaFunc(funcObj, sceneScriptObj, eid, tbl);
			break;
		}
		case SceneScriptEvent::AFTER_ENTITY_LEAVE: {
			int eid = va_arg(args, int);
			std::set<int> leaveIds = va_arg(args, std::set<int>);
			sol::table tbl = sol::table::create_with(luaPtr->lua_state());
			for (int leid : leaveIds) {
				tbl.add(leid);
			}
			sol::function funcObj = sceneScriptObj["after_entity_leave"];
			LuaPlugin::callLuaFunc(funcObj, sceneScriptObj, eid, tbl);
			break;
		}
		case SceneScriptEvent::AFTER_ENTITY_MOVE: {
			int eid = va_arg(args, int);
			std::set<int> enterIds = va_arg(args, std::set<int>);
			sol::table enterTbl = sol::table::create_with(luaPtr->lua_state());
			for (int eeid : enterIds) {
				enterTbl.add(eeid);
			}

			std::set<int> leaveIds = va_arg(args, std::set<int>);
			sol::table leaveTbl = sol::table::create_with(luaPtr->lua_state());
			for (int leid : leaveIds) {
				leaveTbl.add(leid);
			}

			sol::function funcObj = sceneScriptObj["after_entity_move"];
			LuaPlugin::callLuaFunc(funcObj, sceneScriptObj, eid, enterTbl, leaveTbl);
			break;
		}
		default:
			LOG_ERROR("not impl call script func%d", scriptEvent);
			break;
	}
	va_end(args);
}

sol::object LuaScene::createScene(int sceneId, sol::table script, sol::this_state s) {
	sol::state_view lua(s);
	GameScene* gameScene = SceneMgr::getSceneMgr()->createScene(sceneId);
	std::vector<int> regInfo = LuaRegistryObj::addRegistryObj(script);
	gameScene->bindLuaScriptObject(regInfo[0], regInfo[1], callSceneScripFunc);
	// °ó¶¨Ö¸Õë
	return sol::make_object(lua, gameScene);
}

static void destroyScene(int sceneUid, sol::this_state s) {
	GameScene* gameScene = SceneMgr::getSceneMgr()->getScene(sceneUid);
	if (gameScene == NULL) {
		LOG_ERROR("destory scene error, not found scene, scene_uid:%d", sceneUid);
		return;
	}
	LuaRegistryObj::removeRegistryObj(gameScene->getLuaObjId());
	SceneMgr::getSceneMgr()->destroyScene(sceneUid);
}

static sol::table findPath(GameScene* gameScene, sol::table startPos, sol::table endPos, sol::this_state s) {
	float startPosArr[3]{startPos[1], startPos[2], startPos[3]};
	float endPosArr[3]{ endPos[1], endPos[2], endPos[3] };
	std::vector<float> path;
	gameScene->findPath(startPosArr, endPosArr, &path);
	if (path.empty()) {
		return sol::nil;
	}

	sol::table pathTbl = sol::table::create_with(s.lua_state());
	for (auto iter = path.begin(); iter != path.end(); iter += 3) {
		sol::table pos = sol::table::create_with(s.lua_state());
		for (int i = 0; i < 3; i++) {
			pos[i + 1] = sol::make_object(s, *(iter + i));
		}
		pathTbl.add(pos);
	}
	return pathTbl;
}

static void bindScene(std::shared_ptr<sol::state> lua) {
	//sol::usertype<GameScene> gameScene_type = lua->new_usertype<GameScene>("GameScene",
	//	// 3 constructors
	//	sol::constructors<GameScene(int, int)>());
	// typical member function that returns a variable
	sol::usertype<GameScene> gameScene_type = lua->new_usertype<GameScene>("GameScene");
	gameScene_type["loadNavMesh"] = &GameScene::loadNavMesh;
	// typical member function
	gameScene_type["findPath"] = &findPath;
	gameScene_type["getSceneUid"] = &GameScene::getSceneUid;

	gameScene_type["createPlayer"] = &GameScene::createPlayer;
	gameScene_type["createNpc"] = &GameScene::createNpc;
	gameScene_type["removeEntity"] = &GameScene::removeEntity;
	gameScene_type["onEntityEnter"] = &GameScene::onEntityEnter;
}

static void bindPlayer(std::shared_ptr<sol::state> lua) {
	sol::usertype<PlayerEntity> playerType = lua->new_usertype<PlayerEntity>("GamePlayer");
	playerType["setScriptObj"] = &PlayerEntity::setScriptObj;
	playerType["getConnId"] = &PlayerEntity::getConnId;
	playerType["getEntityId"] = &SceneEntity::getEntityId;
	//void(GamePlayer::*sendToClient)(int, const char*, int) = &GamePlayer::sendToClient;
	playerType["sendToClient"] = sol::resolve<void(int, const char*, int)>(&PlayerEntity::sendToClient);   //sendToClient;
	playerType["sendToSight"] = sol::resolve<void(int, const char*, int)>(&SceneEntity::broadcastMsgToSight);
	playerType["x"] = sol::property(&PlayerEntity::getX);
	playerType["y"] = sol::property(&PlayerEntity::getY);
	playerType["move_speed"] = sol::property(&PlayerEntity::getMoveSpeed, &PlayerEntity::setMoveSpeed);
	playerType["setConnId"] = &PlayerEntity::setConnId;
}

static void bindNpc(std::shared_ptr<sol::state> lua) {
	sol::usertype<NpcEntity> npcType = lua->new_usertype<NpcEntity>("GameNpc");
	npcType["getEntityId"] = &SceneEntity::getEntityId;
	npcType["x"] = sol::property(&NpcEntity::getX);
	npcType["y"] = sol::property(&NpcEntity::getY);
	npcType["move_speed"] = sol::property(&NpcEntity::getMoveSpeed, &NpcEntity::setMoveSpeed);
	npcType["moveTo"] = &NpcEntity::moveTo;
	npcType["sendToSight"] = sol::resolve<void(int, const char*, int)>(&SceneEntity::broadcastMsgToSight);
}

void LuaScene::bindLuaScene(std::shared_ptr<sol::state> lua) {
	sol::table sceneMgr = lua->create_named_table("SceneMgr");
	sceneMgr["createScene"] = &LuaScene::createScene;
	sceneMgr["destroyScene"] = &destroyScene;

	bindScene(lua);
	bindPlayer(lua);
	bindNpc(lua);
}
