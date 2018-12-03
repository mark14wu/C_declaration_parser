#include <stdio.h> 
#include <string.h> 
#include <ctype.h>
#include <stdlib.h>
#define MAXTOKEN 100
enum { NAME, PARENS, BRACKETS };

void dcl(void);
void dirdcl(void);
void parameter_type_list(void);
void parameter_list(void);
void parameter_declaration(void);


int gettoken(void);
int tokentype;
char token[MAXTOKEN];
char name[MAXTOKEN];
char datatype[MAXTOKEN]; /* data type = char, int, etc. */ 
char out[1000];

int main() /* convert declaration to words */ 
{
    while(gettoken()!=EOF){ /*1sttokenonline*/
        strcpy(datatype, token); /* is the datatype */ 
        out[0] = '\0';
        dcl(); /* parse rest of line */
        if (tokentype == '\n' || tokentype == ';'){
            printf("%s: %s %s\n", name, out, datatype);
            gettoken();
        }
        /*
        if (tokentype != '\n' && tokentype != ';') {
            printf("syntax error\n");
            exit(-1);
        }*/
    }
    printf("%s: %s %s\n", name, out, datatype); 
    return 0; 
}

/* dcl: parse a declarator */ 
void dcl()
{
    int ns;
    for (ns = 0; gettoken() == '*'; ) /* count *'s */
        ns++;
    dirdcl();
    while (ns-- > 0)
        strcat(out, " pointer to");
}

/* dirdcl: parse a direct declarator */ 
void dirdcl()
{
    int type;
    if (tokentype == '('){
        dcl();
        if (tokentype != ')') {
            printf("error: missing )\n");
            char ch;
            while((ch=getchar())!=EOF){
                putchar(ch);
            }
            exit(-1);
        }
    } 
    else if (tokentype == NAME) /* variable name */ 
        strcpy(name, token);
    else{
        printf("error: expected name or (dcl)\n");
        exit(-1);
    }
    while ((type=gettoken()) == PARENS || type == BRACKETS || type == '(') {
        if (type == PARENS)
            strcat(out, " function returning");
        else if (type == '('){
            strcat(out, " function(");
            printf("ok!\n");
            gettoken();
            printf("token: %s\n", token);
            parameter_type_list();
            printf("token: %s\n", token);
            if (tokentype != ')') {
                printf("error: missing )\n");
                exit(-1);
            }
            strcat(out, ") returning");
        }
        else {
            strcat(out, " array"); 
            strcat(out, token); 
            strcat(out, " of");
        }
    }
}

void parameter_type_list()
{
    parameter_list();
}

void parameter_list()
{
    parameter_declaration();
    while (tokentype == ',')
    {
        strcat(out, ", ");
        gettoken();
        parameter_declaration();
    }
}

void parameter_declaration()
{
    char temp_name[MAXTOKEN];
    char temp_datatype[MAXTOKEN];
    char temp_out[1000];
    char temp[1000];
    strcpy(temp_datatype, datatype);
    strcpy(temp_name, name);
    strcpy(temp_out, out);
    out[0] = '\0';

    strcpy(datatype, token);
    dcl();
    sprintf(temp, "%s: %s %s", name, out, datatype); 
    printf("temp out: %s", temp);
    strcat(temp_out, temp);

    strcpy(datatype, temp_datatype);
    strcpy(name, temp_name);
    strcpy(out, temp_out);
}

int gettoken() /* return next token */ {
    int c;
    char *p = token;
    while ((c = getchar()) == ' ' || c == '\t') ;
    if (c == '(') {
        if ((c = getchar()) == ')') {
            strcpy(token, "()");
            return tokentype = PARENS; 
        } 
        else {
            ungetc(c, stdin);
            return tokentype = '('; 
        }
    } 
    else if (c == '[') {
        for (*p++ = c; (*p++ = getchar()) != ']'; );
        *p = '\0';
        return tokentype = BRACKETS; 
    } 
    else if (isalpha(c)) {
        for (*p++ = c; isalnum(c = getchar()); ) *p++ = c;
        *p = '\0';
        ungetc(c, stdin);
        return tokentype = NAME;
    } else
    return tokentype = c;
}