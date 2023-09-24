-- This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
print("testing byte buffer library")

-- TODO: 2 spaces instead of 4 for tests I guess

local function simple_byte_reads()
    local b = buf.create(1024)

    assert(buf.readi8(b, 5) == 0)
    buf.writei8(b, 10, 32)
    assert(buf.readi8(b, 10) == 32)
    buf.writei8(b, 15, 5)
    buf.writei8(b, 14, 4)
    buf.writei8(b, 13, 3)
    buf.writei8(b, 12, 2)
    buf.writei8(b, 11, 1)
    assert(buf.readi8(b, 11) == 1)
    assert(buf.readi8(b, 12) == 2)
    assert(buf.readi8(b, 13) == 3)
    assert(buf.readi8(b, 14) == 4)
    assert(buf.readi8(b, 15) == 5)

    local x = buf.readi8(b, 14) + buf.readi8(b, 13)
    assert(x == 7)
end

simple_byte_reads()

local function offset_byte_reads(start: number)
    local b = buf.create(1024)

    buf.writei8(b, start, 32)
    assert(buf.readi8(b, start) == 32)
    buf.writei8(b, start + 5, 5)
    buf.writei8(b, start + 4, 4)
    buf.writei8(b, start + 3, 3)
    buf.writei8(b, start + 2, 2)
    buf.writei8(b, start + 1, 1)
    assert(buf.readi8(b, start + 1) == 1)
    assert(buf.readi8(b, start + 2) == 2)
    assert(buf.readi8(b, start + 3) == 3)
    assert(buf.readi8(b, start + 4) == 4)
    assert(buf.readi8(b, start + 5) == 5)

    local x = buf.readi8(b, start + 4) + buf.readi8(b, start + 3)
    assert(x == 7)
end

offset_byte_reads(5)
offset_byte_reads(30)

local function simple_float_reinterpret()
    local b = buf.create(1024)

    buf.writei32(b, 10, 0x3f800000)
    local one = buf.readf32(b, 10)
    assert(one == 1.0)

    buf.writef32(b, 10, 2.75197)
    local magic = buf.readi32(b, 10)
    assert(magic == 0x40302047)
end

simple_float_reinterpret()

local function simple_double_reinterpret()
    local b = buf.create(1024)
    
    buf.writei32(b, 10, 0x00000000)
    buf.writei32(b, 14, 0x3ff00000)
    local one = buf.readf64(b, 10)
    assert(one == 1.0)

    buf.writef64(b, 10, 1.437576533064206)
    local magic1 = buf.readi32(b, 10)
    local magic2 = buf.readi32(b, 14)

    assert(magic1 == 0x40302010)
    assert(magic2 == 0x3ff70050)
end

simple_double_reinterpret()

local function simple_string_ops()
    local b = buf.create(1024)

    buf.writestring(b, 15, " world")
    buf.writestring(b, 10, "hello")
    buf.writei8(b, 21, string.byte('!'))

    local str = buf.readstring(b, 10, 12)

    assert(str == "hello world!")
end

simple_string_ops()

-- bounds checking!

function call(fn, ...)
    local ok, res = pcall(fn, ...)
    assert(ok)
    return res
end

function ecall(fn, ...)
    local ok, err = pcall(fn, ...)
    assert(not ok)
    return err:sub((err:find(": ") or -1) + 2, #err)
end

local function createchecks()
    assert(ecall(function() buf.create(-1) end) == "invalid buffer size")
    assert(ecall(function() buf.create(10e10) end) == "invalid buffer size")
    assert(ecall(function() buf.create(0/0) end) == "invalid buffer size")
end

createchecks()

-- TODO: with 1-based indexing this all has to be adjusted, yay
-- TODO: message doesn't make sense for negative offsets

local function boundchecks()
    local b = buf.create(1024)

    assert(call(function() return buf.readi8(b, 1023) end) == 0)
    assert(ecall(function() buf.readi8(b, 1024) end) == "attempt to read i8 past the length of a buffer")
    --assert(ecall(function() buf.readi8(b, 0) end) == "attempt to read i8 past the length of a buffer") -- TODO
    assert(ecall(function() buf.readi8(b, -1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buf.readi8(b, -100000) end) == "attempt to read i8 past the length of a buffer")

    call(function() buf.writei8(b, 1023, 0) end)
    assert(ecall(function() buf.writei8(b, 1024, 0) end) == "attempt to write i8 past the length of a buffer")
    --assert(ecall(function() buf.writei8(b, 0, 0) end) == "attempt to write i8 past the length of a buffer") -- TODO
    assert(ecall(function() buf.writei8(b, -1, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buf.writei8(b, -100000, 0) end) == "attempt to write i8 past the length of a buffer")

    -- i16
    assert(call(function() return buf.readi16(b, 1022) end) == 0)
    assert(ecall(function() buf.readi16(b, 1023) end) == "attempt to read i16 past the length of a buffer")
    --assert(ecall(function() buf.readi16(b, 0) end) == "attempt to read i16 past the length of a buffer") -- TODO
    assert(ecall(function() buf.readi16(b, -1) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buf.readi16(b, -100000) end) == "attempt to read i16 past the length of a buffer")

    call(function() buf.writei16(b, 1022, 0) end)
    assert(ecall(function() buf.writei16(b, 1023, 0) end) == "attempt to write i16 past the length of a buffer")
    --assert(ecall(function() buf.writei16(b, 0, 0) end) == "attempt to write i16 past the length of a buffer") -- TODO
    assert(ecall(function() buf.writei16(b, -1, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buf.writei16(b, -100000, 0) end) == "attempt to write i16 past the length of a buffer")

    -- i32
    assert(call(function() return buf.readi32(b, 1020) end) == 0)
    assert(ecall(function() buf.readi32(b, 1021) end) == "attempt to read i32 past the length of a buffer")
    --assert(ecall(function() buf.readi32(b, 0) end) == "attempt to read i32 past the length of a buffer") -- TODO
    assert(ecall(function() buf.readi32(b, -1) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buf.readi32(b, -100000) end) == "attempt to read i32 past the length of a buffer")

    call(function() buf.writei32(b, 1020, 0) end)
    assert(ecall(function() buf.writei32(b, 1021, 0) end) == "attempt to write i32 past the length of a buffer")
    --assert(ecall(function() buf.writei32(b, 0, 0) end) == "attempt to write i32 past the length of a buffer") -- TODO
    assert(ecall(function() buf.writei32(b, -1, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buf.writei32(b, -100000, 0) end) == "attempt to write i32 past the length of a buffer")

    -- f32
    assert(call(function() return buf.readf32(b, 1020) end) == 0)
    assert(ecall(function() buf.readf32(b, 1021) end) == "attempt to read f32 past the length of a buffer")
    --assert(ecall(function() buf.readf32(b, 0) end) == "attempt to read f32 past the length of a buffer") -- TODO
    assert(ecall(function() buf.readf32(b, -1) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buf.readf32(b, -100000) end) == "attempt to read f32 past the length of a buffer")

    call(function() buf.writef32(b, 1020, 0) end)
    assert(ecall(function() buf.writef32(b, 1021, 0) end) == "attempt to write f32 past the length of a buffer")
    --assert(ecall(function() buf.writef32(b, 0, 0) end) == "attempt to write f32 past the length of a buffer") -- TODO
    assert(ecall(function() buf.writef32(b, -1, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buf.writef32(b, -100000, 0) end) == "attempt to write f32 past the length of a buffer")

    -- f64
    assert(call(function() return buf.readf64(b, 1016) end) == 0)
    assert(ecall(function() buf.readf64(b, 1017) end) == "attempt to read f64 past the length of a buffer")
    --assert(ecall(function() buf.readf64(b, 0) end) == "attempt to read f64 past the length of a buffer") -- TODO
    assert(ecall(function() buf.readf64(b, -1) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buf.readf64(b, -100000) end) == "attempt to read f64 past the length of a buffer")

    call(function() buf.writef64(b, 1016, 0) end)
    assert(ecall(function() buf.writef64(b, 1017, 0) end) == "attempt to write f64 past the length of a buffer")
    --assert(ecall(function() buf.writef64(b, 0, 0) end) == "attempt to write f64 past the length of a buffer") -- TODO
    assert(ecall(function() buf.writef64(b, -1, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buf.writef64(b, -100000, 0) end) == "attempt to write f64 past the length of a buffer")
end

boundchecks()

local function boundcheckssmall()
    local b = buf.create(1)

    assert(call(function() return buf.readi8(b, 0) end) == 0)
    assert(ecall(function() buf.readi8(b, 1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buf.readi8(b, -1) end) == "attempt to read i8 past the length of a buffer")

    call(function() buf.writei8(b, 0, 0) end)
    assert(ecall(function() buf.writei8(b, 1, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buf.writei8(b, -1, 0) end) == "attempt to write i8 past the length of a buffer")

    -- i16
    assert(ecall(function() buf.readi16(b, 0) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buf.readi16(b, -1) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buf.readi16(b, -2) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buf.writei16(b, 0, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buf.writei16(b, -1, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buf.writei16(b, -2, 0) end) == "attempt to write i16 past the length of a buffer")

    -- i32
    assert(ecall(function() buf.readi32(b, 0) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buf.readi32(b, -1) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buf.readi32(b, -4) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buf.writei32(b, 0, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buf.writei32(b, -1, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buf.writei32(b, -4, 0) end) == "attempt to write i32 past the length of a buffer")

    -- f32
    assert(ecall(function() buf.readf32(b, 0) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buf.readf32(b, -1) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buf.readf32(b, -4) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buf.writef32(b, 0, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buf.writef32(b, -1, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buf.writef32(b, -4, 0) end) == "attempt to write f32 past the length of a buffer")

    -- f64
    assert(ecall(function() buf.readf64(b, 0) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buf.readf64(b, -1) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buf.readf64(b, -8) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buf.writef64(b, 0, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buf.writef64(b, -1, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buf.writef64(b, -7, 0) end) == "attempt to write f64 past the length of a buffer")
end

boundcheckssmall()

local function boundchecksempty()
    local b = buf.create(0) -- useless, but probably more generic
    
    assert(ecall(function() buf.readi8(b, 1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buf.readi8(b, 0) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buf.readi8(b, -1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buf.writei8(b, 1, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buf.writei8(b, 0, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buf.writei8(b, -1, 0) end) == "attempt to write i8 past the length of a buffer")

    assert(ecall(function() buf.readi16(b, 0) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buf.readi32(b, 0) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buf.readf32(b, 0) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buf.readf64(b, 0) end) == "attempt to read f64 past the length of a buffer")
end

boundcheckssmall()

return('OK')
