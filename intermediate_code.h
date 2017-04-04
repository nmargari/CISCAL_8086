#include "id_size.h"



typedef struct quad{
        int lbl;
        char cmd[ID_SIZE];
        char arg1[ID_SIZE];
        char arg2[ID_SIZE];
        char arg3[ID_SIZE];
        struct quad *next_in_hstack;
} quad;

typedef struct lbl_list_node{
        int lbl;
        struct lbl_list_node *next_node;
} lbl_list_node;


int print_quad_table;
FILE *f_ic;


quad *SearchQuadTable(int lbl);
void PrintQuadTable();
int NextQuad();
void GenQuad(char *cmd, char *arg1, char *arg2, char *arg3);
char *NewTemp();
lbl_list_node *EmptyList();
lbl_list_node *MakeList(int lbl);
lbl_list_node *Merge(lbl_list_node *first_node_1, lbl_list_node *first_node_2);
void Backpatch(lbl_list_node *first_node, int lbl);
