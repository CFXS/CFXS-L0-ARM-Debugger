local script = "\n" .. loadfile("ddram.iar")()

local strings = {}
local functions = {}

---------------------------------------------------------------------------------------
-- find all strings
for k in script:gmatch("\".-[^\\]\"") do
    k = k:gsub("([^%w])", "%%%1")
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
-- find all functions
for k in script:gmatch("[%s]-([%w_]-%s-%(.-%)[%s\n]-{.-})") do
    local funcEntry = {
        name = k:match("([%w_]+)%s-%(.-%)[%s\n]-{.-}"),
        params = k:match("[%w_]-%s-%((.-)%)[%s\n]-{.-}"),
        code = k:match("[%w_]-%s-%(.-%)[%s\n]-{(.-)}"):gsub("^%s*", "")
    }
    table.insert(functions, funcEntry)

    print("name:", funcEntry.name)
    print("params:", funcEntry.params)
    print("body:\n" .. funcEntry.body)
end
---------------------------------------------------------------------------------------
