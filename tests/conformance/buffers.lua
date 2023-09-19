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
end

simple_byte_reads()

return('OK')
