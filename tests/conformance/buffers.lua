-- This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
print("testing byte buffer library")

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
    assert(magic1 == 0x40302010)

    buf.writef32(b, 10, 2.75197)
    local magic2 = buf.readi32(b, 14)
    assert(magic2 == 0x3ff70050)
end

simple_double_reinterpret()

return('OK')
