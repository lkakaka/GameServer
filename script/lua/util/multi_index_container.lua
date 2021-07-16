require("base.object")
require("util.logger")

clsMultIndexContainer = clsObject:Inherit("clsMultIndexContainer")

function clsMultIndexContainer:__init__(attrNames)
    self._container = {}
    self.attrNames = attrNames
    for i, attrName in ipairs(attrNames) do
        self._container[attrName] = {}
    end
end

function clsMultIndexContainer:isElemInTable(tab, elem)
    for i, v in ipairs(tab) do
        if v == elem then return true end
    end
    return false
end

function clsMultIndexContainer:removeElemFromTable(tab, elem)
    local pos = -1
    for i, v in ipairs(tab) do
        if v == elem then 
            pos = i
            break
        end
    end
    if pos > 0 then
        table.remove(tab, pos)
    end
end

function clsMultIndexContainer:addElem(elem)
    for i, attrName in ipairs(self.attrNames) do
        local attrVal = elem[attrName]
        if attrVal ~= nil then
            self._container[attrName][attrVal] = self._container[attrName][attrVal] or {}
            if self:isElemInTable(self._container[attrName][attrVal], elem) then
                logger.logError("repeat add elem, attrName:%s", attrVal)
            else
                table.insert(self._container[attrName][attrVal], elem)
            end 
        else
            print(elem.sceneUid)
            error(string.format("elem's %s is nil", attrName))
        end
    end
end

function clsMultIndexContainer:removeElem(elem)
    for i, attrName in ipairs(self.attrNames) do
        local attrVal = elem[attrName]
        if attrVal ~= nil then
            self:removeElemFromTable(self._container[attrName][attrVal], elem)
        else
            error(string.format("elem's %s is nil", attrName))
        end
    end
end

function clsMultIndexContainer:getElems(attrName, attrVal)
    return self._container[attrName][attrVal] or {}
end

function clsMultIndexContainer:getOneElem(attrName, attrVal)
    local lst = self._container[attrName][attrVal]
    if lst == nil or #lst == 0 then return nil end
    return lst[1]
end
