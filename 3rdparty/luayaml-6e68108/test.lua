require "yaml"

yaml.configure {
    dump_auto_array = true,
    dump_check_metatables = true,
    dump_error_on_unsupported = true,
    load_set_metatables = true,
    load_numeric_scalars = true,
    load_nulls_as_nil = false
}

local doc = yaml.load[[
%YAML 1.1
---
empty: ~
map:   Wrapped
       line
map2:
    seq:
    - libyaml     # C Fast YAML 1.1
    - lua-yaml    # Lua binding to libyaml
...
]]

assert(doc.map == "Wrapped line",       'map value should be "Wrapped line"')
assert(doc.map2.seq[1] == "libyaml",    'seq[1] value should be "libyaml"')
assert(#doc.map2.seq == 2,              'length of seq should be 2')
assert(doc.empty == yaml.null,          'empty value should equal yaml.null')

print "All tests passed"
