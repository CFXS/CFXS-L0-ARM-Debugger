-- CFXS_Lib.lua
function table.copy(t)
    local u = {}
    for k, v in pairs(t) do
        u[k] = v
    end
    return setmetatable(u, getmetatable(t))
end

_G.addrof = _G.__SymbolNameToAddress -- C binding
_G.sizeof = function(v)
    if type(v) == "string" then
        return _G.__SymbolNameToSize(v) -- C binding
    else
        return v._T and v._T._sizeof or v._sizeof
    end
end
_G.is_pointer = function(v)
    return v._T._referenceTo ~= nil
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

_G.ReadStruct = function(struct, addr, deref)
    if deref and struct._T._referenceTo ~= nil then -- pointer type
        struct = struct._T._referenceTo
    end

    local s = setmetatable({}, {__index = struct})
    for i, v in pairs(struct._T) do
        if ((string.sub(i, 1, 1) ~= "_") and (string.sub(i, 1, 2) ~= "__")) then
            if v.ignore == nil then
                if v.type._isStruct then
                    s[i] = ReadStruct(v.structType, addr + v.offset)
                else
                    s[i] = ReadMemX(sizeof(v.type), addr + v.offset)
                end
            end
        end
    end
    return s._Reassign and s._Reassign(s) or s
end

_G.deref = function(v)
    return ReadStruct(v, v.ptr, true)
end

_G.derefType = function(v)
    return v._T._referenceTo
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
        local size = sizeof(v.type)
        if pack and pack or size == 2 or size == 4 or size == 8 then
            while(offsetVal % size ~= 0) do
                offsetVal = offsetVal + 1
            end
        end
        struct[v.field] = {offset = offsetVal, type = v.type._T, structType = v.type, ignore = v.ignore}
        offsetVal = offsetVal + size
    end
    struct._sizeof = offsetVal
    struct._isStruct = true
    return struct
end

-------------------------------
-- Types
_G.REG_WIDTH = 4 -- 32bit

_G.uint8_t = {_T = {_sizeof = 1, _isPrimitive = true}}
_G.uint16_t = {_T = {_sizeof = 2, _isPrimitive = true}}
_G.uint32_t = {_T = {_sizeof = 4, _isPrimitive = true}}
_G.uint64_t = {_T = {_sizeof = 8, _isPrimitive = true}}
_G.int = {_T = {_sizeof = 4, _signed=true, _isPrimitive = true}}
_G.bool = uint8_t
_G.size_t = {_T = {_sizeof = REG_WIDTH, _isPrimitive = true}}
_G.void_ptr = {_T = {_sizeof = REG_WIDTH, _isPrimitive = true}}
_G.void = nil

_G.pointer_to = function(t)
    if(t == nil) then
        return void_ptr;
    else
        local ptrType = {_T = {["ptr"] = {offset = 0, type = void_ptr}, _sizeof = REG_WIDTH, _referenceTo = t, _isPrimitive = false}}
        return ptrType
    end
end

------------------------
_G.std_vector = {
    _T = CreateStruct({
         {void_ptr, "m_First"},
         {void_ptr, "m_Last"},
         {void_ptr, "m_End"},
    })
}

std_vector.template = function(elemType)
    local self = setmetatable({}, {__index = std_vector})
    self._ElementType = elemType
    self._Read = function(s, addr)
        ReadStruct(s, addr)
    end
    return self
end

function std_vector:init(addr)
    return ReadStruct(self, addr)
end

function std_vector:size()
    return (self.m_Last - self.m_First) / sizeof(self._ElementType)
end

function std_vector:offset(idx)
    return voffs(self._ElementType) * idx
end

function std_vector:at(idx)
    if self._ElementType._T._isPrimitive == true then
        return ReadMemX(sizeof(self._ElementType), self.m_First + idx * sizeof(self._ElementType));
    else
        return ReadStruct(self._ElementType, self.m_First + idx * sizeof(self._ElementType))
    end
end

function std_vector:list_iterator()
    local i = 0
    local n = self:size()
    return function()
        i = i + 1
        if i <= n then return self:at(i - 1) end
    end
end
