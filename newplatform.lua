function newplatform(plf)
    local name = plf.name
    local description = plf.description
 
    -- Register new platform
    premake.platforms[name] = {
        cfgsuffix = "_"..name,
        iscrosscompiler = true
    }
 
    -- Allow use of new platform in --platfroms
    table.insert(premake.fields["platforms"].allowed, name)
    table.insert(premake.option.list["platform"].allowed, { name, description })
 
    -- Add compiler support
    -- gcc
    premake.gcc.platforms[name] = plf.gcc
end
