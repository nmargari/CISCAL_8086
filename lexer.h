#include "id_size.h"



enum{state0, state1, state2, state3, state4, state5, state6, state7, state8,
     _id, _const, _add, _sub, _mul, _div, _eq, _ls, _lseq, _dif, _gr, _greq,
     _asgn, _comma, _scolon, _lparen, _rparen, _lbracket, _rbracket,
     _lbrace, _rbrace, _eof, _error,
     _and, _declare, _do, _else, _enddeclare, __exit, _procedure, _function,
     _print, _if, _in, _inout, _not, _program, _or, _return, _while};


int line_cntr;
char s[ID_SIZE];
FILE *f;


int Lexer();
