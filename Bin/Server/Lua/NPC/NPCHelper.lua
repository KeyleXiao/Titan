function CreateNPCTest()
	LuaCreateNPC(1, 1, 50, 18, 50)
end

function LuaCreateNPC(nSenceID, nMonsterID, x, y, z)
	gameHepler.createNPC( nSenceID,nMonsterID,x,y,z )
end

function LuaDestroyNPC(nSenceID, npcUID)
	gameHepler.destroyNPC(nSenceID, npcUID)
end