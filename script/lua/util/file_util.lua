require("lfs")
require("io")
require("util.crypt")

FileUtil = {}

function FileUtil.for_each_file(path, cb)
    for file in lfs.dir(path) do
        --过滤"."和".."目录
        if file ~= "." and file ~= ".." then
            local f = path.. '/' ..file
            local attr = lfs.attributes (f)
            --如果是是目录，就递归调用，否则就写入文件
            if attr.mode == "directory" then
                FileUtil.for_each_file(f, cb)
            else
                cb(f)
            end
        end
    end
end

function FileUtil.calc_file_md5(file_name)
    local file = io.open(file_name, "r")
    local s = file:read('*all')
    return Crypt.md5(s)
end
