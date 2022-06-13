-- CFXS_Lib.lua
function CopyTable(t)
    local orig_type = type(t)
    local copy
    if orig_type == "table" then
        copy = {}
        for orig_key, orig_value in next, t, nil do
            copy[CopyTable(orig_key)] = CopyTable(orig_value)
        end
        setmetatable(copy, CopyTable(getmetatable(t)))
    else
        copy = t
    end
    return copy
end

_G.min = math.min
_G.max = math.max
_G.floor = math.floor
_G.ceil = math.ceil
_G.round = function(x)
    return math.floor(x + 0.5)
end

_G.PointerString = function(v)
    return v ~= 0 and string.format("0x%08X", v) or "nullptr"
end

_G.addressof = function(v, member)
    if type(v) == "string" then
        return _G.__SymbolNameToAddress(v) -- C binding
    else
        if member then
            return v["_pAddr_"..member]
        else
            return v.ptr and v.ptr or v._pAddr
        end
    end
end

_G.sizeof = function(v, member)
    if type(v) == "string" then
        return _G.__SymbolNameToSize(v) -- C binding
    else
        if member then
            return v._T[member].type._sizeof
        else
            return v._T and v._T._sizeof or v._sizeof
        end
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

_G.using_namespace = function(ns)
    for i, v in pairs(ns) do
        if _G[i] ~= nil then
            error(string.format("Namespace collision - [%s]: %s", i, tostring(v)));
        else
            _G[i] = v
        end
    end
end

_G.print = function(...) _G.__Print(string.format(...)) end

_G.TableDump = function(t, indent)
    if indent == nil then indent = 0 end

    for i, v in pairs(t) do
        print("%s[%s]: %s %s", string.rep(" ", indent), tostring(i), tostring(v), type(v) == "number" and string.format("(0x%X)", v) or "" )
        if type(v) == "table" then
            TableDump(v, indent + 4)
        end
    end
end

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

_G.ReadVar = function(name)
    local ret = ReadMemX(sizeof(name), addressof(name))
    if ret == nil then
        error("ReadVar(\""..name.."\") does not fit a primitive type")
    end
    return ret
end

_G.ReadStruct = function(struct, addr, deref)
    if deref and struct._T._referenceTo ~= nil then -- pointer type
        struct = struct._T._referenceTo
    end

    local s = setmetatable({}, {__index = struct})
    s._pAddr = addr -- physical base address of struct
    if s._T._isPrimitive then
        s.value = ReadMemX(sizeof(s), addr)
    else
        for i, v in pairs(struct._T) do
            if ((string.sub(i, 1, 1) ~= "_") and (string.sub(i, 1, 2) ~= "__")) then
                if v.ignore == nil then
                    if v.type._isStruct then
                        if _G.__debug_readstruct then
                            print("Read STRUCT %s --------------", i)
                        end
                        s[i] = ReadStruct(v.structType, addr + v.offset)
                        s["_pAddr_"..i] = addr + v.offset -- physical base address of struct as member
                    else
                        if _G.__debug_readstruct then
                            print("Read %s", i)
                        end
                        local val = ReadMemX(sizeof(v.type), addr + v.offset)

                        if v.type._isBool then
                            -- DO NOT CONVERT TO AND/OR - true/false breaks the expression and always returns true somehow
                            if val == 0 then
                                s[i] = false
                            else
                                s[i] = true
                            end
                        else
                            s[i] = val
                        end
                        -- (TYPED READS) s[i] = {_pAddr = addr + v.offset, type = v.type, value = ReadMemX(sizeof(v.type), addr + v.offset)}
                        s["_pAddr_"..i] = addr + v.offset -- physical address of read field
                    end
                end
            end
        end
    end

    return s
end

_G.ReadString = function(addr, maxLen)
    local str = ""
    for i = 0, maxLen-1 do
        local c = ReadMem8(addr + i)
        if c == 0 then
            break
        end
        str = str .. string.char(c)
    end
    return str
end

_G.deref = function(v)
    return ReadStruct(v, addressof(v), true)
end

_G.deref_type = function(v)
    return v._T._referenceTo
end

_G.AlignTo = function(x, a)
    return (x + a - 1) & ~(a - 1)
end

_G.advance = function(v, i)
    if i ~= nil and type(i) ~= "number" then
        error("advance by non-number")
    end
    if is_pointer(v) then
        if i == nil then
            i = 1
        end
        v._pAddr = v._pAddr + sizeof(v) * i
        v._T.ptr.offset = v._pAddr
    else
        error("advance non-pointer")
    end
end

local FirstMemberSize = function(s)
    for i, v in pairs(s._T) do
        if ((string.sub(i, 1, 1) ~= "_") and (string.sub(i, 1, 2) ~= "__")) then
            if v.offset == 0 then
                return v.type._sizeof
            end
        end
    end
    return 4
end

_G.CreateStruct = function(structDesc, pack, alignTo)
    local struct = {}
    local offsetVal = 0
    for i, v in pairs(structDesc) do
        v.type = v[1]
        v.field = v[2]
        if v.type == nil or v.type._T == nil then
            error("CreateStruct: Invalid type for "..v.field.." - "..tostring(v.type).."/"..(v.type and tostring(v.type._T) or ""))
        end
        local size = sizeof(v.type)
        if pack and pack or size == 1 or size == 2 or size == 4 or size == 8 then
            offsetVal = AlignTo(offsetVal, size)
        else
            offsetVal = AlignTo(offsetVal, FirstMemberSize(v.type))
        end
        struct[v.field] = {offset = offsetVal, type = v.type._T, structType = v.type, ignore = v.ignore}
        offsetVal = offsetVal + size
    end
    struct._sizeof = alignTo and AlignTo(offsetVal, alignTo) or offsetVal
    struct._isStruct = true
    return struct
end

_G.CreateType = function(struct, typename)
    return {_T = struct, _typename = typename}
end

-------------------------------
-- Types
_G.REG_WIDTH = 4 -- 32bit

_G.uint8_t = {_T = {_sizeof = 1, _isPrimitive = true}}
_G.uint16_t = {_T = {_sizeof = 2, _isPrimitive = true}}
_G.uint32_t = {_T = {_sizeof = 4, _isPrimitive = true}}
_G.uint64_t = {_T = {_sizeof = 8, _isPrimitive = true}}
_G.int = {_T = {_sizeof = 4, _signed=true, _isPrimitive = true}}
_G.bool = {_T = {_sizeof = 1, _isPrimitive = true, _isBool = true}}
_G.size_t = {_T = {_sizeof = REG_WIDTH, _isPrimitive = true}}
_G.void_ptr = {_T = {_sizeof = REG_WIDTH, _isPrimitive = true}}
_G.c_str = {_T = {_sizeof = REG_WIDTH, _isPrimitive = true, _isString = true}}
_G.float = {_T = {_sizeof = 4, _isPrimitive = true, _isFloat = true}}
_G.void = nil

_G.pointer_to = function(t, addr)
    if(t == nil) then
        return void_ptr;
    else
        local ptrType = {
            _T = {["ptr"] = {offset = addr and addr or 0, type = void_ptr}, _sizeof = REG_WIDTH, _referenceTo = t, _isPrimitive = false, _isStruct = true},
            _pAddr = addr
        }
        return ptrType
    end
end

------------------------
-- std::vector
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
    return self
end

function std_vector:init(addr)
    return ReadStruct(self, addr)
end

function std_vector:size()
    return (self.m_Last - self.m_First) / sizeof(self._ElementType)
end

function std_vector:empty()
    return self:size() == 0
end

function std_vector:offset(idx)
    return sizeof(self._ElementType) * idx
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
function std_vector:index_iterator()
    local i = 0
    local n = self:size()
    return function()
        i = i + 1
        if i <= n then
            return i - 1, self:at(i - 1)
        end
    end
end

------------------------
-- std::array
_G.std_array = {
    _T = CreateStruct({
    })
}

std_array.template = function(elemType, elemCount)
    local self = setmetatable({}, {__index = CopyTable(std_array)})
    self._ElementType = elemType
    self._ElementCount = elemCount
    self._T._sizeof = sizeof(elemType) * elemCount
    return self
end

function std_array:init(addr)
    self.m_First = addr
    return self
end

function std_array:size()
    return self._ElementCount
end

function std_array:offset(idx)
    return sizeof(self._ElementType) * idx
end

function std_array:at(idx)
    if not self.m_First then
        self.m_First = addressof(self)
    end

    if self._ElementType._T._isPrimitive == true then
        return ReadMemX(sizeof(self._ElementType), self.m_First + idx * sizeof(self._ElementType));
    else
        return ReadStruct(self._ElementType, self.m_First + idx * sizeof(self._ElementType))
    end
end

function std_array:list_iterator()
    local i = 0
    local n = self:size()
    return function()
        i = i + 1
        if i <= n then return self:at(i - 1) end
    end
end

function std_array:index_iterator(maxCount)
    local i = 0
    local n = maxCount and min(self:size(), maxCount) or self:size()
    return function()
        i = i + 1
        if i <= n then
            return i - 1, self:at(i - 1)
        end
    end
end





