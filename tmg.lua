local map = require("build/map")

local objects = {}
local rooms = {}
local layers = {}
for i, tiledLayer in ipairs(map.layers) do
    if tiledLayer.type == "objectgroup" then
        for j, object in ipairs(tiledLayer.objects) do
            if object.type == "ROOM" then
                table.insert(rooms, object)
            else
                table.insert(objects, object)
            end
        end
    end
    if tiledLayer.type == "tilelayer" then
        table.insert(layers, tiledLayer)
    end
end

local metaTilesetWidth = 20

local bg1File = io.open("build/bg1.bin", "w")

for i, room in ipairs(rooms) do
    local roomTileX = room.x/16
    local roomTileY = room.y/16
    local roomTileWidth = room.width/16
    local roomTileHeight = room.height/16
    for metaY=roomTileY,roomTileY+roomTileHeight-1 do
        for yOffset=0,1 do
            for metaX=roomTileX,roomTileX+roomTileWidth-1 do
                for j, layer in ipairs(layers) do
                    for k, chunk in ipairs(layer.chunks) do
                        if metaX >= chunk.x and metaX < chunk.x + chunk.width and metaY >= chunk.y and metaY < chunk.y + chunk.height then
                            --table.insert(room.bgs[j], chunk.data[(metaX-chunk.x)+(metaY-chunk.y)*chunk.width+1])
                            local metaTileIndex = chunk.data[(metaX-chunk.x)+(metaY-chunk.y)*chunk.width+1];
                            if metaTileIndex == 0 then
                                bg1File:write('\0')
                                bg1File:write('\0')
                                bg1File:write('\0')
                                bg1File:write('\0')
                            else
                                local metaTileX = metaTileIndex-1
                                local metaTileY = 0
                                while metaTileX >= metaTilesetWidth do
                                    metaTileX = metaTileX - metaTilesetWidth
                                    metaTileY = metaTileY + 1
                                end
                                local tileIndex0 = (metaTileX+yOffset*metaTilesetWidth+metaTileY*metaTilesetWidth*2)*2
                                local tileIndex1 = tileIndex0 + 1
                                bg1File:write(string.char(bit.band(bit.bor(bit.lshift(1, 12), tileIndex0), 0xFF)))
                                bg1File:write(string.char(bit.rshift(bit.bor(bit.lshift(1, 12), tileIndex0), 8)))
                                bg1File:write(string.char(bit.band(bit.bor(bit.lshift(1, 12), tileIndex1), 0xFF)))
                                bg1File:write(string.char(bit.rshift(bit.bor(bit.lshift(1, 12), tileIndex1), 8)))
                            end
                        end
                    end
                end
            end
        end
    end
end
