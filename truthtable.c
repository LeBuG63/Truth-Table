#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

#define TOKEN_MAX_LENGHT        (0xF)
#define EXPRESSION_MAX_LENGHT   (0xFF)
#define STACK_SIZE              (0xFF)
#define N_KEYWORDS              (4)

#define SET_NTH_BIT(b, i, j)    (b = (i & (1 << j)) >> j)

typedef enum {
    UNDEFINED = -1,
    VAR,
    KEYWORD_ONE_OPE,
    KEYWORD_TWO_OPE
} operator_t;

typedef struct {
    char        name[16];
    char        shortcut[8];
    operator_t  op;
} keyword_t;

static keyword_t _keyword[N_KEYWORDS] = {
   { "and", "&&", KEYWORD_TWO_OPE},
   { "or", "||", KEYWORD_TWO_OPE},
   { "xor", "^", KEYWORD_TWO_OPE},
   { "not", "!", KEYWORD_ONE_OPE}
};

typedef struct {
    bool        bit;
    char        name[32];
} bitvar_t;

typedef struct {
    char        name[TOKEN_MAX_LENGHT];
    operator_t  op;
} token_t;

typedef struct {
    token_t token[STACK_SIZE];
    bool    arrbit[STACK_SIZE];

    size_t  size_arrbit;
    size_t  size_token;
    size_t  size_op;
} cstack_t;

typedef struct {
    bitvar_t    var[STACK_SIZE];

    size_t      size;
} var_t;

bool cpop(cstack_t *s) {
    return(s->arrbit[--s->size_arrbit]);
}

void cpush(cstack_t *s, bool bit) {  
    assert(s->size_arrbit < STACK_SIZE);

    s->arrbit[s->size_arrbit++] = bit;
}

void cadd_token(cstack_t *s, const char *name, operator_t op) {
    strcpy(s->token[s->size_token].name, name);
    s->token[s->size_token++].op = op;
}

void var_add(var_t *s, uint32_t bit, const char *name) {
    assert(s->size < STACK_SIZE);

    strcpy(s->var[s->size].name, name);

    s->var[s->size++].bit = bit;
}

int correct_varname(const char *str) {
    while(*str) {
        if(!isalpha(*str++))
            return 0;
    }
    return 1;
}

int already_exist(var_t *varr, const char *var) {
    for(uint32_t i = 0; i < varr->size; ++i) {
        if(strcmp(varr->var[i].name, var) == 0)
            return 1;
    }

    return 0;
}

int parse_string(cstack_t *cs, var_t *varr, char *str) {
    operator_t   op = VAR;

    char        *pch;
    char        *del = " ";

    size_t      total_one_op_lenght = 0u;

    pch = strtok(str, del);

    while(pch != NULL) {
        for(int i = 0; i < N_KEYWORDS ; ++i) {
            if(strcmp(pch, _keyword[i].name) == 0 
            || strcmp(pch, _keyword[i].shortcut) == 0) {
                cs->size_op++;
                op = _keyword[i].op;
                break;
            }
        }

        if(op == VAR) {
            if(correct_varname(pch)) {
                if(!already_exist(varr, pch)) {
                    var_add(varr, 0, pch);
                }
            }
            else {
                return 0;
            }
        }
        else if (op == KEYWORD_ONE_OPE) {
            total_one_op_lenght++;
        }

        cadd_token(cs, pch, op);

        op  = VAR;
        pch = strtok(NULL, del);
    }
    
    if((cs->size_token - (varr->size - 2) - total_one_op_lenght) >= (varr->size - 2)) {
        return 0;
    }

    return 1;
}

bitvar_t *bitvar_get_from_name(var_t *varr, const char *name) {
    for(uint32_t i = 0u; i < varr->size; ++i) {
        if(strcmp(varr->var[i].name, name) == 0)
            return &varr->var[i];
    }
    return NULL;
}

bool evaluate(const char *name, bool a, bool b) {
    if(strcmp(name, "and") == 0 || strcmp(name, "&&") == 0)    return a && b;
    if(strcmp(name, "or") == 0 || strcmp(name, "||") == 0)     return a || b;
    if(strcmp(name, "xor") == 0 || strcmp(name, "^") == 0)    return a ^ b;
    if(strcmp(name, "not") == 0|| strcmp(name, "!") == 0)    return !a;

    return 0;
}

bool result_rpn(var_t *varr, cstack_t *cs) {
    int ret;
    
    if(cs->size_op > 0) {
        for(int i = 0; cs->token[i].op != UNDEFINED; ++i) {
            token_t acttok = cs->token[i];

            if(acttok.op == KEYWORD_TWO_OPE) {
                bool a = cpop(cs);
                bool b = cpop(cs);

                cpush(cs, evaluate(acttok.name, a, b));
            }
            else  if(acttok.op == KEYWORD_ONE_OPE) {
                bool a = cpop(cs);

                cpush(cs, evaluate(acttok.name, a, 0));
            }
            else {
                bitvar_t *b = bitvar_get_from_name(varr, acttok.name);

                cpush(cs, b->bit);
            }
        }
        ret = cpop(cs);
        
        cs->size_arrbit = 0;
    }
    else {
        ret = 0;
    }


    return ret;
}

void generate_truthtable(var_t *varr, cstack_t *cs) {
    uint32_t    nelem = varr->size;
    int         res;
    int         maxlenght;

    printf("     ");

    for(int j = varr->size - 1; j >= 0 ; --j) {
        int act_size = strlen(varr->var[j].name);
        if(act_size > maxlenght)
            maxlenght = act_size;
    }
    
    maxlenght += 2;

    for(int j = varr->size - 1; j >= 2 ; --j) {
        printf("%*s", maxlenght, varr->var[j].name);
    }

    printf("%*s", 5 + maxlenght, "Result");

    for(int i = 0; i < (1 << (nelem - 1)); ++i) {
        printf("\n%4d:", i + 1);

        for(int j = nelem - 1; j > 1 ; --j) {
            SET_NTH_BIT(varr->var[j].bit, i, (j - 1));

            printf("%*d", maxlenght, varr->var[j].bit);
        }
        res = result_rpn(varr, cs);
        printf("%*d", maxlenght, res);
    }
}

void compute_truthtable(char *expr) {    
    var_t    varr = { .size = 0u };
    cstack_t cs = { .size_arrbit = 0u, 
                    .size_token = 0u,
                    .size_op = 0u };

    int      ret = 0;

    for(int i = 0; i < STACK_SIZE; ++i)
        cs.token[i].op = UNDEFINED;
    
    // Constants
    var_add(&varr, 1, "true");
    var_add(&varr, 0, "false");

    ret = parse_string(&cs, &varr, expr);
    
    if(ret) {
        generate_truthtable(&varr, &cs);
    
        putchar('\n');
    }
    else {
        fprintf(stderr, "Erreur: trop d'operandes\n");
    }
}

int main(int argc, char *argv[]) {
    if(argc == 2) {
        compute_truthtable(argv[1]);
    } 
    else {
        printf("Utilisation: ttable <expr>\n "
                "L'expression fonctionne avec la notation polonaise inverse du type: \"variable1 variable2 operateur_2_entrees\" ou \"variable1 operateur_1_entree\"\n"
                "Operateurs:    \n"
                "\t- and (&&)   \n"
                "\t- or  (||)   \n"
                "\t- xor (^)    \n"
                "\t- not (!)    \n"
                "Constantes:    \n"
                "\t- true (1)   \n"
                "\t- false (0)  \n"
                "Variables:     \n"
                "\t- une chaine de caracteres\n"
                "\t- un caractere\n");

        return 1;
    }

    return 0;
}