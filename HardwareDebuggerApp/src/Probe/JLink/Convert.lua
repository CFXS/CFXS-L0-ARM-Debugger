-------- parse
for fn in str:gmatch("[^\r\n]+") do
    local origName = fn:match(".+%s(.-)%(")
    local fname = origName:gsub("^JLINK_", ""):gsub("^JLINKARM_", "")
    print(string.format("%s = nullptr;", fn:gsub(";", ""):gsub(origName, "(*"..fname..")")))
end

-------- assignment gen
for fn in str:gmatch("[^\r\n]+") do
    local origName = fn:match(".+%s(.-)%(")
    local fname = origName:gsub("^JLINK_", ""):gsub("^JLINKARM_", "")
    print(string.format("%s = m_Library->GetFunction<decltype(%s)>(\"%s\");", fname, fname, origName))
end