require("base.object")
require("util.logger")

clsMultiIndexElem = clsObject:Inherit("clsMultiIndexElem")

function clsMultiIndexElem:__init__(index_attr_names)
    self._index_attr_names = {}
    for _,attr_name in ipairs(index_attr_names or {}) do
        self._index_attr_names[attr_name] = true
    end
    self._container = nil
end

function clsMultiIndexElem:set_container(container)
    self._container = container
end

function clsMultiIndexElem:change_index_attr_value(attr_name, attr_val)
    if self._index_attr_names[attr_name] == nil then
        logger.logError("not index attr name, %s", attr_name)
        return
    end
    local old_attr_val = self[attr_name]
    self[attr_name] = attr_val
    self._container:on_elem_change_attr(self, attr_name, old_attr_val, attr_val)
end
