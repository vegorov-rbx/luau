-- This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
print("testing byte buffer library")

local function simple_byte_reads()
    local b = buf.create(1024)

    assert(buf.readi8(b, 5) == 0)
    buf.writei8(b, 10, 32)
    assert(buf.readi8(b, 10) == 32)
end

simple_byte_reads()

return('OK')
