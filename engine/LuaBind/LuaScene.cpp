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
		case SceneScriptEvent::AFTER_ACTOR_ENTER: {
			int actorId = va_arg(args, int);
			std::set<int> enterActors = va_arg(args, std::set<int>);
			sol::table tbl = sol::table::create_with(luaPtr->lua_state());
			for (int actorId : enterActors) {
				tbl.add(actorId);
			}
			sol::function funcObj = sceneScriptObj["after_actor_enter"];
			LuaPlugin::callLuaFunc(funcObj, sceneScriptObj, actorId, tbl);
			break;
		}
		case SceneScriptEvent::AFTER_ACTOR_LEAVE: {
			int actorId = va_arg(args, int);
			std::set<int> leaveActors = va_arg(args, std::set<int>);
			sol::table tbl = sol::table::create_with(luaPtr->lua_state());
			for (int actorId : leaveActors) {
				tbl.add(actorId);
			}
			sol::function funcObj = sceneScriptObj["after_actor_leave"];
			LuaPlugin::callLuaFunc(funcObj, sceneScriptObj, actorId, tbl);
			break;
		}
		case SceneScriptEvent::AFTER_ACTOR_MOVE: {
			int actorId = va_arg(args, int);
			std::set<int> enterActors = va_arg(args, std::set<int>);
			sol::table enterTbl = sol::table::create_with(luaPtr->lua_state());
			for (int actorId : enterActors) {
				enterTbl.add(actorId);
			}

			std::set<int> leaveActors = va_arg(args, std::set<int>);
			sol::table leaveTbl = sol::table::create_with(luaPtr->lua_state());
			for (int actorId : leaveActors) {
				leaveTbl.add(actorId);
			}

			sol::function funcObj = sceneScriptObj["after_actor_move"];
			LuaPlugin::callLuaFunc(funcObj, sceneScriptObj, actorId, enterTbl, leaveTbl);
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
	// ��ָ��
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
	gameScene_type["removeActor"] = &GameScene::removeActor;
	gameScene_type["onActorEnter"] = &GameScene::onActorEnter;
}

static void bindPlayer(std::shared_ptr<sol::state> lua) {
	sol::usertype<GamePlayer> gamePlayer_type = lua->new_usertype<GamePlayer>("GamePlayer");
	gamePlayer_type["setScriptObj"] = &GamePlayer::setScriptObj;
	gamePlayer_type["getConnId"] = &GamePlayer::getConnId;
	gamePlayer_type["getActorId"] = &GameActor::getActorId;
	//void(GamePlayer::*sendToClient)(int, const char*, int) = &GamePlayer::sendToClient;
	gamePlayer_type["sendToClient"] = sol::resolve<void(int, const char*, int)>(&GamePlayer::sendToClient);   //sendToClient;
	gamePlayer_type["sendToSight"] = sol::resolve<void(int, const char*, int)>(&GameActor::broadcastMsgToSight);
	gamePlayer_type["x"] = sol::property(&GamePlayer::getX);
	gamePlayer_type["y"] = sol::property(&GamePlayer::getY);
	gamePlayer_type["move_speed"] = sol::property(&GamePlayer::getMoveSpeed, &GamePlayer::setMoveSpeed);
}

static void bindNpc(std::shared_ptr<sol::state> lua) {
	sol::usertype<GameNpc> gameNpc_type = lua->new_usertype<GameNpc>("GameNpc");
	gameNpc_type["getActorId"] = &GameActor::getActorId;
	gameNpc_type["x"] = sol::property(&GameNpc::getX);
	gameNpc_type["y"] = sol::property(&GameNpc::getY);
	gameNpc_type["move_speed"] = sol::property(&GameNpc::getMoveSpeed, &GameNpc::setMoveSpeed);
	gameNpc_type["moveTo"] = &GameNpc::moveTo;
	gameNpc_type["sendToSight"] = sol::resolve<void(int, const char*, int)>(&GameActor::broadcastMsgToSight);
}

void LuaScene::bindLuaScene(std::shared_ptr<sol::state> lua) {
	sol::table sceneMgr = lua->create_named_table("SceneMgr");
	sceneMgr["createScene"] = &LuaScene::createScene;
	sceneMgr["destroyScene"] = &destroyScene;

	bindScene(lua);
	bindPlayer(lua);
	bindNpc(lua);
}


