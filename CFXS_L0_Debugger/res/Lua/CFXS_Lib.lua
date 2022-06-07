-- CFXS_Lib.lua
function table.copy(t)
    local u = {}
    for k, v in pairs(t) do
        u[k] = v
    end
    return u
end

_G.addrof = _G.__SymbolNameToAddress -- C binding
_G.sizeof = function(v)
    if type(v) == "string" then
        return _G.__SymbolNameToSize(v) -- C binding
    else
        return v._sizeof
    end
end
    
-- _G.__Print C binding
-- _G.ReadMem8 C binding
-- _G.ReadMem16 C binding
-- _G.ReadMem32 C binding
-- _G.ReadMem64 C binding

_G.printf = function(...) _G.__Print(string.format(...)) end

_G.ReadMemX = function(size, addr)
    if size == 1 then
        return ReadMem8(addr)
    elseif size == 2 then
        return ReadMem16(addr)
    elseif size == 4 then
        return ReadMem32(addr)
    elseif size == 8 then
        return ReadMem64(addr)
    else
        return nil
    end
end

_G.ReadStruct = function(struct, addr)
    local s = setmetatable({}, {__index = struct})
    for i, v in pairs(struct._T) do
        if ((string.sub(i, 1, 1) ~= "_") and (string.sub(i, 1, 2) ~= "__")) then
            if(v.ignore == nil) then
                s[i] = ReadMemX(sizeof(v.type), addr + v.offset)
            end
        end
    end
    return s
end

_G.CreateStruct = function(structDesc, pack)
    local struct = {}
    local offsetVal = 0
    for i, v in pairs(structDesc) do
        v.type = v[1]
        v.field = v[2]
        if v.type == nil or v.type._T == nil then
            error("CreateStruct: Invalid type for "..v.field.." - "..tostring(v.type).."/"..(v.type and tostring(v.type._T) or ""))
        end
        local size = sizeof(v.type._T)
        if pack and pack or size == 2 or size == 4 or size == 8 then
            while(offsetVal % size ~= 0) do
                offsetVal = offsetVal + 1
            end
        end
        struct[v.field] = {offset = offsetVal, type = v.type._T, ignore = v.ignore}
        offsetVal = offsetVal + size
    end
    struct._sizeof = offsetVal
    return struct
end

-------------------------------
-- Types
_G.uint8_t = {_T = {_sizeof = 1}}
_G.uint16_t = {_T = {_sizeof = 2}}
_G.uint32_t = {_T = {_sizeof = 4}}
_G.uint64_t = {_T = {_sizeof = 8}}
_G.int = {_T = {_sizeof = 4, signed=true}}
_G.bool = uint8_t
_G.size_t = uint32_t
_G.void_ptr = size_t

_G.std_vector = {
    _T = CreateStruct({
         {void_ptr, "m_First"},
         {void_ptr, "m_Last"},
         {void_ptr, "m_End"},
    })
}

std_vector.template = function(elemSize)
    local self = setmetatable({}, {__index = std_vector})
    self._ElemSize = elemSize
    return self
end

function std_vector:init(addr)
    return ReadStruct(self, addr)
end

function std_vector:size()
    return (self.m_Last - self.m_First) / self._ElemSize
end

function std_vector:offset(idx)
    return self._ElemSize * idx
end

function std_vector:at(idx)
    return ReadMemX(self._ElemSize, self.m_First + idx * self._ElemSize);
end

function std_vector:list_iterator()
    local i = 0
    local n = self:size()
    return function()
        i = i + 1
        if i <= n then return self:at(i - 1) end
    end
end
