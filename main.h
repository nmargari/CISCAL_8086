#include "id_size.h"



enum{parse, semantic};


typedef struct factor{
        char place_[ID_SIZE];
} factor;

typedef struct term{
        char place_[ID_SIZE];
} term;

typedef struct expression{
        char place_[ID_SIZE];
} expression;

typedef struct bool_factor{
        lbl_list_node *true_;
        lbl_list_node *false_;
} bool_factor;

typedef struct bool_term{
        lbl_list_node *true_;
        lbl_list_node *false_;
} bool_term;

typedef struct condition{
        lbl_list_node *true_;
        lbl_list_node *false_;
} condition;

typedef struct relational_oper{
        int name_;
} relational_oper;

typedef struct optional_sign{
        int name_;
} optional_sign;

typedef struct add_oper{
        int name_;
} add_oper;

typedef struct mul_oper{
        int name_;
} mul_oper;

typedef struct sequence{
        lbl_list_node *next_;
        lbl_list_node *exit_next_;
} sequence;

typedef struct statement{
        lbl_list_node *next_;
        lbl_list_node *exit_next_;
} statement;

typedef struct if_stat{
        lbl_list_node *next_;
        lbl_list_node *exit_next_;
} if_stat;

typedef struct braces_seq{
        lbl_list_node *next_;
        lbl_list_node *exit_next_;
} braces_seq;

typedef struct braces_or_stat{
        lbl_list_node *next_;
        lbl_list_node *exit_next_;
} braces_or_stat;

typedef struct else_part{
        lbl_list_node *next_;
        lbl_list_node *exit_next_;
} else_part;

typedef struct exit_stat{
        lbl_list_node *exit_next_;
} exit_stat;


int tk, exit_can_be_used, return_has_been_used;


void Program();
void Block();
void Declarations();
void VarList();
void Subprograms();
void Func(int kind);
void FuncBody();
void FormalPars();
void FormalParList();
void FormalParItem(int kind);
sequence Sequence();
statement Statement();
void AssignmentStat();
if_stat IfStat();
braces_or_stat BracesOrStat();
braces_seq BracesSeq();
else_part ElsePart();
void WhileStat();
void PrintStat();
void ReturnStat();
exit_stat ExitStat();
condition Condition();
bool_term BoolTerm();
bool_factor BoolFactor();
relational_oper RelationalOper();
expression Expression();
term Term();
factor Factor();
void IdTail(symbol *sym);
void ActualPars(symbol *sym);
void ActualParList(symbol *sym);
void ActualParItem(symbol *sym);
optional_sign OptionalSign();
add_oper AddOper();
mul_oper MulOper();
void Error(int kind, char e[51]);
