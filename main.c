#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "symbol_table.h"
#include "intermediate_code.h"
#include "final_code.h"
#include "main.h"



int main(int argc, char **argv){

    char *ic_file_name, *fc_file_name, *st_file_name;

    ic_file_name = (char *) malloc((strlen(argv[1]) - 3) * sizeof(char));
    st_file_name = (char *) malloc((strlen(argv[1]) - 3) * sizeof(char));
    fc_file_name = (char *) malloc((strlen(argv[1]) - 3) * sizeof(char));
    strncpy(ic_file_name, argv[1], strlen(argv[1]) - 3);
    strcpy(st_file_name, ic_file_name);
    strcpy(fc_file_name, ic_file_name);
    strcat(ic_file_name, "i");
    strcat(st_file_name, "t");
    strcat(fc_file_name, "asm");
    print_quad_table = print_symbol_table = 0;
    if(argc > 2){
            if(strcmp(argv[2], "-i") == 0){
                               print_quad_table = 1;
                               f_ic = fopen(ic_file_name, "w");
            }
            else if(strcmp(argv[2], "-t") == 0){
                                    print_symbol_table = 1;
                                    f_st = fopen(st_file_name, "w");
            }
            if(argc > 3){
                    if(strcmp(argv[3], "-i") == 0 && !print_quad_table){
                                       print_quad_table = 1;
                                       f_ic = fopen(ic_file_name, "w");
                    }
                    else if(strcmp(argv[3], "-t") == 0 && !print_symbol_table){
                                            print_symbol_table = 1;
                                            f_st = fopen(st_file_name, "w");
                    }
            }
    }
    f_fc = fopen(fc_file_name, "w");
    top_scope = scope_bottom = (scope *) malloc(sizeof(scope));
    top_scope->depth = -1;
    PushScope();
    f = fopen(argv[1], "r");
    return_has_been_used = 1;
    exit_can_be_used = 0;
    line_cntr = 1;
    tk = Lexer();
    Program();
    fclose(f);
    if(tk != _eof) Error(parse, "EOF expected");
    PrintQuadTable();
    printf("Program '%s' was compiled successfully.\n", top_scope->top_symbol->name);
    PopScope();
    if(print_quad_table) fclose(f_ic);
    if(print_symbol_table) fclose(f_st);
    fclose(f_fc);
    return 0;

}

void Program(){

     if(tk == _program){
           tk = Lexer();
           if(tk == _id){
                 PushSymbol(prog, s);
                 PushScope();
                 fprintf(f_fc, "\t.xlist\n");
                 fprintf(f_fc, "\tinclude\tstdlib.a\n");
                 fprintf(f_fc, "\tincludelib\tstdlib.lib\n");
                 fprintf(f_fc, "\t.list\n\n");
                 fprintf(f_fc, "xseg\tsegment\tpublic 'code'\n");
                 fprintf(f_fc, "\tassume\tcs:xseg, ds:xseg, ss:xseg\n");
                 fprintf(f_fc, "\torg\t100h\n\n");
                 fprintf(f_fc, "main\tproc\tnear\n");
                 fprintf(f_fc, "\tcall\tnear ptr _%s_%d\n", top_scope->next_in_stack->top_symbol->name, top_scope->next_in_stack->depth);
                 fprintf(f_fc, "\tmov\tax, 4C00h\n");
                 fprintf(f_fc, "\tint\t21h\n");
                 fprintf(f_fc, "main\tendp\n\n");
                 tk = Lexer();
                 Block();
                 fprintf(f_fc, "xseg\tends\n");
                 fprintf(f_fc, "\tend\tmain\n");
           }
           else Error(parse, "program name expected");
     }
     else Error(parse, "keyword 'program' expected");

}

void Block(){

     sequence sq;

     if(tk == _lbrace){
           tk = Lexer();
           Declarations();
           Subprograms();
           if(top_scope->next_in_stack->top_symbol->kind == func) return_has_been_used = 0;
           top_scope->next_in_stack->top_symbol->FIRST_QUAD_LBL = NextQuad();
           GenQuad("begin_block", top_scope->next_in_stack->top_symbol->name, "_", "_");
           sq = Sequence();
           if(tk == _rbrace){
                 if(top_scope->next_in_stack->top_symbol->kind == func && !return_has_been_used) Error(semantic, "function's block must contain 'return' statement");
                 PrintSymbolTable();
                 Backpatch(sq.next_, NextQuad());
                 if(top_scope->next_in_stack->top_symbol->kind == prog) GenQuad("halt", "_", "_", "_");
                 GenQuad("end_block", top_scope->next_in_stack->top_symbol->name, "_", "_");
                 GenFinalCode();
                 PopScope();
                 tk = Lexer();
           }
           else Error(parse, "'}' expected");
     }
     else Error(parse, "'{' expected");

}

void Declarations(){

     if(tk == _declare){
           tk = Lexer();
           VarList();
           if(tk == _enddeclare) tk = Lexer();
           else Error(parse, "keyword 'enddeclare' expected");
     }

}

void VarList(){

     symbol *sym;

     if(tk == _id){
           PushSymbol(var, s);
           tk = Lexer();
           while(tk == _comma){
                    tk = Lexer();
                    if(tk == _id){
                          if((sym = SearchSymbolTable(s)) != NULL && sym->encl_scope == top_scope) Error(semantic, "variable already declared in this scope");
                          PushSymbol(var, s);
                          tk = Lexer();
                    }
                    else Error(parse, "variable name expected");
           }
     }

}

void Subprograms(){

     int kind;

     while(tk == _procedure || tk == _function){
              kind = tk == _procedure ? proc : func;
              tk = Lexer();
              Func(kind);
     }

}

void Func(int kind){

     symbol *sym;

     if(tk == _id){
           if((sym = SearchSymbolTable(s)) != NULL && sym->encl_scope == top_scope) Error(semantic, "procedure/function already declared in this scope");
           PushSymbol(kind, s);
           tk = Lexer();
           FuncBody();
     }
     else Error(parse, "procedure/function name expected");

}

void FuncBody(){

     PushScope();
     FormalPars();
     Block();

}

void FormalPars(){

     int i;
     symbol *sym;

     if(tk == _lparen){
           tk = Lexer();
           FormalParList();
           sym = top_scope->next_in_stack->top_symbol->FIRST_PARAM;
           for(i = top_scope->next_in_stack->top_symbol->PARAM_NUM - 1; i >= 0; i--){
                 sym->OFFSET += i;
                 sym = sym->NEXT_PARAM;
           }
           if(tk == _rparen) tk = Lexer();
           else Error(parse, "')' expected");
     }

}

void FormalParList(){

     int kind;

     if(tk == _in || tk == _inout){
           top_scope->next_in_stack->top_symbol->PARAM_NUM++;
           kind = tk == _in ? in_param : inout_param;
           tk = Lexer();
           FormalParItem(kind);
           while(tk == _comma){
                    tk = Lexer();
                    if(tk == _in || tk == _inout){
                          top_scope->next_in_stack->top_symbol->PARAM_NUM++;
                          kind = tk == _in ? in_param : inout_param;
                          tk = Lexer();
                          FormalParItem(kind);
                    }
                    else Error(parse, "one of the keywords 'in', 'inout' expected");
           }
     }
     else Error(parse, "one of the keywords 'in', 'inout' expected");

}

void FormalParItem(int kind){

     symbol *sym;

     if(tk == _id){
           if((sym = SearchSymbolTable(s)) != NULL && sym->encl_scope == top_scope) Error(semantic, "parameter already declared");
           PushSymbol(kind, s);
           tk = Lexer();
     }
     else Error(parse, "formal parameter name expected");

}

sequence Sequence(){

         sequence sq;
         statement st;
         lbl_list_node *l;

         st = Statement();
         l = st.exit_next_;
         while(tk == _scolon){
                  Backpatch(st.next_, NextQuad());
                  tk = Lexer();
                  st = Statement();
                  l = Merge(l, st.exit_next_);
         }
         sq.next_ = st.next_;
         sq.exit_next_ = l;
         return sq;

}

statement Statement(){

          symbol *sym;
          statement st;
          if_stat is;
          exit_stat es;

          st.next_ = EmptyList();
          st.exit_next_ = EmptyList();
          if(tk == _id){
                if((sym = SearchSymbolTable(s)) == NULL || sym->kind == temp_var) Error(semantic, "variable not declared");
                else if(sym->kind < var) Error(semantic, "assignments must be made to variables");
                AssignmentStat();
          }
          else if(tk == _if){
               is = IfStat();
               st.next_ = is.next_;
               st.exit_next_ = is.exit_next_;
          }
          else if(tk == _do) WhileStat();
          else if(tk == _print) PrintStat();
          else if(tk == _return) ReturnStat();
          else if(tk == __exit){
                     es = ExitStat();
                     st.exit_next_ = es.exit_next_;
          }
          return st;

}

void AssignmentStat(){

     expression e;
     char arg3[ID_SIZE];

     strcpy(arg3, s);
     tk = Lexer();
     if(tk == _asgn){
           tk = Lexer();
           e = Expression();
           GenQuad(":=", e.place_, "_", arg3);
     }
     else Error(parse, "':=' expected");

}

if_stat IfStat(){

        if_stat is;
        condition c;
        braces_or_stat bos;
        else_part ep;
        lbl_list_node *l;

        tk = Lexer();
        if(tk == _lparen){
              tk = Lexer();
              c = Condition();
              if(tk == _rparen){
                    Backpatch(c.true_, NextQuad());
                    tk = Lexer();
                    bos = BracesOrStat();
                    l = Merge(bos.next_, MakeList(NextQuad()));
                    GenQuad("jump", "_", "_", "_");
                    Backpatch(c.false_, NextQuad());
                    ep = ElsePart();
                    is.next_ = Merge(l, ep.next_);
                    is.exit_next_ = Merge(bos.exit_next_, ep.exit_next_);
              }
              else Error(parse, "')' expected");
        }
        else Error(parse, "'(' expected");
        return is;

}

braces_or_stat BracesOrStat(){

               braces_or_stat bos;
               braces_seq bs;
               statement st;

               if(tk == _lbrace){
                     tk = Lexer();
                     bs = BracesSeq();
                     bos.next_ = bs.next_;
                     bos.exit_next_ = bs.exit_next_;
               }
               else{
                    st = Statement();
                    bos.next_ = st.next_;
                    bos.exit_next_ = st.exit_next_;
               }
               return bos;

}

braces_seq BracesSeq(){

           braces_seq bs;
           sequence sq;

           sq = Sequence();
           bs.next_ = sq.next_;
           bs.exit_next_ = sq.exit_next_;
           if(tk == _rbrace) tk = Lexer();
           else Error(parse, "'}' expected");
           return bs;

}

else_part ElsePart(){

          else_part ep;
          braces_or_stat bos;

          ep.next_ = EmptyList();
          ep.exit_next_ = EmptyList();
          if(tk == _else){
                tk = Lexer();
                bos = BracesOrStat();
                ep.next_ = bos.next_;
                ep.exit_next_ = bos.exit_next_;
          }
          return ep;

}

void WhileStat(){

     braces_or_stat bos;
     condition c;
     int lbl;

     lbl = NextQuad();
     exit_can_be_used++;
     tk = Lexer();
     bos = BracesOrStat();
     exit_can_be_used--;
     Backpatch(bos.next_, NextQuad());
     if(tk == _while){
           tk = Lexer();
           if(tk == _lparen){
                 tk = Lexer();
                 c = Condition();
                 if(tk == _rparen){
                       Backpatch(c.true_, lbl);
                       Backpatch(c.false_, NextQuad());
                       Backpatch(bos.exit_next_, NextQuad());
                       tk = Lexer();
                 }
                 else Error(parse, "')' expected");
           }
           else Error(parse, "'(' expected");
     }
     else Error(parse, "keyword 'while' expected");

}

void PrintStat(){

     expression e;

     tk = Lexer();
     if(tk == _lparen){
           tk = Lexer();
           e = Expression();
           GenQuad("print", e.place_, "_", "_");
           if(tk == _rparen) tk = Lexer();
           else Error(parse, "')' expected");
     }
     else Error(parse, "'(' expected");

}

void ReturnStat(){

     expression e;

     if(top_scope->next_in_stack->top_symbol->kind < func) Error(semantic, "a 'return' statement cannot be used outside a function's block");
     if(!return_has_been_used) return_has_been_used = 1;
     tk = Lexer();
     if(tk == _lparen){
           tk = Lexer();
           e = Expression();
           GenQuad("retv", e.place_, "_", "_");
           if(tk == _rparen){
                 GenQuad("ret", "_", "_", "_");
                 tk = Lexer();
           }
           else Error(parse, "')' expected");
     }
     else Error(parse, "'(' expected");

}

exit_stat ExitStat(){

          exit_stat es;

          if(!exit_can_be_used) Error(semantic, "an 'exit' statement cannot be used outside a while loop");
          es.exit_next_ = MakeList(NextQuad());
          GenQuad("jump", "_", "_", "_");
          tk = Lexer();
          return es;

}

condition Condition(){

          condition c;
          bool_term bt1, bt2;

          bt1 = BoolTerm();
          while(tk == _or){
                   Backpatch(bt1.false_, NextQuad());
                   tk = Lexer();
                   bt2 = BoolTerm();
                   bt1.true_ = Merge(bt1.true_, bt2.true_);
                   bt1.false_ = bt2.false_;
          }
          c.true_ = bt1.true_;
          c.false_ = bt1.false_;
          return c;

}

bool_term BoolTerm(){

          bool_term bt;
          bool_factor bf1, bf2;

          bf1 = BoolFactor();
          while(tk == _and){
                   Backpatch(bf1.true_, NextQuad());
                   tk = Lexer();
                   bf2 = BoolFactor();
                   bf1.true_ = bf2.true_;
                   bf1.false_ = Merge(bf1.false_, bf2.false_);
          }
          bt.true_ = bf1.true_;
          bt.false_ = bf1.false_;
          return bt;

}

bool_factor BoolFactor(){

            bool_factor bf;
            condition c;
            relational_oper ro;
            expression e1, e2;

            if(tk == _not){
                  tk = Lexer();
                  if(tk == _lbracket){
                        tk = Lexer();
                        c = Condition();
                        bf.true_ = c.false_;
                        bf.false_ = c.true_;
                        if(tk == _rbracket) tk = Lexer();
                        else Error(parse, "']' expected");
                  }
                  else Error(parse, "'[' expected");
            }
            else if(tk == _lbracket){
                       tk = Lexer();
                       c = Condition();
                       bf.true_ = c.true_;
                       bf.false_ = c.false_;
                       if(tk == _rbracket) tk = Lexer();
                       else Error(parse, "']' expected");
            }
            else{
                 e1 = Expression();
                 ro = RelationalOper();
                 e2 = Expression();
                 bf.true_ = MakeList(NextQuad());
                 if(ro.name_ == _eq) GenQuad("=", e1.place_, e2.place_, "_");
                 else if(ro.name_ == _dif) GenQuad("<>", e1.place_, e2.place_, "_");
                 else if(ro.name_ == _ls) GenQuad("<", e1.place_, e2.place_, "_");
                 else if(ro.name_ == _lseq) GenQuad("<=", e1.place_, e2.place_, "_");
                 else if(ro.name_ == _gr) GenQuad(">", e1.place_, e2.place_, "_");
                 else GenQuad(">=", e1.place_, e2.place_, "_");
                 bf.false_ = MakeList(NextQuad());
                 GenQuad("jump", "_", "_", "_");
            }
            return bf;

}

relational_oper RelationalOper(){

                relational_oper ro;

                if(tk == _eq || tk == _dif || tk == _ls || tk == _lseq || tk == _gr || tk == _greq){
                      ro.name_ = tk;
                      tk = Lexer();
                }
                else Error(parse, "'=', '<>', '<', '<=', '>' or '>=' expected");
                return ro;

}

expression Expression(){

           expression e;
           optional_sign os;
           add_oper ao;
           term t1, t2;
           char *temp;

           os = OptionalSign();
           t1 = Term();
           if(os.name_ == _sub){
                 temp = NewTemp();
                 PushSymbol(temp_var, temp);
                 GenQuad("*", "-1", t1.place_, temp);
                 strcpy(t1.place_, temp);
           }
           while(tk == _add || tk == _sub){
                    ao = AddOper();
                    t2 = Term();
                    temp = NewTemp();
                    PushSymbol(temp_var, temp);
                    if(ao.name_ == _add) GenQuad("+", t1.place_, t2.place_, temp);
                    else GenQuad("-", t1.place_, t2.place_, temp);
                    strcpy(t1.place_, temp);
           }
           strcpy(e.place_, t1.place_);
           return e;

}

term Term(){

     term t;
     mul_oper mo;
     factor f1, f2;
     char *temp;

     f1 = Factor();
     while(tk == _mul || tk == _div){
              mo = MulOper();
              f2 = Factor();
              temp = NewTemp();
              PushSymbol(temp_var, temp);
              if(mo.name_ == _mul) GenQuad("*", f1.place_, f2.place_, temp);
              else GenQuad("/", f1.place_, f2.place_, temp);
              strcpy(f1.place_, temp);
     }
     strcpy(t.place_, f1.place_);
     return t;

}

factor Factor(){

       symbol *sym;
       factor f;
       expression e;
       char *temp;

       if(tk == _const){
             strcpy(f.place_, s);
             tk = Lexer();
       }
       else if(tk == _lparen){
            tk = Lexer();
            e = Expression();
            strcpy(f.place_, e.place_);
            if(tk == _rparen) tk = Lexer();
            else Error(parse, "')' expected");
       }
       else if(tk == _id){
            if((sym = SearchSymbolTable(s)) == NULL || sym->kind == temp_var) Error(semantic, "variable not declared");
            else if(sym->kind < func) Error(semantic, "the name of the program or of a procedure cannot be used as a factor");
            strcpy(f.place_, s);
            tk = Lexer();
            IdTail(sym);
            if(sym->kind == func){
                         temp = NewTemp();
                         PushSymbol(temp_var, temp);
                         GenQuad("par", temp, "RET", "_");
                         GenQuad("call", "_", "_", f.place_);
                         strcpy(f.place_, temp);
            }
       }
       else Error(parse, "factor expected (constant, variable, function call or expression in parentheses)");
       return f;

}

void IdTail(symbol *sym){

     if(tk == _lparen){
           if(sym->kind > func) Error(semantic, "a variable cannot have parameters");
           tk = Lexer();
           ActualPars(sym);
     }
     else if(sym->kind < var && sym->PARAM_NUM > 0) Error(semantic, "in a procedure/function call there must be its actual parameters");

}

void ActualPars(symbol *sym){

     ActualParList(sym);
     if(tk == _rparen) tk = Lexer();
     else Error(parse, "')' expected");

}

void ActualParList(symbol *sym){

     sym = sym->FIRST_PARAM;
     ActualParItem(sym);
     for(sym = sym->NEXT_PARAM; tk == _comma; sym = sym->NEXT_PARAM){
             if(sym == NULL) Error(semantic, "wrong actual parameters number");
             tk = Lexer();
             ActualParItem(sym);
     }
     if(sym != NULL) Error(semantic, "wrong actual parameters number");

}

void ActualParItem(symbol *sym){

     symbol *bv;
     expression e;

     if(tk == _in){
           if(sym->kind != in_param) Error(semantic, "formal parameter is of type 'inout'");
           sym->BOUND_VAR = NULL;
           tk = Lexer();
           e = Expression();
           GenQuad("par", e.place_, "CV", "_");
     }
     else if(tk == _inout){
                if(sym->kind != inout_param) Error(semantic, "formal parameter is of type 'in'");
                tk = Lexer();
                if(tk == _id){
                      if((bv = SearchSymbolTable(s)) == NULL || bv->kind == temp_var) Error(semantic, "variable not declared");
                      else if(bv->kind < var) Error(semantic, "an actual parameter of type 'inout' must be a variable");
                      sym->BOUND_VAR = bv;
                      GenQuad("par", s, "REF", "_");
                      tk = Lexer();
                }
                else Error(parse, "variable name expected");
     }
     else Error(parse, "one of the keywords 'in', 'inout' expected");

}

optional_sign OptionalSign(){

              optional_sign os;
              add_oper ao;

              os.name_ = _add;
              if(tk == _add || tk == _sub){
                    ao = AddOper();
                    os.name_ = ao.name_;
              }
              return os;

}

add_oper AddOper(){

         add_oper ao;

         if(tk == _add || tk == _sub){
               ao.name_ = tk;
               tk = Lexer();
         }
         else Error(parse, "'+' or '-' expected");
         return ao;

}

mul_oper MulOper(){

         mul_oper mo;

         if(tk == _mul || tk == _div){
               mo.name_ = tk;
               tk = Lexer();
         }
         else Error(parse, "'*' or '/' expected");
         return mo;

}

void Error(int kind, char e[51]){

     if(tk == _error) printf("lexical error at line %d: << %s >>\n", line_cntr, s);
     if(kind == parse) printf("parse error at line %d: %s\n", line_cntr, e);
     else printf("semantic error at line %d: %s\n", line_cntr, e);
     exit(1);

}
