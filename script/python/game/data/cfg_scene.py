
_Scene_DATA = {
    1: (1, '测试场景1', 1, 'all_tiles_navmesh.bin'),
    101: (101, '测试场景2', 2, 'all_tiles_navmesh.bin'),
    102: (102, '测试场景3', 2, 'all_tiles_navmesh.bin'),
    103: (103, '测试场景4', 2, 'all_tiles_navmesh.bin'),
}

Index_SceneType = "Index_SceneType"
_Index_SceneType_DATA = {
    1: (1,),
    2: (101, 102, 103),
}

_Index_DATA = {
    Index_SceneType: _Index_SceneType_DATA,
}


class _SceneRow:
    def __init__(self, row_data):
        self.__dict__["scene_id"] = row_data[0]
        self.__dict__["scene_name"] = row_data[1]
        self.__dict__["scene_type"] = row_data[2]
        self.__dict__["nav_name"] = row_data[3]

    def __repr__(self):
        data = {
            "scene_id": self.__dict__["scene_id"],
            "scene_name": self.__dict__["scene_name"],
            "scene_type": self.__dict__["scene_type"],
            "nav_name": self.__dict__["nav_name"],
        }
        return str(data)


def find_by_index(index_name, index_val):
    if index_name not in _Index_DATA:
        raise Exception("table Scene index {0} not exist!!!".format(index_name))
    index_data = _Index_DATA.get(index_name)
    rows = []
    for id in index_data.get(index_val, []):
        rows.append(find(id))
    return tuple(rows)


def find(id):
    data = _Scene_DATA.get(id)
    return _SceneRow(data)
