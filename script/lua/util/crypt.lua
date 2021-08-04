Crypt = {}

function Crypt.md5(s)
    local md5 = MD5.new()
    md5:update(s, string.len(s))
    return md5:hexdigest(s)
end
