local map = require("build/map")

local objects = {}
local rooms = {}
local layers = {}
local collisionLayer = nil
local collisionTileIndex = nil
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
        if tiledLayer.name == "collision layer" then
            collisionLayer = tiledLayer
        else
            table.insert(layers, tiledLayer)
        end
    end
end

for i, tileset in ipairs(map.tilesets) do
    if tileset.name == "collision tileset" then
        collisionTileIndex = tileset.firstgid
    end
end

-- TODO: Take this from embedded tileset!
local metaTilesetWidth = 13

local bg1File = io.open("build/bg1.bin", "w")

local collisionFile = io.open("build/collisionLayer.bin", "w")

local roomsWidth = 0
local roomsHeight = 0
local roomsArray = {}
local tileArrayIndex = 0
local collisionArrayIndex = 0
local roomIndex = 0

local roomsFile = io.open("build/rooms.h", "w")

roomsFile:write("#include \"bg1_bin.h\"\n")
roomsFile:write("#include \"collisionLayer_bin.h\"\n")

roomsFile:write("const Room rooms[] = {")

for i, room in ipairs(rooms) do
    local roomTileX = room.x/16
    local roomTileY = room.y/16
    local roomTileWidth = room.width/16
    local roomTileHeight = room.height/16
    if roomsWidth < (roomTileX + roomTileWidth)/15 then
        roomsWidth = (roomTileX + roomTileWidth)/15
    end
    if roomsHeight < (roomTileY + roomTileHeight)/10 then
        roomsHeight = (roomTileY + roomTileHeight)/10
    end

    for roomY=roomTileY/10,roomTileHeight/10-1 do
        for roomX=roomTileX/15,roomTileWidth/15-1 do
            if roomsArray[roomY] == nil then
                roomsArray[roomY] = {}
            end
            roomsArray[roomY][roomX] = roomIndex
        end
    end
    roomIndex = roomIndex + 1

    roomsFile:write("{", roomTileX/15, ",", roomTileY/10, ",", roomTileWidth, ",", roomTileHeight, ",((uint16 *)bg1_bin) +", tileArrayIndex, ",collisionLayer_bin+", collisionArrayIndex, "},")

    for metaY=roomTileY,roomTileY+roomTileHeight-1 do
        for metaX=roomTileX,roomTileX+roomTileWidth-1 do
            for k, chunk in ipairs(collisionLayer.chunks) do
                if metaX >= chunk.x and metaX < chunk.x + chunk.width and metaY >= chunk.y and metaY < chunk.y + chunk.height then
                    collisionArrayIndex = collisionArrayIndex + 1
                    local tileIndex = chunk.data[(metaX-chunk.x)+(metaY-chunk.y)*chunk.width+1]
                    if tileIndex == 0 then
                        collisionFile:write('\0')
                    else
                        collisionFile:write(string.char(tileIndex-collisionTileIndex+1))
                    end
                end
            end
        end
    end
                          
    for metaY=roomTileY,roomTileY+roomTileHeight-1 do
        for yOffset=0,1 do
            for metaX=roomTileX,roomTileX+roomTileWidth-1 do
                for j, layer in ipairs(layers) do
                    for k, chunk in ipairs(layer.chunks) do
                        if metaX >= chunk.x and metaX < chunk.x + chunk.width and metaY >= chunk.y and metaY < chunk.y + chunk.height then
                            --table.insert(room.bgs[j], chunk.data[(metaX-chunk.x)+(metaY-chunk.y)*chunk.width+1])
                            local metaTileIndex = chunk.data[(metaX-chunk.x)+(metaY-chunk.y)*chunk.width+1]

                            tileArrayIndex = tileArrayIndex + 2
                            
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

roomsFile:write("};\n")
roomsFile:write("const uint8 roomMap[", roomsHeight, "][", roomsWidth, "] = {")
for roomY=0,roomsHeight-1 do
    roomsFile:write("{")
    for roomX=0,roomsWidth-1 do
        roomsFile:write(roomsArray[roomY][roomX], ",")
    end
    roomsFile:write("},")
end
roomsFile:write("};")

roomsFile:close()
bg1File:close()
collisionFile:close()