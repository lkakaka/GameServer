
CfgScene = {}

CfgScene._DATA = {
    [1] = {1,"测试场景1",1,"all_tiles_navmesh.bin",},
    [101] = {101,"测试场景2",2,"all_tiles_navmesh.bin",},
    [102] = {102,"测试场景3",2,"all_tiles_navmesh.bin",},
    [103] = {103,"测试场景4",2,"all_tiles_navmesh.bin",},
}

CfgScene.Index_SceneType = "Index_SceneType"
CfgScene._Index_SceneType_DATA = {
    [1] = {1,},
    [2] = {101,102,103,},
}

CfgScene._Index_DATA = {
    [CfgScene.Index_SceneType] = CfgScene._Index_SceneType_DATA,
}


function CfgScene.find_by_index(index_name, index_val)
    local index_data = CfgScene._Index_DATA[index_name]
    if index_data == nil then
        error(string.format("table Scene index {0} not exist!!!", index_name))
    end

    local rows = {}
    for _,id in ipairs(index_data[index_val] or {}) do
        table.insert(rows, CfgScene.find(id))
    end
    return rows
end

function CfgScene.find(id)
    local data = CfgScene._DATA[id]
    local _data = {
            scene_id = data[1],
            scene_name = data[2],
            scene_type = data[3],
            nav_name = data[4],
        }
    return _data
end
