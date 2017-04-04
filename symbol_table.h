#include "id_size.h"

#define OFFSET         kind_props.var_par_props.offset
#define NEXT_PARAM     kind_props.var_par_props.kind_props.param_props.next_param
#define BOUND_VAR      kind_props.var_par_props.kind_props.param_props.bound_var
#define FIRST_QUAD_LBL kind_props.subpr_props.first_quad_lbl
#define FIRST_PARAM    kind_props.subpr_props.first_param
#define PARAM_NUM      kind_props.subpr_props.param_num



enum{prog, proc, func, temp_var, var, in_param, inout_param};


typedef struct symbol{
        char name[ID_SIZE];
        int kind;
        union{
              struct{
                     int offset;
                     union{
                           struct{
                                  struct symbol *next_param;
                                  struct symbol *bound_var;
                           } param_props;
                     } kind_props;
              } var_par_props;
              struct{
                     int first_quad_lbl;
                     struct symbol *first_param;
                     int param_num;
              } subpr_props;
        } kind_props;
        struct scope *encl_scope;
        struct symbol *next_in_hstack;
        struct symbol *next_in_stack;
} symbol;

typedef struct scope{
        int depth;
        symbol *top_symbol;
        struct scope *next_in_stack;
} scope;


int print_symbol_table;
FILE *f_st;
scope *top_scope, *scope_bottom;


void PushSymbol(int kind, char *name);
void PopSymbol();
void PushScope();
void PopScope();
symbol *SearchSymbolTable(char *name);
void PrintSymbolTable();
