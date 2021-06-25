import cfg_scene


def test():
    scene_id = 1
    row = cfg_scene.find(scene_id)
    print(row.scene_id, row.scene_name)
    print(row)
    assert(row.scene_id == scene_id)

    scene_type = 2
    expect_row_count = 3
    rows = cfg_scene.find_by_index(cfg_scene.Index_SceneType, scene_type)
    print(rows)
    assert len(rows) == expect_row_count
    for row in rows:
        assert row.scene_type == scene_type

    print("test success")


if __name__ == "__main__":
    test()
