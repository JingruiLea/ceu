local TOP   = {}    -- holds all clss/exts/nats
local TOP_i = 1     -- next top
-- TODO: pra q serve?

local node = _AST.node

F = {
    Root_pos = function (me)
        _AST.root = node('Root', me.ln, unpack(TOP))
        return _AST.root
    end,

    Dcl_cls_pos = function (me)
        table.insert(TOP, TOP_i, me)
        TOP_i = TOP_i + 1
        return node('Nothing', me.ln)
    end,

    Dcl_nat_pos = function (me)
        table.insert(TOP, TOP_i, me)
        TOP_i = TOP_i + 1
        return node('Nothing', me.ln)
    end,
    Dcl_ext_pos = function (me)
        table.insert(TOP, TOP_i, me)
        TOP_i = TOP_i + 1
        return node('Nothing', me.ln)
    end,
}

local function id2ifc (id)
    for _, cls in ipairs(TOP) do
        local _,id2 = unpack(cls)
        if id2 == id then
            return cls
        end
    end
    return nil
end

_AST.visit(F)

-- substitute all Dcl_imp for the referred fields
for _, cls in ipairs(TOP) do
    if cls.tag=='Dcl_cls' and cls[2]~='Main' then
        local dcls1 = cls.blk_ifc[1][1]
        assert(dcls1.tag == 'BlockI')
        for i=1, #dcls1 do
            local imp = dcls1[i]
            if imp.tag == '_Dcl_imp' then
                -- interface A,B,...
                for _,dcl in ipairs(imp) do
                    local ifc = id2ifc(dcl)  -- interface must exist
                    ASR(ifc and ifc[1]==true,
                        imp, 'interface "'..dcl..'" is not declared')
                    local dcls2 = ifc.blk_ifc[1][1]
                    assert(dcls2.tag == 'BlockI')
                    for _, dcl2 in ipairs(dcls2) do
                        assert(dcl2.tag ~= 'Dcl_imp')   -- impossible, I'm going in order
                        dcls1[#dcls1+1] = _AST.copy(dcl2)
                            -- fields from interface should go to the end indeed
                    end
                end
                table.remove(dcls1, i) -- remove _Dcl_imp
                i = i - 1                    -- repeat
            else
            end
        end
    end
end
