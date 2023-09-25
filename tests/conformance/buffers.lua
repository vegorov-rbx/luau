-- This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
print("testing byte buffer library")

-- TODO: 2 spaces instead of 4 for tests I guess

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

local function simple_byte_reads()
    local b = buffer.create(1024)

    assert(buffer.readi8(b, 5) == 0)
    buffer.writei8(b, 10, 32)
    assert(buffer.readi8(b, 10) == 32)
    buffer.writei8(b, 15, 5)
    buffer.writei8(b, 14, 4)
    buffer.writei8(b, 13, 3)
    buffer.writei8(b, 12, 2)
    buffer.writei8(b, 11, 1)
    assert(buffer.readi8(b, 11) == 1)
    assert(buffer.readi8(b, 12) == 2)
    assert(buffer.readi8(b, 13) == 3)
    assert(buffer.readi8(b, 14) == 4)
    assert(buffer.readi8(b, 15) == 5)

    local x = buffer.readi8(b, 14) + buffer.readi8(b, 13)
    assert(x == 7)
end

simple_byte_reads()

local function offset_byte_reads(start: number)
    local b = buffer.create(1024)

    buffer.writei8(b, start, 32)
    assert(buffer.readi8(b, start) == 32)
    buffer.writei8(b, start + 5, 5)
    buffer.writei8(b, start + 4, 4)
    buffer.writei8(b, start + 3, 3)
    buffer.writei8(b, start + 2, 2)
    buffer.writei8(b, start + 1, 1)
    assert(buffer.readi8(b, start + 1) == 1)
    assert(buffer.readi8(b, start + 2) == 2)
    assert(buffer.readi8(b, start + 3) == 3)
    assert(buffer.readi8(b, start + 4) == 4)
    assert(buffer.readi8(b, start + 5) == 5)

    local x = buffer.readi8(b, start + 4) + buffer.readi8(b, start + 3)
    assert(x == 7)
end

offset_byte_reads(5)
offset_byte_reads(30)

local function simple_float_reinterpret()
    local b = buffer.create(1024)

    buffer.writei32(b, 10, 0x3f800000)
    local one = buffer.readf32(b, 10)
    assert(one == 1.0)

    buffer.writef32(b, 10, 2.75197)
    local magic = buffer.readi32(b, 10)
    assert(magic == 0x40302047)
end

simple_float_reinterpret()

local function simple_double_reinterpret()
    local b = buffer.create(1024)
    
    buffer.writei32(b, 10, 0x00000000)
    buffer.writei32(b, 14, 0x3ff00000)
    local one = buffer.readf64(b, 10)
    assert(one == 1.0)

    buffer.writef64(b, 10, 1.437576533064206)
    local magic1 = buffer.readi32(b, 10)
    local magic2 = buffer.readi32(b, 14)

    assert(magic1 == 0x40302010)
    assert(magic2 == 0x3ff70050)
end

simple_double_reinterpret()

local function simple_string_ops()
    local b = buffer.create(1024)

    buffer.writestring(b, 15, " world")
    buffer.writestring(b, 10, "hello")
    buffer.writei8(b, 21, string.byte('!'))

    local str = buffer.readstring(b, 10, 12)

    assert(str == "hello world!")
end

simple_string_ops()

-- bounds checking!

local function createchecks()
    assert(ecall(function() buffer.create(-1) end) == "invalid buffer size")
    assert(ecall(function() buffer.create(10e10) end) == "invalid buffer size")
    assert(ecall(function() buffer.create(0/0) end) == "invalid buffer size")
end

createchecks()

-- TODO: message doesn't make sense for negative offsets

local function boundchecks()
    local b = buffer.create(1024)

    assert(call(function() return buffer.readi8(b, 1023) end) == 0)
    assert(ecall(function() buffer.readi8(b, 1024) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.readi8(b, -1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.readi8(b, -100000) end) == "attempt to read i8 past the length of a buffer")

    call(function() buffer.writei8(b, 1023, 0) end)
    assert(ecall(function() buffer.writei8(b, 1024, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, -1, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, -100000, 0) end) == "attempt to write i8 past the length of a buffer")

    -- i16
    assert(call(function() return buffer.readi16(b, 1022) end) == 0)
    assert(ecall(function() buffer.readi16(b, 1023) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, -1) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, -100000) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, 0x7fffffff) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, 0x7ffffffe) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, 0x7ffffffd) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, 0x80000000) end) == "attempt to read i16 past the length of a buffer")

    call(function() buffer.writei16(b, 1022, 0) end)
    assert(ecall(function() buffer.writei16(b, 1023, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buffer.writei16(b, -1, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buffer.writei16(b, -100000, 0) end) == "attempt to write i16 past the length of a buffer")

    -- i32
    assert(call(function() return buffer.readi32(b, 1020) end) == 0)
    assert(ecall(function() buffer.readi32(b, 1021) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.readi32(b, -1) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.readi32(b, -100000) end) == "attempt to read i32 past the length of a buffer")

    call(function() buffer.writei32(b, 1020, 0) end)
    assert(ecall(function() buffer.writei32(b, 1021, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buffer.writei32(b, -1, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buffer.writei32(b, -100000, 0) end) == "attempt to write i32 past the length of a buffer")

    -- f32
    assert(call(function() return buffer.readf32(b, 1020) end) == 0)
    assert(ecall(function() buffer.readf32(b, 1021) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.readf32(b, -1) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.readf32(b, -100000) end) == "attempt to read f32 past the length of a buffer")

    call(function() buffer.writef32(b, 1020, 0) end)
    assert(ecall(function() buffer.writef32(b, 1021, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buffer.writef32(b, -1, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buffer.writef32(b, -100000, 0) end) == "attempt to write f32 past the length of a buffer")

    -- f64
    assert(call(function() return buffer.readf64(b, 1016) end) == 0)
    assert(ecall(function() buffer.readf64(b, 1017) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.readf64(b, -1) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.readf64(b, -100000) end) == "attempt to read f64 past the length of a buffer")

    call(function() buffer.writef64(b, 1016, 0) end)
    assert(ecall(function() buffer.writef64(b, 1017, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buffer.writef64(b, -1, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buffer.writef64(b, -100000, 0) end) == "attempt to write f64 past the length of a buffer")

    -- string
    assert(call(function() return buffer.readstring(b, 1016, 8) end) == "\0\0\0\0\0\0\0\0")
    assert(ecall(function() buffer.readstring(b, 1017, 8) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, -1, 8) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, -100000, 8) end) == "attempt to read string past the length of a buffer")

    call(function() buffer.writestring(b, 1016, 0, "abcdefgh") end)
    assert(ecall(function() buffer.writestring(b, 1017, "abcdefgh") end) == "attempt to write string past the length of a buffer")
    assert(ecall(function() buffer.writestring(b, -1, "abcdefgh") end) == "attempt to write string past the length of a buffer")
    assert(ecall(function() buffer.writestring(b, -100000, "abcdefgh") end) == "attempt to write string past the length of a buffer")
end

boundchecks()

local function boundchecksnonconst(size, minus1, minusbig, intmax)
    local b = buffer.create(size)

    assert(call(function() return buffer.readi8(b, size-1) end) == 0)
    assert(ecall(function() buffer.readi8(b, size) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.readi8(b, minus1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.readi8(b, minusbig) end) == "attempt to read i8 past the length of a buffer")

    call(function() buffer.writei8(b, size-1, 0) end)
    assert(ecall(function() buffer.writei8(b, size, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, minus1, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, minusbig, 0) end) == "attempt to write i8 past the length of a buffer")

    -- i16
    assert(call(function() return buffer.readi16(b, size-2) end) == 0)
    assert(ecall(function() buffer.readi16(b, size-1) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, minus1) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, minusbig) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, intmax) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, intmax-1) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, intmax-2) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, intmax+1) end) == "attempt to read i16 past the length of a buffer")

    call(function() buffer.writei16(b, size-2, 0) end)
    assert(ecall(function() buffer.writei16(b, size-1, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buffer.writei16(b, minus1, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buffer.writei16(b, minusbig, 0) end) == "attempt to write i16 past the length of a buffer")

    -- i32
    assert(call(function() return buffer.readi32(b, size-4) end) == 0)
    assert(ecall(function() buffer.readi32(b, size-3) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.readi32(b, minus1) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.readi32(b, minusbig) end) == "attempt to read i32 past the length of a buffer")

    call(function() buffer.writei32(b, size-4, 0) end)
    assert(ecall(function() buffer.writei32(b, size-3, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buffer.writei32(b, minus1, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buffer.writei32(b, minusbig, 0) end) == "attempt to write i32 past the length of a buffer")

    -- f32
    assert(call(function() return buffer.readf32(b, size-4) end) == 0)
    assert(ecall(function() buffer.readf32(b, size-3) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.readf32(b, minus1) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.readf32(b, minusbig) end) == "attempt to read f32 past the length of a buffer")

    call(function() buffer.writef32(b, size-4, 0) end)
    assert(ecall(function() buffer.writef32(b, size-3, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buffer.writef32(b, minus1, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buffer.writef32(b, minusbig, 0) end) == "attempt to write f32 past the length of a buffer")

    -- f64
    assert(call(function() return buffer.readf64(b, size-8) end) == 0)
    assert(ecall(function() buffer.readf64(b, size-7) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.readf64(b, minus1) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.readf64(b, minusbig) end) == "attempt to read f64 past the length of a buffer")

    call(function() buffer.writef64(b, size-8, 0) end)
    assert(ecall(function() buffer.writef64(b, size-7, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buffer.writef64(b, minus1, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buffer.writef64(b, minusbig, 0) end) == "attempt to write f64 past the length of a buffer")

    -- string
    assert(call(function() return buffer.readstring(b, size-8, 8) end) == "\0\0\0\0\0\0\0\0")
    assert(ecall(function() buffer.readstring(b, size-7, 8) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, minus1, 8) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, minusbig, 8) end) == "attempt to read string past the length of a buffer")

    call(function() buffer.writestring(b, size-8, 0, "abcdefgh") end)
    assert(ecall(function() buffer.writestring(b, size-7, "abcdefgh") end) == "attempt to write string past the length of a buffer")
    assert(ecall(function() buffer.writestring(b, minus1, "abcdefgh") end) == "attempt to write string past the length of a buffer")
    assert(ecall(function() buffer.writestring(b, minusbig, "abcdefgh") end) == "attempt to write string past the length of a buffer")
end

boundchecksnonconst(1024, -1, -100000, 0x7fffffff) -- TODO: make sure this doesn't inline

local function boundcheckssmall()
    local b = buffer.create(1)

    assert(call(function() return buffer.readi8(b, 0) end) == 0)
    assert(ecall(function() buffer.readi8(b, 1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.readi8(b, -1) end) == "attempt to read i8 past the length of a buffer")

    call(function() buffer.writei8(b, 0, 0) end)
    assert(ecall(function() buffer.writei8(b, 1, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, -1, 0) end) == "attempt to write i8 past the length of a buffer")

    -- i16
    assert(ecall(function() buffer.readi16(b, 0) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, -1) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi16(b, -2) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.writei16(b, 0, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buffer.writei16(b, -1, 0) end) == "attempt to write i16 past the length of a buffer")
    assert(ecall(function() buffer.writei16(b, -2, 0) end) == "attempt to write i16 past the length of a buffer")

    -- i32
    assert(ecall(function() buffer.readi32(b, 0) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.readi32(b, -1) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.readi32(b, -4) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.writei32(b, 0, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buffer.writei32(b, -1, 0) end) == "attempt to write i32 past the length of a buffer")
    assert(ecall(function() buffer.writei32(b, -4, 0) end) == "attempt to write i32 past the length of a buffer")

    -- f32
    assert(ecall(function() buffer.readf32(b, 0) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.readf32(b, -1) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.readf32(b, -4) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.writef32(b, 0, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buffer.writef32(b, -1, 0) end) == "attempt to write f32 past the length of a buffer")
    assert(ecall(function() buffer.writef32(b, -4, 0) end) == "attempt to write f32 past the length of a buffer")

    -- f64
    assert(ecall(function() buffer.readf64(b, 0) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.readf64(b, -1) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.readf64(b, -8) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.writef64(b, 0, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buffer.writef64(b, -1, 0) end) == "attempt to write f64 past the length of a buffer")
    assert(ecall(function() buffer.writef64(b, -7, 0) end) == "attempt to write f64 past the length of a buffer")

    -- string
    assert(ecall(function() buffer.readstring(b, 0, 8) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, -1, 8) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, -8, 8) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.writestring(b, 0, "abcdefgh") end) == "attempt to write string past the length of a buffer")
    assert(ecall(function() buffer.writestring(b, -1, "abcdefgh") end) == "attempt to write string past the length of a buffer")
    assert(ecall(function() buffer.writestring(b, -7, "abcdefgh") end) == "attempt to write string past the length of a buffer")
end

boundcheckssmall()

local function boundchecksempty()
    local b = buffer.create(0) -- useless, but probably more generic
    
    assert(ecall(function() buffer.readi8(b, 1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.readi8(b, 0) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.readi8(b, -1) end) == "attempt to read i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, 1, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, 0, 0) end) == "attempt to write i8 past the length of a buffer")
    assert(ecall(function() buffer.writei8(b, -1, 0) end) == "attempt to write i8 past the length of a buffer")

    assert(ecall(function() buffer.readi16(b, 0) end) == "attempt to read i16 past the length of a buffer")
    assert(ecall(function() buffer.readi32(b, 0) end) == "attempt to read i32 past the length of a buffer")
    assert(ecall(function() buffer.readf32(b, 0) end) == "attempt to read f32 past the length of a buffer")
    assert(ecall(function() buffer.readf64(b, 0) end) == "attempt to read f64 past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, 0, 1) end) == "attempt to read string past the length of a buffer")
    assert(ecall(function() buffer.readstring(b, 0, 8) end) == "attempt to read string past the length of a buffer")
end

boundchecksempty()

return('OK')
