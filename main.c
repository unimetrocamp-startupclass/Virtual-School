#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600

enum MenuOption {
    MENU_INICIAR,
    MENU_CREDITOS,
    MENU_SAIR,
    MENU_TOTAL
};


bool exibir_creditos(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font) {
    al_clear_to_color(al_map_rgb(0, 0, 0)); 
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 100, ALLEGRO_ALIGN_CENTER, "Créditos");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 180, ALLEGRO_ALIGN_CENTER, "Gabriel Lima");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 220, ALLEGRO_ALIGN_CENTER, "Geovanna Nayara");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 260, ALLEGRO_ALIGN_CENTER, "Natália Souza");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, "Nicolas Souza");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 400, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para voltar");
    al_flip_display();

    ALLEGRO_EVENT_QUEUE *credits_event_queue = al_create_event_queue();
    if (!credits_event_queue) {
        fprintf(stderr, "Falha ao criar fila de eventos nos créditos.\n");
        return false;
    }
    al_register_event_source(credits_event_queue, al_get_keyboard_event_source());
    al_register_event_source(credits_event_queue, al_get_display_event_source(display));

    bool credits_done = false;
    bool display_closed_during_credits = false;

    while (!credits_done) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(credits_event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            credits_done = true;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            credits_done = true;
            display_closed_during_credits = true;
        }
    }

    al_destroy_event_queue(credits_event_queue);
    return display_closed_during_credits;
}

int main() {
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar o Allegro.\n");
        return -1;
    }

    if (!al_init_image_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon de imagem.\n");
        return -1;
    }

    if (!al_init_font_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon de fontes.\n");
        al_shutdown_image_addon();
        return -1;
    }

    if (!al_init_ttf_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon ttf.\n");
        al_shutdown_font_addon();
        al_shutdown_image_addon();
        return -1;
    }

    if (!al_install_keyboard()) {
        fprintf(stderr, "Falha ao instalar o teclado.\n");
        al_shutdown_ttf_addon();
        al_shutdown_font_addon();
        al_shutdown_image_addon();
        return -1;
    }

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar o addon de primitivas.\n");
        al_shutdown_ttf_addon();
        al_shutdown_font_addon();
        al_shutdown_image_addon();
        return -1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        fprintf(stderr, "Falha ao criar a janela.\n");
        al_shutdown_primitives_addon();
        al_shutdown_ttf_addon();
        al_shutdown_font_addon();
        al_shutdown_image_addon();
        return -1;
    }

    ALLEGRO_BITMAP *background_image = al_load_bitmap("placeholder.png");
    if (!background_image) {
        fprintf(stderr, "Falha ao carregar a imagem de fundo 'placeholder.png'.\n Certifique-se que o arquivo existe no local correto.\n");
    }

    ALLEGRO_FONT *font = al_load_font("VL-Gothic-Regular.ttf", 32, 0);
    if (!font) {
        fprintf(stderr, "Falha ao carregar a fonte. Verifique se o arquivo 'VL-Gothic-Regular.ttf' está na pasta do executável.\n");
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if (!queue) {
        fprintf(stderr, "Falha ao criar a fila de eventos.\n");
        al_destroy_font(font);
        return -1;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    int opcao = MENU_INICIAR;
    bool done = false;

    while (!done) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    opcao = (opcao + MENU_TOTAL - 1) % MENU_TOTAL;
                    break;
                case ALLEGRO_KEY_DOWN:
                    opcao = (opcao + 1) % MENU_TOTAL;
                    break;
                case ALLEGRO_KEY_ENTER:
                    if (opcao == MENU_INICIAR) {
                        printf("Iniciar Jogo Pressionado!\n");
                    } else if (opcao == MENU_CREDITOS) {
                        if (exibir_creditos(display, font)) {
                            done = true;
                        }
                    } else if (opcao == MENU_SAIR) {
                        done = true;
                    }
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    done = true;
                    break;
            }
        }

        if (!done) {
            if (background_image) {
                al_draw_bitmap(background_image, 0, 0, 0);

                ALLEGRO_COLOR aviso_cor = al_map_rgb(0, 0, 0); 

                int altura_linha_aviso = al_get_font_line_height(font) / 1.5; 
                al_draw_text(font, aviso_cor, 10, SCREEN_H - altura_linha_aviso - 5, ALLEGRO_ALIGN_LEFT, "Aviso: Imagem de fundo é placeholder");

            } else {
                al_clear_to_color(al_map_rgb(0, 100, 255));
            }

            const char *opcoes_texto[MENU_TOTAL] = { "Iniciar", "Créditos", "Sair" };
            for (int i = 0; i < MENU_TOTAL; i++) {
                ALLEGRO_COLOR cor = (i == opcao) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
                al_draw_text(font, cor, SCREEN_W / 2, 200 + i * 50, ALLEGRO_ALIGN_CENTER, opcoes_texto[i]);
            }
            al_flip_display();
        }
    }
}
