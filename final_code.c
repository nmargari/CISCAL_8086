#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symbol_table.h"
#include "intermediate_code.h"
#include "final_code.h"


#define VAR_SIZE 2


void GNLVCode(char *v)
{

     int i;
     symbol *sym;

     sym = SearchSymbolTable(v);
     fprintf(f_fc, "\tmov\tsi, word ptr [bp + 4]\n");
     for(i = 0; i < top_scope->depth - sym->encl_scope->depth - 1; i++) fprintf(f_fc, "\tmov\tsi, word ptr [si + 4]\n");
     fprintf(f_fc, "\tmov\tax, %d\n", sym->OFFSET * VAR_SIZE);
     fprintf(f_fc, "\tadd\tsi, ax\n");

}

void Load(char *v, char *r)
{

     symbol *sym;

     if(v[0] >= '0' && v[0] <= '9') fprintf(f_fc, "\tmov\t%s, %s\n", r, v);
     else{
          sym = SearchSymbolTable(v);
          if(sym->encl_scope->depth == top_scope->depth){
             if(sym->kind < inout_param) fprintf(f_fc, "\tmov\t%s, word ptr [bp + %d]\n", r, sym->OFFSET * VAR_SIZE);
             else{
                  fprintf(f_fc, "\tmov\tsi, word ptr [bp + %d]\n", sym->OFFSET * VAR_SIZE);
                  fprintf(f_fc, "\tmov\t%s, word ptr [si]\n", r);
             }
          }
          else{
               GNLVCode(v);
               if(sym->kind == inout_param) fprintf(f_fc, "\tmov\tsi, word ptr [si]\n");
               fprintf(f_fc, "\tmov\t%s, word ptr [si]\n", r);
          }
     }

}

void Store(char *r, char *v){

     symbol *sym;

     sym = SearchSymbolTable(v);
     if(sym->encl_scope->depth == top_scope->depth){
        if(sym->kind < inout_param) fprintf(f_fc, "\tmov\tword ptr [bp + %d], %s\n", sym->OFFSET * VAR_SIZE, r);
        else{
             fprintf(f_fc, "\tmov\tsi, word ptr [bp + %d]\n", sym->OFFSET * VAR_SIZE);
             fprintf(f_fc, "\tmov\tword ptr [si], %s\n", r);
        }
     }
     else{
          GNLVCode(v);
          if(sym->kind == inout_param) fprintf(f_fc, "\tmov\tsi, word ptr [si]\n");
          fprintf(f_fc, "\tmov\tword ptr [si], %s\n", r);
     }

}

void GenFinalCode(){

     int lbl, frame_length = 8, i;
     symbol *sym;
     quad *q;

     if(top_scope->top_symbol->kind > func){
        frame_length += top_scope->next_in_stack->top_symbol->PARAM_NUM * VAR_SIZE;
        if(top_scope->top_symbol->kind < in_param) frame_length -= top_scope->top_symbol->OFFSET * VAR_SIZE;
     }
     for(lbl = top_scope->next_in_stack->top_symbol->FIRST_QUAD_LBL; lbl < NextQuad(); lbl++){
           fprintf(f_fc, "L%d:\n", lbl);
           q = SearchQuadTable(lbl);
           if(strcmp(q->cmd, ":=") == 0){
                             Load(q->arg1, "ax");
                             Store("ax", q->arg3);
           }
           else if(strcmp(q->cmd, "print") == 0){
                Load(q->arg1, "ax");
                fprintf(f_fc, "\tputi\n");
                fprintf(f_fc, "\tputcr\n");
           }
           else if(strcmp(q->cmd, "jump") == 0){
                fprintf(f_fc, "\tjmp\tL%s\n", q->arg3);
           }
           else if(strcmp(q->cmd, ">") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tcmp\tax, dx\n");
                fprintf(f_fc, "\tjg\tL%s\n", q->arg3);
           }
           else if(strcmp(q->cmd, ">=") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tcmp\tax, dx\n");
                fprintf(f_fc, "\tjge\tL%s\n", q->arg3);
           }
           else if(strcmp(q->cmd, "<") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tcmp\tax, dx\n");
                fprintf(f_fc, "\tjl\tL%s\n", q->arg3);
           }
           else if(strcmp(q->cmd, "<=") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tcmp\tax, dx\n");
                fprintf(f_fc, "\tjle\tL%s\n", q->arg3);
           }
           else if(strcmp(q->cmd, "=") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tcmp\tax, dx\n");
                fprintf(f_fc, "\tje\tL%s\n", q->arg3);
           }
           else if(strcmp(q->cmd, "<>") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tcmp\tax, dx\n");
                fprintf(f_fc, "\tjne\tL%s\n", q->arg3);
           }
           else if(strcmp(q->cmd, "+") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tadd\tax, dx\n");
                Store("ax", q->arg3);
           }
           else if(strcmp(q->cmd, "-") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "dx");
                fprintf(f_fc, "\tsub\tax, dx\n");
                Store("ax", q->arg3);
           }
           else if(strcmp(q->cmd, "*") == 0){
                Load(q->arg1, "ax");
                Load(q->arg2, "cx");
                fprintf(f_fc, "\timul\tcx\n");
                Store("ax", q->arg3);
           }
           else if(strcmp(q->cmd, "/") == 0){
                Load(q->arg1, "ax");
                fprintf(f_fc, "\tcwd\n");
                Load(q->arg2, "cx");
                fprintf(f_fc, "\tidiv\tcx\n");
                Store("ax", q->arg3);
           }
           else if(strcmp(q->cmd, "retv") == 0){
                Load(q->arg1, "ax");
                fprintf(f_fc, "\tmov\tsi, word ptr [bp + 6]\n");
                fprintf(f_fc, "\tmovi\tword ptr [si], ax\n");
           }
           else if(strcmp(q->cmd, "par") == 0){
                sym = SearchSymbolTable(q->arg1);
                if(strcmp(q->arg2, "CV") == 0){
                                   Load(q->arg1, "ax");
                                   fprintf(f_fc, "\tpush\tax\n");
                }
                else if(strcmp(q->arg2, "CV") == 0){
                     if(sym->encl_scope->depth == top_scope->depth){
                                               if(sym->kind < inout_param) fprintf(f_fc, "\tlea\tsi, word ptr [bp + %d]\n", sym->OFFSET * VAR_SIZE);
                                               else fprintf(f_fc, "\tmov\tsi, word ptr [bp + %d]\n", sym->OFFSET * VAR_SIZE);
                     }
                     else{
                          GNLVCode(q->arg1);
                          if(sym->kind < inout_param) fprintf(f_fc, "\tlea\tsi, word ptr [si + %d]\n", sym->OFFSET * VAR_SIZE);
                          else fprintf(f_fc, "\tmov\tsi, word ptr [si + %d]\n", sym->OFFSET * VAR_SIZE);
                     }
                     fprintf(f_fc, "\tpush\tsi\n");
                }
                else{
                     fprintf(f_fc, "\tlea\tsi, word ptr [bp + %d]\n", sym->OFFSET * VAR_SIZE);
                     fprintf(f_fc, "\tpush\tsi\n");
                }
           }
           else if(strcmp(q->cmd, "call") == 0){
                sym = SearchSymbolTable(q->arg3);
                if(sym->kind < func) fprintf(f_fc, "\tsub\tsp, 2\n");
                if(top_scope->next_in_stack->depth < sym->encl_scope->depth) fprintf(f_fc, "\tpush\tbp\n");
                else if(top_scope->next_in_stack->depth < sym->encl_scope->depth) fprintf(f_fc, "\tpush\tword ptr [bp + 4]\n");
                else{
                     fprintf(f_fc, "\tmov\tsi, word ptr [bp + 4]\n");
                     for(i = 0; i < top_scope->next_in_stack->depth - sym->encl_scope->depth - 1; i++) fprintf(f_fc, "\tmov\tsi, word ptr [si + 4]\n");
                     fprintf(f_fc, "\tpush\tword ptr [si + 4]\n");
                }
                fprintf(f_fc, "\tcall\tnear ptr _%s_%d\n", sym->name, sym->encl_scope->depth);
                fprintf(f_fc, "\tadd\tsp, %d\n", sym->PARAM_NUM * VAR_SIZE + 4);
           }
           else if(strcmp(q->cmd, "begin_block") == 0){
                fprintf(f_fc, "_%s_%d\tproc\tnear\n", top_scope->next_in_stack->top_symbol->name, top_scope->next_in_stack->depth);
                fprintf(f_fc, "\tpush\tbp\n");
                fprintf(f_fc, "\tmov\tbp, sp\n");
                fprintf(f_fc, "\tsub\tsp, %d\n", frame_length);
           }
           else if(strcmp(q->cmd, "end_block") == 0){
                fprintf(f_fc, "%s_%d_END:\n", top_scope->next_in_stack->top_symbol->name, top_scope->next_in_stack->depth);
                fprintf(f_fc, "\tmov\tsp, bp\n");
                fprintf(f_fc, "\tpop\tbp\n");
                fprintf(f_fc, "\tret\n");
                fprintf(f_fc, "_%s_%d\tendp\n", top_scope->next_in_stack->top_symbol->name, top_scope->next_in_stack->depth);
           }
           else fprintf(f_fc, "\tjmp\t%s_%d_END\n", top_scope->next_in_stack->top_symbol->name, top_scope->next_in_stack->depth);
           fprintf(f_fc, "\n");
     }

}
