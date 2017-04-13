#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symbol_table.h"


#define SHT_SIZE 31



symbol *symbol_htable[SHT_SIZE];


int SymbolHFunction(char *name){

    int i, sum = 0;
    for(i = 0; name[i] != '\0'; i++) sum += name[i];
    return sum % SHT_SIZE;

}

void PushSymbol(int kind, char *name){

     symbol *sym = (symbol *) malloc(sizeof(symbol)), *sym_tmp;

     strcpy(sym->name, name);
     sym->kind = kind;
     if(kind > func){
             if(kind < in_param){
                     sym->OFFSET = -1;
                     for(sym_tmp = top_scope->top_symbol; sym_tmp->kind < temp_var && sym_tmp != top_scope->next_in_stack->top_symbol; sym_tmp = sym_tmp->next_in_stack);
                     if(sym_tmp->kind > func && sym_tmp->kind < in_param) sym->OFFSET = sym_tmp->OFFSET - 1;
             }
             else{
                  sym->OFFSET = 4;
                  if(top_scope->top_symbol->kind > func) top_scope->top_symbol->NEXT_PARAM = sym;
                  else top_scope->top_symbol->FIRST_PARAM = sym;
             }
     }
     else sym->PARAM_NUM = 0;
     sym->encl_scope = top_scope;
     sym->next_in_hstack = symbol_htable[SymbolHFunction(name)];
     symbol_htable[SymbolHFunction(name)] = sym;
     sym->next_in_stack = top_scope->top_symbol;
     top_scope->top_symbol = sym;

}

void PopSymbol(){

     symbol_htable[SymbolHFunction(top_scope->top_symbol->name)] = top_scope->top_symbol->next_in_hstack;
     top_scope->top_symbol = top_scope->top_symbol->next_in_stack;

}

void PushScope(){

     scope *sc = (scope *) malloc(sizeof(scope));

     sc->depth = top_scope->depth + 1;
     sc->top_symbol = top_scope->top_symbol;
     sc->next_in_stack = top_scope;
     top_scope = sc;

}

void PopScope(){

     while(top_scope->top_symbol != top_scope->next_in_stack->top_symbol) PopSymbol();
     top_scope = top_scope->next_in_stack;

}

symbol *SearchSymbolTable(char *name){

       symbol *sym;

       for(sym = symbol_htable[SymbolHFunction(name)]; sym != NULL && strcmp(name, sym->name) != 0; sym = sym->next_in_hstack);
       return sym;

}

void PrintSymbolTable(){

     scope *sc;
     symbol *sym, *par;

     if(print_symbol_table){
        fprintf(f_st, "~~~~~~~~~~~~~~~~~~~~~\n");
        for(sc = top_scope; sc != scope_bottom; sc = sc->next_in_stack){
               fprintf(f_st, "\n");
               for(sym = sc->top_symbol; sym != sc->next_in_stack->top_symbol; sym = sym->next_in_stack){
                       fprintf(f_st, ">\nname: %s\nkind: %d\n", sym->name, sym->kind);
                       if(sym->kind > func) fprintf(f_st, "offset: %d\n", sym->OFFSET);
                       else{
                            fprintf(f_st, "first quad label: %d\n", sym->FIRST_QUAD_LBL);
                            if(sym->kind > prog){
                                    fprintf(f_st, "formal parameters (%d):", sym->PARAM_NUM);
                                    for(par = sym->FIRST_PARAM; par != NULL; par = par->NEXT_PARAM) fprintf(f_st, " %s", par->name);
                                    fprintf(f_st, "\n");
                            }
                       }
                       fprintf(f_st, ">\n\n");
               }
               fprintf(f_st, "==========%d==========\n", sc->depth);
        }
        fprintf(f_st, "*\n\n");
     }

}
