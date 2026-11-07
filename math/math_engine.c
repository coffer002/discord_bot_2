#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define PI 3.14159265358979323846
#define EULER_E 2.71828182845904523536

#define ENABLE_LOGS 1

#if ENABLE_LOGS
    #define LOG(fmt, ...) do { \
        printf("[LOG] " fmt "\n", ##__VA_ARGS__); \
        fflush(stdout); \
    } while(0)
#else
    #define LOG(fmt, ...)
#endif


typedef struct {
    double r;
    double i;
} Complex;

static double custom_fabs(double x) {
    return x < 0 ? -x : x;
}

static double custom_fmod(double x, double y) {
    long long quotient = (long long)(x / y);
    return x - quotient * y;
}

static double custom_sqrt_real(double x) {
    if (x <= 0.0) return 0.0;
    double z = x;
    for (int i = 0; i < 20; i++) {
        double next_z = 0.5 * (z + x / z);
        if (custom_fabs(z - next_z) < 1e-15) break;
        z = next_z;
    }
    return z;
}

static double custom_atan(double x) {
    int sign = x < 0 ? -1 : 1;
    x = custom_fabs(x);
    int inv = 0;
    if (x > 1.0) { x = 1.0 / x; inv = 1; }
    
    int halves = 0;
    while (x > 0.1) {
        x = x / (1.0 + custom_sqrt_real(1.0 + x * x));
        halves++;
    }
    
    double sum = 0, term = x, x2 = x * x;
    for (int i = 1; i < 30; i += 2) {
        sum += term / i;
        term = -term * x2;
        if (custom_fabs(term) < 1e-15) break;
    }
    
    while (halves > 0) {
        sum *= 2.0;
        halves--;
    }
    
    return sign * (inv ? (PI / 2.0 - sum) : sum);
}

static double custom_atan2(double y, double x) {
    if (x > 0.0) return custom_atan(y / x);
    if (x < 0.0 && y >= 0.0) return custom_atan(y / x) + PI;
    if (x < 0.0 && y < 0.0) return custom_atan(y / x) - PI;
    if (x == 0.0 && y > 0.0) return PI / 2.0;
    if (x == 0.0 && y < 0.0) return -PI / 2.0;
    return 0.0;
}

static double custom_ln_real(double x) {
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

static double custom_exp_real(double x) {
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

static double custom_sin_real(double x) {
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

static double custom_cos_real(double x) {
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

static double custom_sinh_real(double x) {
    return (custom_exp_real(x) - custom_exp_real(-x)) / 2.0;
}

static double custom_cosh_real(double x) {
    return (custom_exp_real(x) + custom_exp_real(-x)) / 2.0;
}

static Complex c_add(Complex a, Complex b) {
    return (Complex){a.r + b.r, a.i + b.i};
}

static Complex c_sub(Complex a, Complex b) {
    return (Complex){a.r - b.r, a.i - b.i};
}

static Complex c_mul(Complex a, Complex b) {
    return (Complex){a.r * b.r - a.i * b.i, a.r * b.i + a.i * b.r};
}

static Complex c_div(Complex a, Complex b) {
    double den = b.r * b.r + b.i * b.i;
    if (den == 0.0) return (Complex){0.0, 0.0};
    return (Complex){(a.r * b.r + a.i * b.i) / den, (a.i * b.r - a.r * b.i) / den};
}

static Complex c_sqrt(Complex x) {
    if (custom_fabs(x.i) < 1e-12) {
        if (x.r >= 0) return (Complex){custom_sqrt_real(x.r), 0.0};
        return (Complex){0.0, custom_sqrt_real(-x.r)};
    }
    double mod = custom_sqrt_real(x.r * x.r + x.i * x.i);
    double r = custom_sqrt_real((mod + x.r) / 2.0);
    double i = custom_sqrt_real((mod - x.r) / 2.0);
    if (x.i < 0) i = -i;
    return (Complex){r, i};
}

static Complex c_ln(Complex x) {
    double mod = custom_sqrt_real(x.r * x.r + x.i * x.i);
    return (Complex){custom_ln_real(mod), custom_atan2(x.i, x.r)};
}

static Complex c_exp(Complex x) {
    double er = custom_exp_real(x.r);
    return (Complex){er * custom_cos_real(x.i), er * custom_sin_real(x.i)};
}

static Complex c_pow(Complex b, Complex e) {
    if (custom_fabs(b.r) < 1e-12 && custom_fabs(b.i) < 1e-12) return (Complex){0.0, 0.0};
    return c_exp(c_mul(e, c_ln(b)));
}

static Complex c_sin(Complex x) {
    return (Complex){custom_sin_real(x.r) * custom_cosh_real(x.i), custom_cos_real(x.r) * custom_sinh_real(x.i)};
}

static Complex c_cos(Complex x) {
    return (Complex){custom_cos_real(x.r) * custom_cosh_real(x.i), -custom_sin_real(x.r) * custom_sinh_real(x.i)};
}

static Complex c_tan(Complex x) {
    return c_div(c_sin(x), c_cos(x));
}

static Complex c_sinh(Complex x) {
    return (Complex){custom_sinh_real(x.r) * custom_cos_real(x.i), custom_cosh_real(x.r) * custom_sin_real(x.i)};
}

static Complex c_cosh(Complex x) {
    return (Complex){custom_cosh_real(x.r) * custom_cos_real(x.i), custom_sinh_real(x.r) * custom_sin_real(x.i)};
}

static Complex c_tanh(Complex x) {
    return c_div(c_sinh(x), c_cosh(x));
}

static void skip_whitespace(const char** s) {
    while (**s && isspace((unsigned char)**s)) (*s)++;
}

static Complex parse_expression(const char** s);

static Complex parse_primary(const char** s) {
    skip_whitespace(s);
    LOG("parse_primary() inicializado. Analisando: '%s'", *s);

    if (**s == 'i') {
        (*s)++;
        LOG("Identificado: numero imaginario 'i'");
        return (Complex){0.0, 1.0};
    }

    if (isalpha((unsigned char)**s)) {
        char func[16];
        int len = 0;
        while (isalpha((unsigned char)**s) && len < 15) {
            func[len++] = **s;
            (*s)++;
        }
        func[len] = '\0';
        LOG("Identificada funcao: '%s'", func);

        skip_whitespace(s);
        if (**s == '(') {
            (*s)++;
            Complex arg = parse_expression(s);
            skip_whitespace(s);
            if (**s == ')') (*s)++;

            LOG("Calculando funcao: %s(%g + %gi)", func, arg.r, arg.i);

            if (strcmp(func, "ln") == 0) return c_ln(arg);
            if (strcmp(func, "log") == 0) return c_div(c_ln(arg), (Complex){custom_ln_real(10.0), 0.0});
            if (strcmp(func, "exp") == 0) return c_exp(arg);
            if (strcmp(func, "sqrt") == 0) return c_sqrt(arg);
            if (strcmp(func, "sin") == 0) return c_sin(arg);
            if (strcmp(func, "cos") == 0) return c_cos(arg);
            if (strcmp(func, "tan") == 0) return c_tan(arg);
            if (strcmp(func, "sinh") == 0) return c_sinh(arg);
            if (strcmp(func, "cosh") == 0) return c_cosh(arg);
            if (strcmp(func, "tanh") == 0) return c_tanh(arg);
            
            return (Complex){0.0, 0.0};
        }
        return (Complex){0.0, 0.0};
    }

    if (**s == '(') {
        LOG("Identificado: abertura de parenteses '('");
        (*s)++;
        Complex val = parse_expression(s);
        skip_whitespace(s);
        if (**s == ')') {
            LOG("Identificado: fechamento de parenteses ')'");
            (*s)++;
        }
        return val;
    }

    char* end_ptr;
    double val_r = strtod(*s, &end_ptr);
    if (*s != end_ptr) {
        *s = end_ptr;
        
        skip_whitespace(s);
        if (**s == 'i') {
            (*s)++;
            LOG("Identificado: valor puramente imaginario %gi", val_r);
            return (Complex){0.0, val_r};
        }
        LOG("Identificado: valor real %g", val_r);
        return (Complex){val_r, 0.0};
    }
    
    return (Complex){0.0, 0.0};
}

static Complex parse_power(const char** s) {
    Complex left = parse_primary(s);
    skip_whitespace(s);
    if (**s == '^') {
        LOG("Operador de potencia identificado: '^'");
        (*s)++;
        Complex right = parse_power(s);
        left = c_pow(left, right);
        LOG("Resultado da potenciacao: %g + %gi", left.r, left.i);
    }
    return left;
}

static Complex parse_factor(const char** s) {
    skip_whitespace(s);
    if (**s == '-') {
        LOG("Operador unario negativo identificado: '-'");
        (*s)++;
        Complex val = parse_factor(s);
        return (Complex){-val.r, -val.i};
    }
    if (**s == '+') {
        LOG("Operador unario positivo identificado: '+'");
        (*s)++;
        return parse_factor(s);
    }
    return parse_power(s);
}

static Complex parse_term(const char** s) {
    Complex left = parse_factor(s);
    while (1) {
        skip_whitespace(s);
        if (**s == '*') {
            LOG("Operador de multiplicacao identificado: '*'");
            (*s)++;
            Complex right = parse_factor(s);
            left = c_mul(left, right);
            LOG("Resultado multiplicacao parcial: %g + %gi", left.r, left.i);
        } else if (**s == '/') {
            LOG("Operador de divisao identificado: '/'");
            (*s)++;
            Complex right = parse_factor(s);
            left = c_div(left, right);
            LOG("Resultado divisao parcial: %g + %gi", left.r, left.i);
        } else {
            break;
        }
    }
    return left;
}

static Complex parse_expression(const char** s) {
    LOG("parse_expression() avaliando expressao.");
    Complex left = parse_term(s);
    while (1) {
        skip_whitespace(s);
        if (**s == '+') {
            LOG("Operador de adicao identificado: '+'");
            (*s)++;
            Complex right = parse_term(s);
            left = c_add(left, right);
            LOG("Resultado adicao parcial: %g + %gi", left.r, left.i);
        } else if (**s == '-') {
            LOG("Operador de subtracao identificado: '-'");
            (*s)++;
            Complex right = parse_term(s);
            left = c_sub(left, right);
            LOG("Resultado subtracao parcial: %g + %gi", left.r, left.i);
        } else {
            break;
        }
    }
    return left;
}

static char result_buffer[256];

const char* evaluate_math_expression(const char* expression) {
    LOG("--- INICIANDO AVALIACAO ---");
    LOG("Expressao original: %s", expression);
    
    if (!expression || *expression == '\0') {
        LOG("Expressao vazia, retornando 0.");
        sprintf(result_buffer, "0");
        return result_buffer;
    }
    
    const char* ptr = expression;
    Complex c = parse_expression(&ptr);
    
    LOG("Avaliacao concluida. Resultado Complexo Interno: %g + %gi", c.r, c.i);

    if (custom_fabs(c.r) < 1e-12 && custom_fabs(c.i) < 1e-12) {
        sprintf(result_buffer, "0");
    } else if (custom_fabs(c.i) < 1e-12) {
        sprintf(result_buffer, "%g", c.r);
    } else if (custom_fabs(c.r) < 1e-12) {
        if (custom_fabs(c.i - 1.0) < 1e-12) sprintf(result_buffer, "i");
        else if (custom_fabs(c.i + 1.0) < 1e-12) sprintf(result_buffer, "-i");
        else sprintf(result_buffer, "%gi", c.i);
    } else {
        if (c.i > 0) {
            if (custom_fabs(c.i - 1.0) < 1e-12) sprintf(result_buffer, "%g + i", c.r);
            else sprintf(result_buffer, "%g + %gi", c.r, c.i);
        } else {
            // CORREÇÃO AQUI: As linhas quebradas foram corrigidas
            if (custom_fabs(c.i + 1.0) < 1e-12) sprintf(result_buffer, "%g - i", c.r);
            else sprintf(result_buffer, "%g - %gi", c.r, -c.i);
        }
    }
    
    LOG("--- AVALIACAO FINALIZADA ---");
    return result_buffer;
}

int main(void) {
    const char* expressions[] = {
        "sqrt(-4) + 2",
        "2 + sqrt(-4)",
        "i * sqrt(2)",
        "(-1) ^ 0.5",
        "ln(-1)"
    };

    int n = sizeof(expressions) / sizeof(expressions[0]);
    for (int i = 0; i < n; i++) {
        printf("Expr: %-22s = %s\n\n", expressions[i], evaluate_math_expression(expressions[i]));
    }

    return 0;
}