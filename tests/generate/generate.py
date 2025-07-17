#!/usr/bin/env python3
'''
This script reads in a grammar file and emits templates for a parser and and
AST. It does not attempt to actually generate the code. If this is run in the
actual source tree, it will destroy all edits.
'''

import re
import time
import os
import sys

tokens = []
non_terminals = []
terminals = []
keywords = []
translate = []
srules = {}
sstates = []
snames = []
grules = {}
rule_name = ""
grammar = []

def convert(str):

    s = "TOK"
    for c in str:
        if c == '!': s += "_BANG"
        elif c == '@': s += "_AT"
        elif c == '#': s += "_HASH"
        elif c == '$': s += "_DOLLAR"
        elif c == '%': s += "_PERCENT"
        elif c == '^': s += "_CARAT"
        elif c == '&': s += "_AMPERSAND"
        elif c == '*': s += "_STAR"
        elif c == '(': s += "_OPAREN"
        elif c == ')': s += "_CPAREN"
        elif c == '-': s += "_MINUS"
        elif c == '=': s += "_EQUAL"
        elif c == '+': s += "_PLUS"
        elif c == '[': s += "_OSBRACE"
        elif c == ']': s += "_CSBRACE"
        elif c == '{': s += "_OCBRACE"
        elif c == '}': s += "_CCBRACE"
        elif c == '\\': s += "_BSLASH"
        elif c == ':': s += "_COLON"
        elif c == ';': s += "_SEMICOLON"
        elif c == '\'': s += "_SQUOTE"
        elif c == '\"': s += "_DQUOTE"
        elif c == ',': s += "_COMMA"
        elif c == '<': s += "_OPBRACE"
        elif c == '.': s += "_DOT"
        elif c == '>': s += "_CPBRACE"
        elif c == '/': s += "_SLASH"
        elif c == '?': s += "_QUESTION"
        elif c == '~': s += "_TILDE"
    return s

def get_sstate(fp, marker):
    res = re.findall(r"<([a-zA-Z_]+)>", marker)
    if not res[0] in snames:
        snames.append(res[0])
    sstates.append([])
    sstates[-1].append(marker)
    for line in fp:
        line = line.strip()
        if len(line) == 0:
            break;
        else:
            sstates[-1].append(line)

def get_srule(fp, name):
    tok = "TOK_"+name.upper()
    if not tok in tokens:
        tokens.append(tok)
        translate.append("(type == %s)? \"%s\" :"%(tok, name.upper()))

    srules[name] = {}
    srules[name]['rules'] = []
    srules[name]['action'] = []
    for line in fp:
        line = line.strip()
        if len(line) > 0:
            if line[0] == ':' or line[0] == '|':
                srules[name]['rules'].append(line[1:].strip())
            elif line == '{':
                count = 1
                srules[name]['action'].append(line)
                for x in fp:
                    count -= x.count('}')
                    srules[name]['action'].append(" "*count+line)
                    count += x.count('{')
                    if count <= 0:
                        break
            elif line[0] == ';':
                break
        else:
            break

def get_grule(fp, name):
    non_terminals.append(name)
    grules[name] = []
    for line in fp:
        line = line.strip()
        if len(line) <= 0: continue
        if line[0] == ':' or line[0] == '|':
            grules[name].append("    %s"%(line))
            res = re.findall(r"\'([^\']+)\'|\"([^\"]+)\"", line)
            for mat in res:
                if len(mat[0]) != 0:
                    tmp = mat[0]
                elif len(mat[1]) != 0:
                    tmp = mat[1]

                if not tmp in terminals:
                    terminals.append(tmp)

                if re.match(r"[a-z_]+", tmp):
                    if not tmp in keywords:
                        keywords.append(tmp)
                    s = "TOK_%s"%(tmp.upper())
                else:
                    s = convert(tmp)

                if not s in tokens:
                    tokens.append(s)
                    translate.append("(type == %s)? \"%s\" :"%(s, tmp.replace("_", " ")))

            res = re.findall(r"([A-Z_]+)", line)
            for mat in res:
                if mat != "_":
                    s = "TOK_"+mat
                    if not s in tokens:
                        tokens.append(s)
                        translate.append("(type == %s)? \"%s\" :"%(s, mat.replace("_", " ")))

        elif line[0] == ';':
            grules[name].append("    ;")
            break

def gen_lists(fname):

    with open(fname, "r") as fp:
        for line in fp :
            line = line.strip()
            if len(line) > 0 and line[0] != '#' :
                grammar.append(line)
                if re.match(r"[A-Z_]+", line):
                    get_srule(fp, line)
                elif re.match(r"[a-z_]+", line):
                    get_grule(fp, line)
                elif re.match(r"<[a-zA-Z_]+>", line):
                    get_sstate(fp, line)
                else:
                    print("unknown line ignored: ", line.strip())

    #terminals.sort()
    #non_terminals.sort()
    #tokens.sort()

def gen_ast():

    with open("ast/ast.h", "w") as fp:
        fp.write("/**\n *\n")
        fp.write(" * @file ast.h\n *\n")
        fp.write(" * @brief AST traverse public interface.\n")
        fp.write(" * This file was generated on %s.\n"%(time.asctime()))
        fp.write(" *\n */\n")
        fp.write("\n#ifndef _AST_H_\n#define _AST_H_\n\n")

        fp.write("typedef enum {\n")
        for str in grules:
            fp.write("    AST_%s,\n"%(str.upper()))
        fp.write("} ast_node_type_t;\n\n")

        fp.write("typedef struct _ast_node_ {\n")
        fp.write("    ast_node_type_t type;\n")
        fp.write("} ast_node_t;\n\n")

        for str in grules:
            fp.write("/**\n")
            fp.write(" * %s\n"%(str))
            for s in grules[str]:
                fp.write(" * %s\n"%(s))
            fp.write(" */\n")
            fp.write("typedef struct _ast_%s_t_ {\n"%(str))
            fp.write("    ast_node_t node;\n\n")
            fp.write("} ast_%s_t;\n\n"%(str))

        for str in grules:
            fp.write("void traverse_%s(ast_%s_t* node);\n"%(str, str))

        fp.write("\nast_node_t* create_ast_node(ast_node_type_t type);\n")
        fp.write("void traverse_ast(ast_%s_t* node);\n\n"%(non_terminals[0]))
        fp.write("\n#endif /* _AST_H_ */\n\n")

    with open("ast/ast.c", "w") as fp:
            fp.write("/**\n *\n")
            fp.write(" * @file %s.c\n *\n"%(str))
            fp.write(" * @brief AST implementation.\n")
            fp.write(" * This file was generated on %s.\n"%(time.asctime()))
            fp.write(" *\n */\n")
            fp.write("#include <stddef.h>\n\n")

            fp.write("#include \"ast.h\"\n\n")
            fp.write("static size_t node_size(ast_node_type_t type) {\n\n")
            fp.write("    return\n")
            for str in grules:
                fp.write("    (type == AST_%s)? sizeof(ast_%s_t) : \n"%(str.upper(), str))
            fp.write("    (size_t)-1; // error if we reach here\n")
            fp.write("}\n\n")
            fp.write("ast_node_t* create_ast_node(ast_node_type_t type) {\n\n")
            fp.write("    ast_node_t* node = _ALLOC(node_size(type));\n")
            fp.write("    node->type = type;\n")
            fp.write("    return node;\n")
            fp.write("}\n\n")
            fp.write("void traverse_ast(ast_%s_t* node) {\n\n"%(non_terminals[0]))
            fp.write("    traverse_%s(node);\n"%(non_terminals[0]))
            fp.write("}\n\n")

    for str in grules:
        with open("ast/%s.c"%(str), "w") as fp:
            fp.write("/**\n *\n")
            fp.write(" * @file %s.c\n *\n"%(str))
            fp.write(" * @brief Traverse AST for node %s.\n"%(str))
            fp.write(" * This file was generated on %s.\n"%(time.asctime()))
            fp.write(" *\n */\n")
            fp.write("#include \"ast.h\"\n")
            fp.write("#include \"trace.h\"\n\n")
            fp.write("/**\n")
            fp.write(" * %s\n"%(str))
            for s in grules[str]:
                fp.write(" * %s\n"%(s))
            fp.write(" */\n")
            fp.write("void traverse_%s(ast_%s_t* node) {\n\n"%(str, str))
            fp.write("    ENTER;\n")
            fp.write("    RETURN();\n")
            fp.write("}\n\n")

def gen_parse():

    with open("parser/parser.h", "w") as fp:
        fp.write("/**\n *\n")
        fp.write(" * @file parser.h\n *\n")
        fp.write(" * @brief Parse grammar public interface.\n")
        fp.write(" * This file was generated on %s.\n"%(time.asctime()))
        fp.write(" *\n */\n")
        fp.write("#ifndef _PARSER_H_\n#define _PARSER_H_\n\n")

        fp.write("#include \"ast.h\"\n")
        fp.write("#include \"parser_prototypes.h\"\n\n")

        fp.write("typedef enum {\n")
        fp.write("    STATE_START = 0,\n")
        fp.write("    STATE_MATCH = 1000,\n")
        fp.write("    STATE_NO_MATCH = 1010,\n")
        fp.write("    STATE_ERROR = 1020,\n")
        fp.write("} parser_state_t;\n")

        fp.write("typedef struct {\n")
        fp.write("    int mode;\n")
        fp.write("} parser_state_t;\n\n")
        #fp.write("void recover_error(void);\n")

        fp.write("#define STATE \\ \n")
        fp.write("    do { \\ \n")
        fp.write("        TRACE(\"STATE: %d\", state); \\ \n")
        fp.write("    } while(0)\n\n")

        fp.write("ast_%s_t* parse(void);\n\n"%(non_terminals[0]))

        fp.write("#endif /* _PARSER_H_ */\n\n")

    with open("parser/parser_prototypes.h", "w") as fp:
        fp.write("/**\n *\n")
        fp.write(" * @file parser_prototypes.h\n *\n")
        fp.write(" * @brief Internal prototypes for parser.\n")
        fp.write(" * This file was generated on %s.\n"%(time.asctime()))
        fp.write(" *\n */\n")
        fp.write("#ifndef _PARSER_PROTOTYPES_H_\n#define _PARSER_PROTOTYPES_H_\n\n")
        fp.write("#include \"parser.h\"\n")
        fp.write("#include \"ast.h\"\n\n")
        for str in grules:
            fp.write("ast_%s_t* parse_%s(void);\n"%(str, str))
        fp.write("\n#endif /* _PARSER_PROTOTYPES_H_ */\n\n")

    with open("parser/parser.c", "w") as fp:
        fp.write("/**\n *\n")
        fp.write(" * @file parser.c\n *\n")
        fp.write(" * @brief Parser external interface implementation.\n")
        fp.write(" * This file was generated on %s.\n"%(time.asctime()))
        fp.write(" *\n */\n")
        fp.write("#include \"ast.h\"\n")
        fp.write("#include \"parser_prototypes.h\"\n\n")
        #fp.write("#include \"trace.h\"\n\n")
        #fp.write("void recover_error(void) {\n\n")
        #fp.write("}\n\n")
        fp.write("ast_%s_t* parse(void) {\n\n"%(non_terminals[0]))
        #fp.write("    parser_state_t* pstate = _ALLOC_DS(parser_state_t);\n")
        #fp.write("    pstate->mode = 0;\n")
        fp.write("    ast_%s_t* %s = parse_%s();\n"%(non_terminals[0], non_terminals[0], non_terminals[0]))
        fp.write("    return %s;\n"%(non_terminals[0]))
        fp.write("}\n\n")

    for str in grules:
        with open("parser/%s.c"%(str), "w") as fp:
            fp.write("/**\n *\n")
            fp.write(" * @file %s.c\n *\n"%(str))
            fp.write(" * @brief Parse grammar production %s.\n"%(str))
            fp.write(" * This file was generated on %s.\n"%(time.asctime()))
            fp.write(" *\n */\n")
            #fp.write("#include \"common.h\"\n")
            fp.write("#include \"tokens.h\"\n")
            fp.write("#include \"trace.h\"\n")
            fp.write("#include \"parser.h\"\n\n")
            fp.write("/**\n")
            fp.write(" * %s\n"%(str))
            for s in grules[str]:
                fp.write(" * %s\n"%(s))
            fp.write(" */\n")
            fp.write("ast_%s_t* parse_%s(void) {\n\n"%(str, str))
            #fp.write("    ASSERT(pstate != NULL);\n")
            fp.write("    ENTER;\n\n")
            fp.write("    ast_%s_t* node = NULL;\n"%(str))
            fp.write("    int state = STATE_START;\n")
            fp.write("    bool finished = false;\n")
            fp.write("    void* post = post_token_queue();\n\n")
            fp.write("    while(!finished) {\n")
            fp.write("        switch(state) {\n")
            fp.write("            case STATE_START:\n")
            fp.write("                // initial state\n")
            fp.write("                STATE;\n")
            fp.write("                break;\n\n")
            fp.write("            case STATE_MATCH:\n")
            fp.write("                // production recognized\n")
            fp.write("                STATE;\n")
            fp.write("                node = (ast_%s_t*)create_ast_node(AST_%s);\n"%(str, str.upper()))
            fp.write("                finished = true;\n")
            fp.write("                break;\n\n")
            fp.write("            case STATE_NO_MATCH:\n")
            fp.write("                // not a match, not an error\n")
            fp.write("                STATE;\n")
            fp.write("                reset_token_queue(post);\n")
            fp.write("                finished = true;\n")
            fp.write("                break;\n\n")
            fp.write("            case STATE_ERROR:\n")
            fp.write("                // error found\n")
            fp.write("                STATE;\n")
            fp.write("                recover_error();\n")
            fp.write("                finished = true;\n")
            fp.write("                break;\n\n")
            fp.write("            default:\n")
            fp.write("                FATAL(\"unknown state: %d\", state);\n")
            fp.write("        }\n")
            fp.write("    }\n\n")
            fp.write("    RETURN(node);\n")
            fp.write("}\n\n")

def gen_scanner():

    with open("scanner/tokens.h", "w") as fp:
        fp.write("/**\n *\n")
        fp.write(" * @file token_defs.h\n *\n")
        fp.write(" * @brief Token definitions public interface.\n")
        fp.write(" * This file was generated on %s.\n"%(time.asctime()))
        fp.write(" *\n */\n")
        fp.write("#ifndef _TOKEN_DEFS_H_\n")
        fp.write("#define _TOKEN_DEFS_H_\n\n")

        fp.write("#include <stdint.h>\n")
        fp.write("#include \"string_buf.h\"\n\n")

        token_num = 256
        fp.write("typedef enum {\n")
        for item in tokens:
            fp.write("    %s = %d,\n"%(item, token_num))
            token_num += 1

        fp.write("\n    TOK_END_OF_INPUT = %d,\n"%(token_num))
        token_num += 1
        fp.write("    TOK_END_OF_FILE = %d,\n"%(token_num))
        token_num += 1
        fp.write("    TOK_ERROR = %d,\n"%(token_num))
        token_num += 1
        fp.write("} token_type_t;\n\n")

        fp.write(
'''
typedef struct _token_t_ {
    token_type_t type;
    string_buf_t* raw;
    string_buf_t* fname;
    int line_no;
    int col_no;
} token_t;

void init_token_queue(const char* fname);
void* post_token_queue(void);
void reset_token_queue(void* post);
void consume_token_queue(void);
void add_token_queue(token_t* tok);

token_t* create_token(const char* str, token_type_t type);
void destroy_token(token_t* tok);
token_t* get_token(void);
token_t* advance_token(void);
const char* token_type_to_str(token_type_t type);

#endif /* _TOKEN_DEFS_H_ */

''')

    with open("scanner/tokens.c", "w") as fp:
        fp.write("/**\n *\n")
        fp.write(" * @file tokens.c\n *\n")
        fp.write(" * @brief Token queue implementation.\n")
        fp.write(" * This file was generated on %s.\n"%(time.asctime()))
        fp.write(" *\n */\n")
        fp.write(
'''

#include <stdio.h>
#include <strings.h>

#include "tokens.h"
#include "scanner.h"
#include "queue.h"
#include "fileio.h"
#include "alloc.h"

queue_t* token_queue = NULL;

void init_token_queue(const char* fname) {

    if(token_queue == NULL)
        token_queue = _ALLOC_TYPE(queue_t);

    open_file(fname);
    yylex();
}

void* post_token_queue(void) {

    return (void*)token_queue->crnt;
}

void reset_token_queue(void* post) {

    token_queue->crnt = (queue_element_t*)post;
}

void consume_token_queue(void) {

    queue_element_t* next;
    for(queue_element_t* elem = token_queue->head; elem != token_queue->crnt; elem = next) {
        next = elem->next;
        destroy_token(elem->data);
        _FREE(elem);
    }
}

void add_token_queue(token_t* tok) {

    add_queue(token_queue, (void*)tok);
}

token_t* create_token(const char* str, token_type_t type) {

    token_t* tok = _ALLOC_TYPE(token_t);
    tok->type = type;
    tok->raw = create_string_buf(str);
    tok->line_no = get_line_no();
    tok->col_no = get_col_no();
    tok->fname = create_string_buf(get_file_name());

    return tok;
}

void destroy_token(token_t* tok) {

    if(tok != NULL) {
        if(tok->raw != NULL)
            destroy_string_buf(tok->raw);
        if(tok->fname != NULL)
            destroy_string_buf(tok->fname);

        _FREE(tok);
    }
}

token_t* get_token(void) {

    if(token_queue != NULL) {
        return peek_queue(token_queue);
    }
    else
        return NULL;
}

token_t* advance_token(void) {

    token_t* tok = advance_queue(token_queue);
    if(tok == NULL) {
        tok = get_token();
        if(tok->type != TOK_END_OF_INPUT) {
            if(yylex() == 0)
                add_token_queue(create_token("end of input", TOK_END_OF_INPUT));

            token_queue->crnt = token_queue->tail;
            tok = get_token();
        }
    }

    return tok;
}

''')
        fp.write("const char* token_type_to_str(token_type_t type) {\n\n")
        fp.write("    return ")
        fp.write("%s\n"%(translate[0]))
        for item in translate[1:]:
            fp.write("        %s\n"%(item))
        fp.write("\n        (type == TOK_END_OF_INPUT)? \"END OF INPUT\" :\n")
        fp.write("        (type == TOK_END_OF_FILE)? \"END OF FILE\" :\n")
        fp.write("        (type == TOK_ERROR)? \"ERROR\" : \"UNKNOWN\";\n")
        fp.write("}\n\n")

    with open("scanner/scanner.l", "w") as fp:
        fp.write("/**\n *\n")
        fp.write(" * @file scanner.l\n *\n")
        fp.write(" * @brief Scanner template.\n")
        fp.write(" * This file was generated on %s.\n"%(time.asctime()))
        fp.write(" *\n */\n")
        fp.write(
'''
%{
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "errors.h"
#include "tokens.h"
#include "alloc.h"

int yycolumn = 1;

typedef struct _file_info_t_ {
    const char* fname;
    FILE* fp;
    size_t line_no;
    size_t col_no;
    YY_BUFFER_STATE buffer;
    struct _file_info_t_* next;
} file_info_t;

static file_info_t* file_stack = NULL;
static file_info_t* end_file = NULL;

#define YY_USER_ACTION \\
  file_stack->col_no = yycolumn; \\
  file_stack->line_no = yylineno; \\
  if (yylineno == prev_yylineno) \\
    yycolumn += yyleng; \\
  else { \\
    for (yycolumn = 1; yytext[yyleng - yycolumn] != '\\n'; ++yycolumn) {} \\
    prev_yylineno = yylineno; \\
  }

%}

''')
        if len(snames) > 0:
            fp.write("%%x %s"%(" ".join(snames)))

        fp.write(
'''
%option yylineno
%option noinput
%option nounput
%option noyywrap
%option header-file="scanner.h"
%option outfile="scanner.c"

%%

   // int start_line, start_column;
   int prev_yylineno = yylineno;

''')
        # implicit scanner rules
        for item in terminals:
            if re.match(r'[a-z_A-Z]+', item):
                tok = "TOK_"+item.upper()
            else:
                tok = convert(item)
            fp.write("\"%s\" {\n    add_token_queue(create_token(yytext, %s));\n    return %s;\n}\n\n"%(item, tok, tok))

        for item in sstates:
            fp.write("%s\n"%(item[0]))
            count = 4
            for x in item[1:]:
                if "{" in x:
                    count += 4
                elif "}" in x:
                    count -= 4
                fp.write(" "*count+"%s\n"%(x))
            fp.write("\n")

        # explicit scanner rules
        for item in srules:
            l = len(srules[item]['rules'])
            for i, x in enumerate(srules[item]['rules']):
                fp.write("%s"%(x))
                if i+1 < l:
                    fp.write("|")

            if len(srules[item]['action']) > 0:
                fp.write(" ")
                count = 0
                for x in srules[item]['action']:
                    if "{" in x:
                        count += 4
                    elif "}" in x:
                        count -= 4
                    fp.write(" "*count+"%s\n"%(x))
                fp.write("\n")
            else:
                tok = "TOK_"+item
                fp.write(" {\n    add_token_queue(create_token(yytext, %s));\n    return %s;\n}\n\n"%(tok, tok))


            # s = srules[item]['attrs'].strip()
            # if len(s) == 0:
            #     tok = "TOK_"+item
            #     fp.write(" {\n    add_token_queue(create_token(yytext, %s));\n    return %s;\n}\n\n"%(tok, tok))
            # elif '{' in srules[item]['attrs']:
            #     fp.write(" %s"%(srules[item]['attrs'].strip()))
            # elif 'i' in srules[item]['attrs']:
            #     fp.write(" { /* ignore match */ }\n\n")

        fp.write(
'''

<<EOF>> {

    if(file_stack == NULL) {
        yyterminate();
        return 0;
    }

    file_info_t* fs = file_stack;
    yy_delete_buffer(fs->buffer);
    file_stack = file_stack->next;

    fclose(fs->fp);

    if(file_stack != NULL) {
        yy_switch_to_buffer(file_stack->buffer);
        _FREE(fs->fname);
        _FREE(fs);
    }
    else {
        end_file = fs;
        yyterminate();
    }
}

%%

void open_file(const char* fn) {

    const char *fname = fn; //find_file(fn);
    yyin = fopen(fname, "r");
    if(yyin == NULL)
        FATAL("cannot open input file: \\"%s\\": %s\\n", fname, strerror(errno));

    file_info_t* fs = _ALLOC_TYPE(file_info_t);
    fs->fname = fname;
    fs->next = NULL;
    fs->line_no = 1;
    fs->col_no = 1;

    fs->fp = yyin;
    fs->buffer = yy_create_buffer(yyin, YY_BUF_SIZE);
    yy_switch_to_buffer(fs->buffer);

    if(file_stack != NULL) {
        fs->next = file_stack;
        file_stack = fs;
    }
    else
        file_stack = fs;

}

int get_line_no(void) {

    if(file_stack != NULL)
        return file_stack->line_no;
    else if(end_file != NULL)
        return end_file->line_no;
    else
        return -1; // no file has ever been open
}

int get_col_no(void) {

    if(file_stack != NULL)
        return file_stack->col_no;
    else if(end_file != NULL)
        return end_file->col_no;
    else
        return -1;
}

const char* get_file_name(void) {

    if(file_stack != NULL)
        return file_stack->fname;
    else if(end_file != NULL)
        return end_file->fname;
    else
        return NULL;
}

''')

def generate() :

    if not os.path.isdir('ast'):
        os.mkdir('ast')
    if not os.path.isdir('parser'):
        os.mkdir('parser')
    if not os.path.isdir('scanner'):
        os.mkdir('scanner')

    gen_ast()
    gen_parse()
    gen_scanner()

import pprint
if __name__ == '__main__':

    # print("Before you run this, know that it will destroy the parser")
    # print("and all of the manual changes that were made to it.")
    # exit(1)

    if(len(sys.argv) < 2) :
        sys.stderr.write("%s: file name required\n"%(sys.argv[0]))
        exit(1)

    gen_lists(sys.argv[1])

    count = 0
    with open("non-terminals.txt", "w") as fp:
        for item in non_terminals:
            fp.write("%s\n"%(item))
            count += 1
    print("non-terminals:", count)

    count = 0
    with open("terminals.txt", "w") as fp:
        for item in terminals:
            fp.write("%s\n"%(item))
            count += 1

    print("terminals:", count)

    count = 0
    with open("keywords.txt", "w") as fp:
        for item in keywords:
            fp.write("%s\n"%(item))
            count += 1

    print("keywords:", count)

    count = 0
    with open("tokens.txt", "w") as fp:
        for item in tokens:
            fp.write("%s\n"%(item))
            count += 1

    print("tokens:", count)

    count = 0
    with open("srules.txt", "w") as fp:
        for item in srules:
            t = ""
            l = len(srules[item]['rules'])
            for i, x in enumerate(srules[item]['rules']):
                fp.write("%s"%(x))
                if i+1 < l:
                    fp.write("|")
            count += 1
            fp.write("\n")

    print("srules:", count)

    pprint.pprint(sstates)
    generate()
