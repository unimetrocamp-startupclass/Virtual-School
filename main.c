#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600

// Enums MenuOption e FaseOption permanecem os mesmos
enum MenuOption {
    MENU_INICIAR,
    MENU_CREDITOS,
    MENU_SAIR,
    MENU_TOTAL
};

enum FaseOption {
    FASE_1,
    FASE_VOLTAR,
    FASE_TOTAL
};

// ---------- Fase 1 ----------
void fase_um(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font) {
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);

    ALLEGRO_BITMAP *background = al_load_bitmap("fase1_background.png");
    ALLEGRO_BITMAP *car_img = al_load_bitmap("carro.png");
    ALLEGRO_BITMAP *obst_img = al_load_bitmap("carro2.png");

    if (!background || !car_img || !obst_img) {
        fprintf(stderr, "Erro carregando imagens da fase 1:\n");
        if (!background) fprintf(stderr, " - Falha ao carregar fase1_background.png\n");
        if (!car_img) fprintf(stderr, " - Falha ao carregar carro.png\n");
        if (!obst_img) fprintf(stderr, " - Falha ao carregar carro2.png\n");

        if (background) al_destroy_bitmap(background);
        if (car_img) al_destroy_bitmap(car_img);
        if (obst_img) al_destroy_bitmap(obst_img);
        if (queue) al_destroy_event_queue(queue);
        if (timer) al_destroy_timer(timer);
        return;
    }

    float car_w = 100;
    float car_h = 100;
    float car_x = 100;
    float car_y = 150;

    float vaga_x = 650;
    float vaga_y = 350;
    float vaga_w = car_w;
    float vaga_h = car_h;

    float obst_w_real = al_get_bitmap_width(obst_img);
    float obst_h_real = al_get_bitmap_height(obst_img);
    float espacamento = 10;
    float obst_x = vaga_x - obst_w_real - espacamento;
    float obst_y = vaga_y;

    bool up = false, down = false, left = false, right = false;
    bool running = true;
    bool venceu = false;
    bool perdeu = false;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(display));

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (!venceu && !perdeu) {
                if (up) car_y -= 4;
                if (down) car_y += 4;
                if (left) car_x -= 4;
                if (right) car_x += 4;

                if (car_x < 0) car_x = 0;
                if (car_y < 0) car_y = 0;
                if (car_x + car_w > SCREEN_W) car_x = SCREEN_W - car_w;
                if (car_y + car_h > SCREEN_H) car_y = SCREEN_H - car_h;

                // Vitória com ponto central no alvo
                float carro_centro_x = car_x + car_w / 2;
                float carro_centro_y = car_y + car_h / 2;

                float alvo_w = 20;
                float alvo_h = 20;
                float alvo_x = vaga_x + (vaga_w - alvo_w) / 2;
                float alvo_y = vaga_y + (vaga_h - alvo_h) / 2;

                if (carro_centro_x >= alvo_x && carro_centro_x <= alvo_x + alvo_w &&
                    carro_centro_y >= alvo_y && carro_centro_y <= alvo_y + alvo_h) {
                    venceu = true;
                }

                // Colisão com obstáculo (com área menor)
                float margem_obst = 10.0;
                float obst_col_x = obst_x + margem_obst;
                float obst_col_y = obst_y + margem_obst;
                float obst_col_w = obst_w_real - 2 * margem_obst;
                float obst_col_h = obst_h_real - 2 * margem_obst;

                if (car_x + car_w > obst_col_x && car_x < obst_col_x + obst_col_w &&
                    car_y + car_h > obst_col_y && car_y < obst_col_y + obst_col_h) {
                    perdeu = true;
                }
            }

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(background, 0, 0, 0);

            // Vaga visual
            al_draw_filled_rectangle(
                vaga_x, vaga_y,
                vaga_x + vaga_w, vaga_y + vaga_h,
                al_map_rgb(0, 255, 0)
            );

            al_draw_bitmap(obst_img, obst_x, obst_y, 0);
            al_draw_bitmap(car_img, car_x, car_y, 0);

            if (venceu) {
                al_draw_text(font, al_map_rgb(255, 255, 0), SCREEN_W / 2, 50, ALLEGRO_ALIGN_CENTER, "Parabéns! Você estacionou!");
            }
            if (perdeu) {
                al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2, 50, ALLEGRO_ALIGN_CENTER, "Você perdeu! Colidiu com outro carro");
            }

            al_flip_display();

            // Espera 5 segundos antes de retornar ao menu
            if (venceu || perdeu) {
                al_rest(5.0);
                running = false;
            }

        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
            if (!venceu && !perdeu) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_W: up = true; break;
                    case ALLEGRO_KEY_S: down = true; break;
                    case ALLEGRO_KEY_A: left = true; break;
                    case ALLEGRO_KEY_D: right = true; break;
                }
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (!venceu && !perdeu) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_W: up = false; break;
                    case ALLEGRO_KEY_S: down = false; break;
                    case ALLEGRO_KEY_A: left = false; break;
                    case ALLEGRO_KEY_D: right = false; break;
                }
            }
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
    }

    // Liberação de recursos
    al_destroy_bitmap(background);
    al_destroy_bitmap(car_img);
    al_destroy_bitmap(obst_img);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
}


void exibir_creditos(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_BITMAP *background_menu) { // Usa background do menu
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    al_draw_bitmap(background_menu, 0, 0, 0);
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 100, ALLEGRO_ALIGN_CENTER, "Créditos");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 180, ALLEGRO_ALIGN_CENTER, "Gabriel Lima");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 220, ALLEGRO_ALIGN_CENTER, "Geovanna Nayara");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 260, ALLEGRO_ALIGN_CENTER, "Natália Souza");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, "Nicolas Souza");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 400, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para voltar");
    al_flip_display();

    bool creditos_rodando = true;
    while(creditos_rodando){
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            creditos_rodando = false;
        }
    }
    al_destroy_event_queue(queue);
}

// ---------- Seletor de Fase ---------- (copiado da sua versão anterior)
void selecionar_fase(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_BITMAP *background_menu) { // Usa background do menu
    int fase_selecionada = 0;
    bool selecionando = true;

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    while (selecionando) {
        al_draw_bitmap(background_menu, 0, 0, 0);

        const char *fases[FASE_TOTAL] = {"Fase 1", "Voltar"};

        for (int i = 0; i < FASE_TOTAL; i++) {
            al_draw_textf(font, (i == fase_selecionada) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255),
                          SCREEN_W / 2, 200 + i * 50, ALLEGRO_ALIGN_CENTER, "%s", fases[i]);
        }

        al_flip_display();

        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    fase_selecionada = (fase_selecionada + FASE_TOTAL - 1) % FASE_TOTAL;
                    break;
                case ALLEGRO_KEY_DOWN:
                    fase_selecionada = (fase_selecionada + 1) % FASE_TOTAL;
                    break;
                case ALLEGRO_KEY_ENTER:
                    if (fase_selecionada == FASE_1) {
                        fase_um(display, font); 
                    } else if (fase_selecionada == FASE_VOLTAR) {
                        selecionando = false;
                    }
                    break;
                 case ALLEGRO_KEY_ESCAPE: 
                    selecionando = false;
                    break;
            }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
             selecionando = false; 
        }
    }
    al_destroy_event_queue(queue);
}

// ---------- MAIN ---------- (copiado da sua versão anterior)
int main() {
    if(!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }
    if(!al_install_keyboard()) {
        fprintf(stderr, "Falha ao instalar teclado.\n");
        return -1;
    }
    al_init_font_addon(); 
    if(!al_init_ttf_addon()) {
        fprintf(stderr, "Falha ao inicializar ttf addon.\n");
        return -1;
    }
    if(!al_init_primitives_addon()){
        fprintf(stderr, "Falha ao inicializar primitives addon.\n");
        return -1;
    }
    if(!al_init_image_addon()){
        fprintf(stderr, "Falha ao inicializar image addon.\n");
        return -1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display){
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }
    
    ALLEGRO_FONT *font = al_load_ttf_font("VL-Gothic-Regular.ttf", 24, 0); 
    ALLEGRO_BITMAP *background_menu = al_load_bitmap("placeholder.png");

    if (!font) {
        fprintf(stderr, "Falha ao carregar fonte VL-Gothic-Regular.ttf.\n");
        al_destroy_display(display); 
        return -1;
    }
    if (!background_menu) {
        fprintf(stderr, "Falha ao carregar imagem de fundo placeholder.png para o menu.\n");
        al_destroy_font(font); 
        al_destroy_display(display);
        return -1;
    }
    
    al_set_window_title(display, "Jogo de Trânsito Educativo");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if(!queue){
        fprintf(stderr, "Falha ao criar event queue para o menu.\n");
        al_destroy_bitmap(background_menu);
        al_destroy_font(font);
        al_destroy_display(display);
        return -1;
    }
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    int menu_selecionado = 0;
    bool rodando = true;

    while (rodando) {
        al_draw_bitmap(background_menu, 0, 0, 0);

        const char *opcoes[MENU_TOTAL] = {"Iniciar Jogo", "Créditos", "Sair"};

        for (int i = 0; i < MENU_TOTAL; i++) {
            al_draw_textf(font, (i == menu_selecionado) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255),
                          SCREEN_W / 2, 200 + i * 50, ALLEGRO_ALIGN_CENTER, "%s", opcoes[i]);
        }

        al_flip_display();

        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    menu_selecionado = (menu_selecionado + MENU_TOTAL - 1) % MENU_TOTAL;
                    break;
                case ALLEGRO_KEY_DOWN:
                    menu_selecionado = (menu_selecionado + 1) % MENU_TOTAL;
                    break;
                case ALLEGRO_KEY_ENTER:
                    if (menu_selecionado == MENU_INICIAR) {
                        selecionar_fase(display, font, background_menu); 
                    } else if (menu_selecionado == MENU_CREDITOS) {
                        exibir_creditos(display, font, background_menu);
                    } else if (menu_selecionado == MENU_SAIR) {
                        rodando = false;
                    }
                    break;
                 case ALLEGRO_KEY_ESCAPE: 
                    rodando = false;
                    break;
            }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { 
            rodando = false;
        }
    }

    al_destroy_event_queue(queue);
    al_destroy_font(font);
    al_destroy_bitmap(background_menu);
    al_destroy_display(display);
    
    // Opcional: desinicializar addons explicitamente
    // al_shutdown_primitives_addon();
    // al_shutdown_image_addon();
    // al_shutdown_font_addon();
    // al_uninstall_keyboard();

    return 0;
}
