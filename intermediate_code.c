#include <stdio.h>
#include <string.h>

#include "intermediate_code.h"


#define QHT_SIZE 251



int cur_lbl = 0;
quad *quad_htable[QHT_SIZE];


int QuadHFunction(int lbl){

    return lbl % QHT_SIZE;

}

void InsertQuad(quad *q){

     q->next_in_hstack = quad_htable[QuadHFunction(q->lbl)];
     quad_htable[QuadHFunction(q->lbl)] = q;

}

quad *SearchQuadTable(int lbl){

     quad *q;

     for(q = quad_htable[QuadHFunction(lbl)]; q != NULL && q->lbl != lbl; q = q->next_in_hstack);
     return q;

}

void PrintQuadTable(){

     int lbl;
     quad *q;

     if(print_quad_table){
        for(lbl = 0; lbl < cur_lbl; lbl++){
                q = SearchQuadTable(lbl);
                fprintf(f_ic, "%d:\t%s, %s, %s, %s\n", lbl, q->cmd, q->arg1, q->arg2, q->arg3);
        }
     }

}

int NextQuad(){

    return cur_lbl;

}

void GenQuad(char *cmd, char *arg1, char *arg2, char *arg3){

     quad *q = (quad *) malloc(sizeof(quad));

     q->lbl = cur_lbl++;
     strcpy(q->cmd, cmd);
     strcpy(q->arg1, arg1);
     strcpy(q->arg2, arg2);
     strcpy(q->arg3, arg3);
     InsertQuad(q);

}

char *NewTemp(){

     static int temp_cntr = 1;
     char t[9] = "T_", t_num[7];
     
     sprintf(t_num, "%d", temp_cntr++);
     strcat(t, t_num);
     return t;

}

lbl_list_node *EmptyList(){

               lbl_list_node *n = (lbl_list_node *) malloc(sizeof(lbl_list_node));

               n->lbl = -1;
               n->next_node = NULL;
               return n;

}

lbl_list_node *MakeList(int lbl){

               lbl_list_node *n = (lbl_list_node *) malloc(sizeof(lbl_list_node));

               n->lbl = lbl;
               n->next_node = NULL;
               return n;

}

lbl_list_node *Merge(lbl_list_node *first_node_1, lbl_list_node *first_node_2){

               lbl_list_node *n;

               for(n = first_node_1; n->next_node != NULL; n = n->next_node);
               n->next_node = first_node_2;
               return first_node_1;

}

void Backpatch(lbl_list_node *first_node, int lbl){

     char l[ID_SIZE];
     lbl_list_node *n;

     sprintf(l, "%d", lbl);
     for(n = first_node; n != NULL; n = n->next_node)
           if(n->lbl >= 0) strcpy(SearchQuadTable(n->lbl)->arg3, l);

}
