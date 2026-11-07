#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

static void skip_whitespace(const char** s) {
    while (**s && isspace((unsigned char)**s)) {
        (*s)++;
    }
}

static double parse_expression(const char** s);

static double parse_primary(const char** s) {
    skip_whitespace(s);

    // Identifica chamadas de funções como ln(...)
    if (isalpha((unsigned char)**s)) {
        char func[16];
        int len = 0;
        while (isalpha((unsigned char)**s) && len < 15) {
            func[len++] = **s;
            (*s)++;
        }
        func[len] = '\0';

        skip_whitespace(s);
        if (**s == '(') {
            (*s)++; // Consome '('
            double arg = parse_expression(s);
            skip_whitespace(s);
            
            if (**s == ')') {
                (*s)++; // Consome ')'
            } else {
                fprintf(stderr, "Erro de sintaxe: esperava ')' após '%s'\n", func);
            }

            if (strcmp(func, "ln") == 0) {
                if (arg <= 0.0) {
                    fprintf(stderr, "Erro de domínio: ln(%.2f) não é permitido (deve ser > 0).\n", arg);
                    return 0.0;
                }
                return log(arg); // log() em C representa o logaritmo natural (base e)
            } else {
                fprintf(stderr, "Erro: Função desconhecida '%s'\n", func);
                return 0.0;
            }
        } else {
            fprintf(stderr, "Erro de sintaxe: esperava '(' após '%s'\n", func);
            return 0.0;
        }
    }

    // Processa subexpressões entre parênteses
    if (**s == '(') {
        (*s)++;
        double val = parse_expression(s);
        skip_whitespace(s);
        if (**s == ')') {
            (*s)++;
        } else {
            fprintf(stderr, "Erro de sintaxe: parêntese ')' ausente.\n");
        }
        return val;
    }

    // Converte números
    char* end_ptr;
    double val = strtod(*s, &end_ptr);
    if (*s == end_ptr) {
        fprintf(stderr, "Erro de sintaxe próximo a '%s'\n", *s);
        return 0.0;
    }
    *s = end_ptr;
    return val;
}

static double parse_factor(const char** s) {
    skip_whitespace(s);
    if (**s == '-') {
        (*s)++;
        return -parse_factor(s);
    }
    if (**s == '+') {
        (*s)++;
        return parse_factor(s);
    }
    return parse_primary(s);
}

static double parse_term(const char** s) {
    double left = parse_factor(s);
    while (1) {
        skip_whitespace(s);
        if (**s == '*') {
            (*s)++;
            left *= parse_factor(s);
        } else if (**s == '/') {
            (*s)++;
            double divisor = parse_factor(s);
            if (divisor == 0.0) {
                fprintf(stderr, "Erro: Divisão por zero.\n");
                return 0.0;
            }
            left /= divisor;
        } else {
            break;
        }
    }
    return left;
}

static double parse_expression(const char** s) {
    double left = parse_term(s);
    while (1) {
        skip_whitespace(s);
        if (**s == '+') {
            (*s)++;
            left += parse_term(s);
        } else if (**s == '-') {
            (*s)++;
            left -= parse_term(s);
        } else {
            break;
        }
    }
    return left;
}

double evaluate_expression(const char* expression) {
    if (!expression || *expression == '\0') {
        return 0.0;
    }
    const char* ptr = expression;
    return parse_expression(&ptr);
}

double evaluate_math_expression(const char* expression) {
    return evaluate_expression(expression);
}

int main(void) {
    const char* expr1 = "ln(2.718281828459)";
    const char* expr2 = "10 + ln(100 / 4) * 2";
    const char* expr3 = "ln(0)"; // Teste de erro

    printf("Resultado de '%s' = %f\n", expr1, evaluate_math_expression(expr1));
    printf("Resultado de '%s' = %f\n", expr2, evaluate_math_expression(expr2));
    printf("Resultado de '%s' = %f\n", expr3, evaluate_math_expression(expr3));

    return 0;
}