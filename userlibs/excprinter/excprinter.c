// Include MicroPython API.
#include "py/runtime.h"

// Used to get the time in the Timer class example.
#include "py/mphal.h"

#include "py/mpstate.h"
#include "py/bc.h"
#include "py/objfun.h"


static void append_stack_frame(mp_code_state_t *code_state, mp_obj_t *res) {
    const byte *ip = code_state->fun_bc->bytecode;
    MP_BC_PRELUDE_SIG_DECODE(ip);
    MP_BC_PRELUDE_SIZE_DECODE(ip);
    const byte *line_info_top = ip + n_info;
    const byte *bytecode_start = ip + n_info + n_cell;
    size_t bc = code_state->ip - bytecode_start;
    qstr block_name = mp_decode_uint_value(ip);
    for (size_t i = 0; i < 1 + n_pos_args + n_kwonly_args; ++i) {
        ip = mp_decode_uint_skip(ip);
    }
#if MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE
    block_name = code_state->fun_bc->context->constants.qstr_table[block_name];
    qstr source_file = code_state->fun_bc->context->constants.qstr_table[0];
#else
    qstr source_file = code_state->fun_bc->context->constants.source_file;
#endif
    size_t source_line = mp_bytecode_get_source_line(ip, line_info_top, bc);

    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 100, &print);
#if MICROPY_ENABLE_SOURCE_LINE
    mp_printf(&print, "%q:%d", source_file, (int)source_line);
#else
    mp_printf(&print, "%q", values[i]);
#endif
    // the block name can be NULL if it's unknown
    if (block_name != MP_QSTRnull) {
        mp_printf(&print, " %q", block_name);
    }

    mp_obj_list_append(*res, mp_obj_new_str_from_utf8_vstr(&vstr));
}

static mp_obj_t excprinter_get_current_callstack() {
    mp_code_state_t *code_state = mp_state_ctx.thread.current_code_state;

    mp_obj_t res = mp_obj_new_list(0, NULL);

    while (code_state != NULL) {
        append_stack_frame(code_state, &res);
        code_state = code_state->prev_state;
    }
    
    return res;
}

static mp_obj_t excprinter_get_callstack(mp_obj_t exc) {

    if (mp_obj_is_exception_instance(exc)) {
        mp_obj_t res = mp_obj_new_list(0, NULL);
        
        size_t n, *values;
        mp_obj_exception_get_traceback(exc, &n, &values);
        if (n > 0) {
            assert(n % 3 == 0);

            for (int i = n - 3; i >= 0; i -= 3) {
                vstr_t vstr;
                mp_print_t print;
                vstr_init_print(&vstr, 100, &print);

#if MICROPY_ENABLE_SOURCE_LINE
                mp_printf(&print, "%q:%d", values[i], (int)values[i + 1]);
#else
                mp_printf(&print, "%q", values[i]);
#endif
                // the block name can be NULL if it's unknown
                qstr block = values[i + 2];
                if (block != MP_QSTRnull) {
                    mp_printf(&print, " %q", block);
                }

                mp_obj_list_append(res, mp_obj_new_str_from_utf8_vstr(&vstr));
            }
        }

        return res;
    }
    else
    {
        return mp_const_none;
    }
}

static MP_DEFINE_CONST_FUN_OBJ_1(excprinter_get_callstack_obj, excprinter_get_callstack);
static MP_DEFINE_CONST_FUN_OBJ_0(excprinter_get_current_callstack_obj, excprinter_get_current_callstack);

static const mp_rom_map_elem_t excprinter_module_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_excprinter) },
    { MP_ROM_QSTR(MP_QSTR_get_callstack), MP_ROM_PTR(&excprinter_get_callstack_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_current_callstack), MP_ROM_PTR(&excprinter_get_current_callstack_obj) },
};
static MP_DEFINE_CONST_DICT(excprinter_module_globals, excprinter_module_table);

const mp_obj_module_t excprinter_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&excprinter_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_excprinter, excprinter_cmodule);
