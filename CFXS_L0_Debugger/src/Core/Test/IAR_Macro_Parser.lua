--[[
TODO:
 - do x; while(y);
 - do {x;} while(y);
 - for {}
]] --
local script = ""
local path = "TestMacro.mac"

local io = require("io")
local f = io.open(path, "r")
for line in io.lines(path) do
    script = script .. line .. "\n" --
end

local strings = {}
local functions = {}

---------------------------------------------------------------------------------------
-- find all strings
for k in script:gmatch("\".-[^\\]\"") do
    k = k:gsub("[%(%)%.%%%+%-%*%?%[%]%^%$]", "%%%0")
    table.insert(strings, k)
end

for i, v in pairs(strings) do
    script = script:gsub(v, "`STRING_" .. i .. "`") --
end
---------------------------------------------------------------------------------------
-- remove all comments and multiple newlines
script = script:gsub("%s-//.-[\n$]", "\n") -- remove single line comments
script = script:gsub("/%*.-%*/", "") -------- remove multi line comments
script = script:gsub("\n", ""); ------------- remove all newlines
script = script:gsub(";%s-(%S)", ";\n%1") --- remove spaces between ; and next non space char, replace with one newline
---------------------------------------------------------------------------------------
-- place () around __message "str" param
script = script:gsub("%f[%w_]__message%f[^%w_](.-);", "__message(%1);")
-- replace "__var" with "local"
script = script:gsub("%f[%w_]__var%f[^%w_]", "local")
-- replace +=/++ with x = x + y (Lua does not support +=/++)
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*+=(.-);", "%1 = %1 + (%2);") ----- +=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*-=(.-);", "%1 = %1 - (%2);") ----- -=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s**=(.-);", "%1 = %1 * (%2);") ----- *=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*/=(.-);", "%1 = %1 / (%2);") ----- /=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*%%=(.-);", "%1 = %1 %% (%2);") --- %=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*%^=(.-);", "%1 = %1 ^ (%2);") ---- ^=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*&=(.-);", "%1 = %1 & (%2);") ----- &=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*|=(.-);", "%1 = %1 | (%2);") ----- |=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*>>=(.-);", "%1 = %1 >> (%2);") --- >>=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*<<=(.-);", "%1 = %1 << (%2);") --- <<=
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*%+%+", "%1 = %1 + 1") -- ++
script = script:gsub("%f[%w_]([%w_]+)%f[^%w_]%s*%-%-", "%1 = %1 - 1") -- --
--
script = script:gsub("!=", "~=")
script = script:gsub("&&", "and")
script = script:gsub("||", "or")
script = script:gsub("!([%w_]-%b())", "((%1) == 0)")
script = script:gsub("!([%w_]+)", "((%1) == 0)")
-- remove multiple spaces
script = script:gsub(" +", " ")
-- replace "else if" with "elseif"
script = script:gsub("else if", "elseif")
-- replace registers with get function calls
script = script:gsub("#([%w_]*)", "CPU.Registers.%1")
-- while
for depth = 1, 64 do -- max depth 64
    script = script:gsub("%f[%w_]while%s-(%b())%s-;",
                         "while __is_true(%1) do end") -- single line no body
    script = script:gsub("%f[%w_]while%s-(%b())%s-([^%b{}]-)%s-;",
                         "while __is_true(%1) do %2 end") -- single line body
    script = script:gsub("%f[%w_]while%s-(%b())%s-(%b{})",
                         "while __is_true(%1) do `REMOVE_RIGHT`%2`REMOVE_LEFT` end") -- bracket body
    script = script:gsub("`REMOVE_RIGHT`.", ""); -- remove { from %b{} match
    script = script:gsub(".`REMOVE_LEFT`", ""); -- remove } from %b{} match
end

-- if prep
for depth = 1, 64 do -- max depth 64
    script = script:gsub("%f[%w_]if%f[^%w_]%s-(%b())%s-([^%b{}]-%s-;)",
                         "if(%1){%2}") -- single line body - add {}
    script = script:gsub("%f[%w_]else%f[^%w_]%s-([^%b{}]-%s-;)", "else{%1}") -- single line body - add {}
    script = script:gsub("%f[%w_]elseif%f[^%w_]%s-(%b())%s-([^%b{}]-%s-;)",
                         "elseif(%1){%2}") -- single line body - add {}
end
-- unsigned
script = script:gsub("([%d%x]+)[uU][lL]([^%w_])", "%1%2")
script = script:gsub("([%d%x]+)[uU]([^%w_])", "%1%2")
-- address
script = script:gsub("&([%w_]+)", "__address_of(\"%1\")")
-- uninitialized locals
script = script:gsub("local ([%w_]-);", "local %1 = 0;")
---------------------------------------------------------------------------------------
-- find all functions  
for k in script:gmatch("[%s]-[%w_]-%s-%(.-%)[%s\n]-%b{}") do
    local funcEntry = {
        sourcePattern = k:gsub("[%(%)%.%%%+%-%*%?%[%]%^%$]", "%%%0"),
        name = k:match("[%s]-([%w_]+)%s-%(.-%)[%s\n]-%b{}"),
        params = k:match("[%s]-[%w_]-%s-%((.-)%)[%s\n]-%b{}"),
        code = k:match("[%s]-[%w_]-%s-%(.-%)[%s\n]-(%b{})"):gsub("^%s*", "")
    }

    if (funcEntry.name == "if" or funcEntry.name == "while" or funcEntry.name ==
        "for") then
        error("if/for/while not allowed outside of a function")
        script = ""
        break
    end

    funcEntry.code = "function " .. funcEntry.name .. "(" .. funcEntry.params ..
                         ")" .. funcEntry.code

    while funcEntry.code:find("%b{}") do
        funcEntry.code = funcEntry.code:gsub("%b{}", "do\n%0\nend")
        funcEntry.code = funcEntry.code:gsub("do%s-{", "do\n")
        funcEntry.code = funcEntry.code:gsub("}%s-end", "end\n")
    end

    funcEntry.code = funcEntry.code:gsub("(function.-)%)%s-do", "%1)")
    funcEntry.code = funcEntry.code:gsub("(%Wif.-[%s%)])%s-do", "%1then")
    funcEntry.code = funcEntry.code:gsub("end[\n%s]-else%s-do", "else")
    funcEntry.code = funcEntry.code:gsub("end[\n%s]-elseif", "elseif")
    funcEntry.code = funcEntry.code:gsub("(%Welseif.-[%s%)])%s-do", "%1then")

    for is, s in pairs(strings) do
        s = s:gsub("%%", "%%%%")
        s = s:gsub(",", "__temp_com_temp__");
        funcEntry.code = funcEntry.code:gsub("`STRING_" .. is .. "`", s);
    end

    if funcEntry.code:find("%s-__message%b()") then
        local messages = {}
        for msg in funcEntry.code:gmatch("%s-(__message%b())") do
            local content = {}
            local args = msg:match("%b()")
            args = string.sub(args, 2, string.len(args) - 1);
            local found = false
            local msgContent = args:gmatch("([^,]+)")

            for arg in msgContent do
                found = true
                if arg:find("[%w_%(%)].-:%%%w+") then
                    -- param
                    local var = arg:match("([%w_%(%)]-):%%%w+")
                    local disp = arg:match("[%w_%(%)]-:(%%%w+)")
                    local fmt = "string.format(\"" .. disp .. "\", " .. var ..
                                    ")"
                    table.insert(content, fmt)
                else
                    -- str
                    table.insert(content, arg)
                end
            end

            local params = table.concat(content, "..")
            table.insert(messages, {
                str = msg:gsub("[%(%)%.%%%+%-%*%?%[%]%^%$]", "%%%0"),
                par = params:gsub("%%", "%%%%")
            })
        end

        for im, m in ipairs(messages) do
            funcEntry.code = funcEntry.code:gsub(m.str,
                                                 "__message(" .. m.par .. ")")
        end
    end

    funcEntry.code = funcEntry.code:gsub("__temp_com_temp__", ",")
    funcEntry.code = funcEntry.code:gsub("%%", "%%%%")

    table.insert(functions, funcEntry)
end

for i, v in ipairs(functions) do
    script = script:gsub(v.sourcePattern, v.code .. "\n\n") --
end
---------------------------------------------------------------------------------------

script = script:gsub("%%%-", "-")
script = script:gsub("%%%.", ".")
script = script:gsub("%%%(", "(")
script = script:gsub("%%%)", ")")
script = script:gsub("%%%?", "?")
print(script)
