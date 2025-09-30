function onUse(cid, item, fromPosition, itemEx, toPosition)
    if not isPlayer(cid) then
        return false
    end
    -- send open stash.
    doPlayerOpenStash(cid)    
    return true
end
