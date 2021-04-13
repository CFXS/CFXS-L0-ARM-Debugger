local script = "\n" .. loadfile("ddram.iar")()

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
-- script = script:gsub("\n", ""); ------------- remove all newlines
script = script:gsub(";%s-(%S)", ";\n%1") --- remove spaces between ; and next non space char, replace with one newline
---------------------------------------------------------------------------------------
-- replace "__var" with "local"
script = script:gsub("%f[%w_]__var%f[^%w_]", "local")
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*+=", "%1 = %1 + ") ----- +=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*-=", "%1 = %1 - ") ----- -=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s**=", "%1 = %1 * ") ----- *=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*/=", "%1 = %1 / ") ----- /=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*%%=", "%1 = %1 %% ") --- %=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*%^=", "%1 = %1 ^ ") ---- ^=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*&=", "%1 = %1 & ") ----- &=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*|=", "%1 = %1 | ") ----- |=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*>>=", "%1 = %1 >> ") --- >>=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*<<=", "%1 = %1 << ") --- <<=
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*++", "%1 = %1 + 1") ---- ++
script = script:gsub("%f[%w_](%w+)%f[^%w_]%s*%-%-", "%1 = %1 - 1") -- --
---------------------------------------------------------------------------------------
-- find all functions
for k in script:gmatch("[%s]-[%w_]-%s-%(.-%)[%s\n]-%b{}") do
    local funcEntry = {
        sourcePattern = k:gsub("[%(%)%.%%%+%-%*%?%[%]%^%$]", "%%%0"),
        name = k:match("[%s]-([%w_]+)%s-%(.-%)[%s\n]-%b{}"),
        params = k:match("[%s]-[%w_]-%s-%((.-)%)[%s\n]-%b{}"),
        code = k:match("[%s]-[%w_]-%s-%(.-%)[%s\n]-(%b{})"):gsub("^%s*", "")
    }
    table.insert(functions, funcEntry)
end

for i, v in ipairs(functions) do
    -- script = script:gsub(v.sourcePattern, "\n`FUNC_" .. i .. "`") --
end
---------------------------------------------------------------------------------------

print(script)
