import cfg_scene

row = cfg_scene.find(1)
print(row.scene_id, row.scene_name)
print(row)

rows = cfg_scene.find_by_index(cfg_scene.Index_SceneType, 2)
print(rows)

