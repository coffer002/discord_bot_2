#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cairo.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static cairo_surface_t* load_image_to_surface(const char* filename) {
    if (!filename || strlen(filename) == 0) return NULL;

    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) return NULL;

    // Converte RGBA -> ARGB32 pré-multiplicado exigido pelo Cairo (Little Endian)
    for (int i = 0; i < width * height; i++) {
        unsigned char r = data[i * 4 + 0];
        unsigned char g = data[i * 4 + 1];
        unsigned char b = data[i * 4 + 2];
        unsigned char a = data[i * 4 + 3];

        unsigned int alpha = a;
        unsigned int pr = (r * alpha) / 255;
        unsigned int pg = (g * alpha) / 255;
        unsigned int pb = (b * alpha) / 255;

        unsigned int *pixel = (unsigned int*)&data[i * 4];
        *pixel = (alpha << 24) | (pr << 16) | (pg << 8) | pb;
    }

    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
    cairo_surface_t *surface = cairo_image_surface_create_for_data(
        data, CAIRO_FORMAT_ARGB32, width, height, stride
    );

    static cairo_user_data_key_t key;
    cairo_surface_set_user_data(surface, &key, data, (cairo_destroy_func_t)stbi_image_free);
    return surface;
}


int generate_welcome_card(const char* bg_path, const char* avatar_path, const char* username, const char* subtext, const char* output_path) {
    int target_width = 1200;
    int target_height = 480;

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, target_width, target_height);
    cairo_t *cr = cairo_create(surface);

    // banner
    cairo_surface_t *bg_surface = load_image_to_surface(bg_path);
    if (bg_surface) {
        int bg_w = cairo_image_surface_get_width(bg_surface);
        int bg_h = cairo_image_surface_get_height(bg_surface);
        cairo_save(cr);
        cairo_scale(cr, (double)target_width / bg_w, (double)target_height / bg_h);
        cairo_set_source_surface(cr, bg_surface, 0, 0);
        cairo_paint(cr);
        cairo_restore(cr);
        cairo_surface_destroy(bg_surface);
    } else {
        // Background escuro padrão de fallback
        cairo_pattern_t *pat = cairo_pattern_create_linear(0, 0, target_width, target_height);
        cairo_pattern_add_color_stop_rgb(pat, 0, 0.1, 0.1, 0.2);
        cairo_pattern_add_color_stop_rgb(pat, 1, 0.05, 0.05, 0.1);
        cairo_set_source(cr, pat);
        cairo_paint(cr);
        cairo_pattern_destroy(pat);
    }


    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.40);
    cairo_paint(cr);

    cairo_set_source_rgba(cr, 0.1, 0.1, 0.15, 0.75);
    double card_x = 50, card_y = 40, card_w = 1100, card_h = 400, radius = 25;

    cairo_new_sub_path(cr);
    cairo_arc(cr, card_x + card_w - radius, card_y + radius, radius, -M_PI/2, 0);
    cairo_arc(cr, card_x + card_w - radius, card_y + card_h - radius, radius, 0, M_PI/2);
    cairo_arc(cr, card_x + radius, card_y + card_h - radius, radius, M_PI/2, M_PI);
    cairo_arc(cr, card_x + radius, card_y + radius, radius, M_PI, 3*M_PI/2);
    cairo_close_path(cr);
    cairo_fill(cr);

//avatar
    double avatar_cx = 220;
    double avatar_cy = 240;
    double avatar_radius = 110;

    cairo_surface_t *avatar_surface = load_image_to_surface(avatar_path);
    if (avatar_surface) {
        cairo_save(cr);
        cairo_arc(cr, avatar_cx, avatar_cy, avatar_radius, 0, 2 * M_PI);
        cairo_clip(cr);

        int av_w = cairo_image_surface_get_width(avatar_surface);
        int av_h = cairo_image_surface_get_height(avatar_surface);

        cairo_translate(cr, avatar_cx - avatar_radius, avatar_cy - avatar_radius);
        cairo_scale(cr, (2.0 * avatar_radius) / av_w, (2.0 * avatar_radius) / av_h);
        cairo_set_source_surface(cr, avatar_surface, 0, 0);
        cairo_paint(cr);
        cairo_restore(cr);
        cairo_surface_destroy(avatar_surface);
    }


    cairo_arc(cr, avatar_cx, avatar_cy, avatar_radius, 0, 2 * M_PI);
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.9);
    cairo_set_line_width(cr, 6.0);
    cairo_stroke(cr);


    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_set_font_size(cr, 50.0);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_move_to(cr, 370, 220);
    cairo_show_text(cr, username);

    // Subtexto
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 30.0);
    cairo_set_source_rgb(cr, 0.8, 0.85, 0.95);
    cairo_move_to(cr, 370, 275);
    cairo_show_text(cr, subtext);


    cairo_status_t status = cairo_surface_write_to_png(surface, output_path);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return (status == CAIRO_STATUS_SUCCESS) ? 0 : -1;
}