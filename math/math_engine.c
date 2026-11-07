#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define PI 3.14159265358979323846
#define EULER_E 2.71828182845904523536

static double custom_fabs(double x) {
    return x < 0 ? -x : x;
}

static double custom_fmod(double x, double y) {
    long long quotient = (long long)(x / y);
    return x - quotient * y;
}

// Logaritmo natural (ln) usando Série de Taylor com redução de escala
static double custom_ln(double x) {
    if (x <= 0.0) return 0.0;
    double result = 0.0;
    while (x >= EULER_E) { x /= EULER_E; result += 1.0; }
    while (x < (1.0 / EULER_E)) { x *= EULER_E; result -= 1.0; }
    double y = (x - 1.0) / (x + 1.0);
    double y2 = y * y;
    double term = y;
    double sum = 0.0;
    double divisor = 1.0;
    while (custom_fabs(term) > 1e-15) {
        sum += term / divisor;
        term *= y2;
        divisor += 2.0;
    }
    return result + 2.0 * sum;
}

// Exponencial (e^x) usando Série de Taylor com redução de escala
static double custom_exp(double x) {
    int n = 0;
    while (custom_fabs(x) > 1.0) { x /= 2.0; n++; }
    
    double sum = 1.0;
    double term = 1.0;
    for (int i = 1; i < 50; i++) {
        term = term * x / i;
        sum += term;
        if (custom_fabs(term) < 1e-15) break;
    }
    while (n > 0) { sum *= sum; n--; }
    return sum;
}

static double custom_pow(double base, double exp) {
    if (exp == 0.0) return 1.0;
    if (base == 0.0) return exp > 0 ? 0.0 : 0.0;
    if (base > 0.0) return custom_exp(exp * custom_ln(base));
    
    long e = (long)exp;
    if ((double)e == exp) {
        double res = 1.0;
        double b = base;
        if (e < 0) { e = -e; b = 1.0 / b; }
        while (e > 0) {
            if (e % 2 == 1) res *= b;
            b *= b;
            e /= 2;
        }
        return res;
    }
    fprintf(stderr, "Erro de dominio: base negativa com expoente fracionario.\n");
    return 0.0;
}

// Raiz quadrada usando o Método de Newton
static double custom_sqrt(double x) {
    if (x < 0.0) { fprintf(stderr, "Erro: raiz de numero negativo.\n"); return 0.0; }
    if (x == 0.0) return 0.0;
    double z = x;
    for (int i = 0; i < 20; i++) {
        double next_z = 0.5 * (z + x / z);
        if (custom_fabs(z - next_z) < 1e-15) break;
        z = next_z;
    }
    return z;
}

// Seno usando Série de Taylor
static double custom_sin(double x) {
    x = custom_fmod(x, 2 * PI);
    if (x > PI) x -= 2 * PI;
    else if (x < -PI) x += 2 * PI;

    double sum = x, term = x, x2 = x * x;
    for (int i = 1; i < 20; i++) {
        term = -term * x2 / ((2 * i) * (2 * i + 1));
        sum += term;
        if (custom_fabs(term) < 1e-15) break;
    }
    return sum;
}

// Cosseno usando Série de Taylor
static double custom_cos(double x) {
    x = custom_fmod(x, 2 * PI);
    if (x > PI) x -= 2 * PI;
    else if (x < -PI) x += 2 * PI;

    double sum = 1.0, term = 1.0, x2 = x * x;
    for (int i = 1; i < 20; i++) {
        term = -term * x2 / ((2 * i - 1) * (2 * i));
        sum += term;
        if (custom_fabs(term) < 1e-15) break;
    }
    return sum;
}

static void skip_whitespace(const char** s) {
    while (**s && isspace((unsigned char)**s)) (*s)++;
}

static double parse_expression(const char** s);

static double parse_primary(const char** s) {
    skip_whitespace(s);

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
            (*s)++;
            double arg = parse_expression(s);
            skip_whitespace(s);
            if (**s == ')') (*s)++;
            else fprintf(stderr, "Erro de sintaxe: esperava ')' após '%s'\n", func);

            if (strcmp(func, "ln") == 0) return custom_ln(arg);
            if (strcmp(func, "log") == 0) return custom_ln(arg) / custom_ln(10.0);
            if (strcmp(func, "exp") == 0) return custom_exp(arg);
            if (strcmp(func, "sqrt") == 0) return custom_sqrt(arg);
            if (strcmp(func, "sin") == 0) return custom_sin(arg);
            if (strcmp(func, "cos") == 0) return custom_cos(arg);
            if (strcmp(func, "tan") == 0) return custom_sin(arg) / custom_cos(arg);
            if (strcmp(func, "sinh") == 0) return (custom_exp(arg) - custom_exp(-arg)) / 2.0;
            if (strcmp(func, "cosh") == 0) return (custom_exp(arg) + custom_exp(-arg)) / 2.0;
            if (strcmp(func, "tanh") == 0) {
                double e_x = custom_exp(arg);
                double e_nx = custom_exp(-arg);
                return (e_x - e_nx) / (e_x + e_nx);
            }
            fprintf(stderr, "Erro: Funcao desconhecida '%s'\n", func);
            return 0.0;
        }
        fprintf(stderr, "Erro de sintaxe: esperava '(' após '%s'\n", func);
        return 0.0;
    }

    if (**s == '(') {
        (*s)++;
        double val = parse_expression(s);
        skip_whitespace(s);
        if (**s == ')') (*s)++;
        else fprintf(stderr, "Erro de sintaxe: parêntese ')' ausente.\n");
        return val;
    }

    char* end_ptr;
    double val = strtod(*s, &end_ptr);
    if (*s == end_ptr) {
        fprintf(stderr, "Erro de sintaxe proximo a '%s'\n", *s);
        return 0.0;
    }
    *s = end_ptr;
    return val;
}

static double parse_power(const char** s) {
    double left = parse_primary(s);
    skip_whitespace(s);
    if (**s == '^') {
        (*s)++;
        double right = parse_power(s);
        left = custom_pow(left, right);
    }
    return left;
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
    return parse_power(s);
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

double evaluate_math_expression(const char* expression) {
    if (!expression || *expression == '\0') return 0.0;
    const char* ptr = expression;
    return parse_expression(&ptr);
}

int main(void) {
    const char* expressions[] = {
        "2 + 3 * 4",
        "10 / 2 - 3",
        "2 ^ 3 ^ 2",
        "sqrt(144)",
        "27 ^ (1/3)",
        "log(100)",
        "ln(2.718281828459)",
        "exp(2)",
        "sin(3.14159265 / 2)",
        "cos(3.14159265)",
        "tan(3.14159265 / 4)",
        "sinh(1) + cosh(1)",
        "tanh(0)"
    };

    int n = sizeof(expressions) / sizeof(expressions[0]);
    for (int i = 0; i < n; i++) {
        printf("Expr: %-22s = %f\n", expressions[i], evaluate_math_expression(expressions[i]));
    }

    return 0;
}