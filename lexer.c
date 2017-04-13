#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"



enum{letter, digit, plus, minus, asterisk, slash, backslash, equals, less, greater,
     colon, comma, semicolon, l_parenthesis, r_parenthesis, l_bracket, r_bracket,
     l_brace, r_brace, end_of_file, blank, other};


int InputToNum(char c){

    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return letter;
    else if(c >= '0' && c <= '9') return digit;
    else if(c == '+') return plus;
    else if(c == '-') return minus;
    else if(c == '*') return asterisk;
    else if(c == '/') return slash;
    else if(c == '\\') return backslash;
    else if(c == '=') return equals;
    else if(c == '<') return less;
    else if(c == '>') return greater;
    else if(c == ':') return colon;
    else if(c == ',') return comma;
    else if(c == ';') return semicolon;
    else if(c == '(') return l_parenthesis;
    else if(c == ')') return r_parenthesis;
    else if(c == '[') return l_bracket;
    else if(c == ']') return r_bracket;
    else if(c == '{') return l_brace;
    else if(c == '}') return r_brace;
    else if(c == EOF) return end_of_file;
    else if(c == ' ' || c == '\t' || c == '\n') return blank;
    else return other;

}

int Lexer(){

    int state = state0, colon_flag = 0;
    int trans_diagram[9][22] = {{state1, state2, _add,   _sub,   _mul,   _div,   state3, _eq,    state6, state7, state8, _comma, _scolon, _lparen, _rparen, _lbracket, _rbracket, _lbrace, _rbrace, _eof,   state0, _error},
                                {state1, state1, _id,    _id,    _id,    _id,    _id,    _id,    _id,    _id,    _id,    _id,    _id,     _id,     _id,     _id,       _id,       _id,     _id,     _id,    _id,    _id},
                                {_const, state2, _const, _const, _const, _const, _const, _const, _const, _const, _const, _const, _const,  _const,  _const,  _const,    _const,    _const,  _const,  _const, _const, _const},
                                {_error, _error, _error, _error, state4, _error, _error, _error, _error, _error, _error, _error, _error,  _error,  _error,  _error,    _error,    _error,  _error,  _error, _error, _error},
                                {state4, state4, state4, state4, state5, state4, state4, state4, state4, state4, state4, state4, state4,  state4,  state4,  state4,    state4,    state4,  state4,  _error, state4, state4},
                                {state4, state4, state4, state4, state5, state4, state0, state4, state4, state4, state4, state4, state4,  state4,  state4,  state4,    state4,    state4,  state4,  _error, state4, state4},
                                {_ls,    _ls,    _ls,    _ls,    _ls,    _ls,    _ls,    _lseq,  _ls,    _dif,   _ls,    _ls,    _ls,     _ls,     _ls,     _ls,       _ls,       _ls,     _ls,     _ls,    _ls,    _ls},
                                {_gr,    _gr,    _gr,    _gr,    _gr,    _gr,    _gr,    _greq,  _gr,    _gr,    _gr,    _gr,    _gr,     _gr,     _gr,     _gr,       _gr,       _gr,     _gr,     _gr,    _gr,    _gr},
                                {_error, _error, _error, _error, _error, _error, _error, _asgn,  _error, _error, _error, _error, _error,  _error,  _error,  _error,    _error,    _error,  _error,  _error, _error, _error}};
    char c, c_tmp[2];
    
    strcpy(s, "");
    while(state <= state8){
                if((c = fgetc(f)) == '\n') line_cntr++;
                state = trans_diagram[state][InputToNum(c)];
                if(state == _asgn) colon_flag = 0;
                if(InputToNum(c) != blank && 
                   state != _id && state != _const && state != _ls && state != _gr && colon_flag != 1 &&
                   c != '\\' && state != state3 && state != state4 && state != state5){
                     sprintf(c_tmp, "%c", c);
                     strcat(s, c_tmp);
                }
                if(state == state8) colon_flag = 1;
                else if(state == state1 && strlen(s) == ID_SIZE - 1){
                     state = _id;
                     while(trans_diagram[state1][InputToNum(c = fgetc(f))] != _id);
                }
    }
    if(state == _id || state == _const || state == _ls || state == _gr || colon_flag == 1){
             fseek(f, ftell(f) - 1, SEEK_SET);
             if(c == '\n') line_cntr--;
             if(state == _id){
                      if(strcmp(s, "and") == 0) return _and;
                      else if(strcmp(s, "declare") == 0) return _declare;
                      else if(strcmp(s, "do") == 0) return _do;
                      else if(strcmp(s, "else") == 0) return _else;
                      else if(strcmp(s, "enddeclare") == 0) return _enddeclare;
                      else if(strcmp(s, "exit") == 0) return __exit;
                      else if(strcmp(s, "procedure") == 0) return _procedure;
                      else if(strcmp(s, "function") == 0) return _function;
                      else if(strcmp(s, "print") == 0) return _print;
                      else if(strcmp(s, "if") == 0) return _if;
                      else if(strcmp(s, "in") == 0) return _in;
                      else if(strcmp(s, "inout") == 0) return _inout;
                      else if(strcmp(s, "not") == 0) return _not;
                      else if(strcmp(s, "program") == 0) return _program;
                      else if(strcmp(s, "or") == 0) return _or;
                      else if(strcmp(s, "return") == 0) return _return;
                      else if(strcmp(s, "while") == 0) return _while;
             }
    }
    return state;

}
