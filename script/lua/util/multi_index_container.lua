require("base.object")
require("util.logger")

clsMultIndexContainer = clsObject:Inherit("clsMultIndexContainer")

function clsMultIndexContainer:__init__(attrNames)
    self._container = {}
    self.attrNames = attrNames
    for _, attrName in ipairs(attrNames) do
        self._container[attrName] = {}
    end
end

function clsMultIndexContainer:addElem(elem)
    elem:set_container(self)
    for _, attrName in ipairs(self.attrNames) do
        local attrVal = elem[attrName]
        if attrVal ~= nil then
            local con = self._container[attrName]
            con[attrVal] = con[attrVal] or {}
            if self._container[attrName][attrVal][elem] ~= nil then
                logger.logError("repeat add elem, attrName:%s, attrVal:%s", attrName, attrVal)
            end
            con[attrVal][elem] = true
        else
            error(string.format("elem's %s is nil", attrName))
        end
    end
end

function clsMultIndexContainer:removeElem(elem)
    for _, attrName in ipairs(self.attrNames) do
        local attrVal = elem[attrName]
        if attrVal ~= nil then
            self._container[attrName][attrVal][elem] = nil
        else
            error(string.format("elem's %s is nil", attrName))
        end
    end
    elem:set_container(nil)
end

function clsMultIndexContainer:on_elem_change_attr(elem, attr_name, old_attr_val, attr_val)
    local con = self._container[attr_name]
    con[old_attr_val][elem] = nil
    con[attr_val] = con[attr_val] or {}
    con[attr_val][elem] = true
end

function clsMultIndexContainer:getElems(attrName, attrVal)
    local elems = {}
    for elem, _ in pairs(self._container[attrName][attrVal] or {}) do
        table.insert(elems, elem)
    end
    return elems
end

function clsMultIndexContainer:getOneElem(attrName, attrVal)
    local lst = self._container[attrName][attrVal]
    if lst == nil then return nil end
    local elem,_ = next(lst)
    return elem
end
