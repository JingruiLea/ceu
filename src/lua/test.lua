CEU = {
    arg  = {},
    opts = {
        ceu          = true,
        ceu_input    = './test.ceu',
        ceu_output   = './test.ceu.c',

        env          = true,
        env_types    = '../../env/types.h',
        env_threads  = '../../env/threads.h',
        env_ceu      = './test.ceu.c',
        env_main     = '../../env/main.c',
        env_output   = './test.c',

        cc           = true,
        cc_input     = './test.c',
        cc_output    = './test.exe',
        cc_args      = '-Wall -Wextra -Werror'
                        -- TODO: remove all "-Wno-*"
                        ..' -Wno-unused'
                        ..' -Wno-missing-field-initializers'
                        ..' -Wno-implicit-fallthrough'
                        ..' -llua5.3 -lpthread '
                     ,

        --ceu_features_lua    = 'true',
        --ceu_features_thread = 'true',
        --ceu_line_directives = 'true',
        --ceu_line_directives = 'false',
        --ceu_err_unused_native = 'pass'

        ceu_features_os    = 'true',
        ceu_features_async = 'true',
    }
}

-- CEU.opts.pre          = true
-- CEU.opts.pre_args     = '-I ../include'
-- CEU.opts.pre_input    = './test.ceu'
-- CEU.opts.pre_output   = './test.ceu.cpp'
-- CEU.opts.ceu_input    = './test.ceu.cpp'


dofile 'dbg.lua'
DBG,ASR = DBG1,ASR1

local ceu_vector_h = assert(io.open'../../src/c/ceu_vector.h'):read'*a'
local ceu_vector_c = assert(io.open'../../src/c/ceu_vector.c'):read'*a'
local ceu_pool_c   = assert(io.open'../../src/c/ceu_pool.c'):read'*a'
local ceu_c        = assert(io.open'../../src/c/ceu.c'):read'*a'
ceu_c = SUB(ceu_c, '=== CEU_VECTOR_H ===',   ceu_vector_h)
ceu_c = SUB(ceu_c, '=== CEU_VECTOR_C ===',   ceu_vector_c)
ceu_c = SUB(ceu_c, '=== CEU_POOL_C ===',     ceu_pool_c)
PAK = {
    lua_exe = '?',
    ceu_ver = '?',
    ceu_git = '?',
    files = {
        ceu_c = ceu_c,
    }
}


dofile 'lines.lua'
dofile 'parser.lua'

dofile 'ast.lua'

AST.dump(AST.root)

-- DBG,ASR = DBG1,ASR1
-- dofile 'adjs.lua'
-- dofile 'types.lua'
-- dofile 'exps.lua'
-- dofile 'dcls.lua'
-- dofile 'inlines.lua'
-- dofile 'consts.lua'
-- dofile 'fins.lua'
-- dofile 'spawns.lua'
-- dofile 'stmts.lua'
-- dofile 'inits.lua'
-- dofile 'ptrs.lua'
-- dofile 'scopes.lua'
-- dofile 'tight_.lua'
-- dofile 'props_.lua'
-- dofile 'trails.lua'
-- dofile 'labels.lua'
-- dofile 'vals.lua'
-- dofile 'multis.lua'
-- dofile 'mems.lua'
-- dofile 'codes.lua'

-- DBG,ASR = DBG1,ASR1

