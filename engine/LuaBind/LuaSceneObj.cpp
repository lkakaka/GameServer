#include "LuaSceneObj.h"
#include "Logger.h"
#include "MyBuffer.h"
#include "ServiceType.h"
#include "ServiceInfo.h"
#include "GameScene.h"
#include "SceneMgr.h"
#include "LuaPlugin.h"

static sol::object createScene(int sceneId, sol::table script, sol::this_state s) {
	GameScene* gameScene = SceneMgr::getSceneMgr()->createScene(sceneId, &script);
	sol::state_view lua(s);
	//lua["gameScene1"] = *gameScene;
	return sol::make_object(lua, *gameScene);
}

static void destroyScene(int sceneUid, sol::this_state s) {
	SceneMgr::getSceneMgr()->destroyScene(sceneUid);
}

void LuaSceneObj::bindLuaSceneObj(std::shared_ptr<sol::state> lua) {
	sol::table sceneMgr = lua->create_named_table("SceneMgr");
	sceneMgr["createScene"] = &createScene;
	sceneMgr["destroyScene"] = &destroyScene;

	sol::usertype<GameScene> gameScene_type = lua->new_usertype<GameScene>("GameScene",
		// 3 constructors
		sol::constructors<GameScene(int, int, void*)>());

	// typical member function that returns a variable
	gameScene_type["loadNavMesh"] = &GameScene::loadNavMesh;
	// typical member function
	gameScene_type["findPath"] = &GameScene::findPath;
	gameScene_type["getSceneUid"] = &GameScene::getSceneUid;

	gameScene_type["createPlayer"] = &GameScene::createPlayer;
	gameScene_type["removeActor"] = &GameScene::removeActor;

	sol::usertype<GamePlayer> gamePlayer_type = lua->new_usertype<GamePlayer>("GamePlayer");
	gamePlayer_type["setScriptObj"] = &GamePlayer::setScriptObj;
	gamePlayer_type["getConnId"] = &GamePlayer::getConnId;
	gamePlayer_type["getActorId"] = &GameActor::getActorId;
	void(GamePlayer::*sendToClient)(int, const char*, int) = &GamePlayer::sendToClient;
	gamePlayer_type["sendToClient"] = sendToClient;
	
	// gets or set the value using member variable syntax
	//gameScene_type["hp"] = sol::property(&player::get_hp, &player::set_hp);
}


