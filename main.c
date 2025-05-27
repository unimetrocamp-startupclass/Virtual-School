#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600

enum MenuOption { MENU_INICIAR, MENU_CREDITOS, MENU_SAIR, MENU_TOTAL };
enum FaseOption { FASE_1, FASE_VOLTAR, FASE_TOTAL };

void exibir_creditos(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 100, ALLEGRO_ALIGN_CENTER, "Créditos");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 180, ALLEGRO_ALIGN_CENTER, "Gabriel Lima");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 220, ALLEGRO_ALIGN_CENTER, "Geovanna Nayara");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 260, ALLEGRO_ALIGN_CENTER, "Natália Souza");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 300, ALLEGRO_ALIGN_CENTER, "Nicolas Souza");
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 400, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para voltar");
    al_flip_display();

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());

    ALLEGRO_EVENT event;
    al_wait_for_event(queue, &event);
    al_destroy_event_queue(queue);
}

void fase_um(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font) {
    // Posição inicial do carro (canto esquerdo, centralizado verticalmente)
    float carro_x = 50;
    float carro_y = SCREEN_H / 2 - 15;

    const float velocidade = 4.0;

    bool keys[4] = {false, false, false, false}; // W A S D
    bool rodando = true;
    bool venceu = false;

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60);

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    // Definição da vaga
    float vaga_x = SCREEN_W - 150;
    float vaga_y = (SCREEN_H / 2) + 40; // Um pouco mais abaixo do carro
    float vaga_largura = 80;
    float vaga_altura = 40;

    // Tamanho do carro
    float carro_largura = 50;
    float carro_altura = 30;

    while (rodando) {
        while (!al_is_event_queue_empty(queue)) {
            ALLEGRO_EVENT event;
            al_get_next_event(queue, &event);

            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                rodando = false;
            }
            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_W: keys[0] = true; break;
                    case ALLEGRO_KEY_A: keys[1] = true; break;
                    case ALLEGRO_KEY_S: keys[2] = true; break;
                    case ALLEGRO_KEY_D: keys[3] = true; break;
                    case ALLEGRO_KEY_ESCAPE: rodando = false; break;
                }
            }
            if (event.type == ALLEGRO_EVENT_KEY_UP) {
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_W: keys[0] = false; break;
                    case ALLEGRO_KEY_A: keys[1] = false; break;
                    case ALLEGRO_KEY_S: keys[2] = false; break;
                    case ALLEGRO_KEY_D: keys[3] = false; break;
                }
            }
        }

        // Atualização de movimento
        if (keys[0] && carro_y > 0) carro_y -= velocidade;
        if (keys[1] && carro_x > 0) carro_x -= velocidade;
        if (keys[2] && carro_y < SCREEN_H - carro_altura) carro_y += velocidade;
        if (keys[3] && carro_x < SCREEN_W - carro_largura) carro_x += velocidade;

        // Verifica vitória (carro dentro da vaga)
        if (carro_x >= vaga_x &&
            carro_x + carro_largura <= vaga_x + vaga_largura &&
            carro_y >= vaga_y &&
            carro_y + carro_altura <= vaga_y + vaga_altura) {
            venceu = true;
        } else {
            venceu = false;
        }

        // Desenho
        al_clear_to_color(al_map_rgb(30, 30, 30));

        // Desenha a vaga
        al_draw_filled_rectangle(vaga_x, vaga_y, vaga_x + vaga_largura, vaga_y + vaga_altura, al_map_rgb(0, 255, 0));

        // Desenha o carro
        al_draw_filled_rectangle(carro_x, carro_y, carro_x + carro_largura, carro_y + carro_altura, al_map_rgb(255, 0, 0));

        // Se venceu, exibe mensagem
        if (venceu) {
            al_draw_text(font, al_map_rgb(255, 255, 0), SCREEN_W/2, 50, ALLEGRO_ALIGN_CENTER, "Estacionamento Perfeito!");
            al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W/2, 80, ALLEGRO_ALIGN_CENTER, "Pressione ESC para voltar");
        }

        al_flip_display();
        al_rest(1.0 / 60);
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}




bool selecionar_fase(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font) {
    int fase_selecionada = 0;
    bool selecionando = true;

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());

    while (selecionando) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        const char *fases[FASE_TOTAL] = {"Fase 1", "Voltar"};

        for (int i = 0; i < FASE_TOTAL; i++) {
            al_draw_textf(font, (i == fase_selecionada) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255),
                          SCREEN_W / 2, 150 + i * 50, ALLEGRO_ALIGN_CENTER, "%s", fases[i]);
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
        }
    }

    al_destroy_event_queue(queue);
    return true;
}

int main() {
    al_init();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_W, SCREEN_H);
    ALLEGRO_FONT *font = al_load_ttf_font("VL-Gothic-Regular.ttf", 24, 0);

    if (!font) {
        fprintf(stderr, "Falha ao carregar fonte.\n");
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());

    int menu_selecionado = 0;
    bool rodando = true;

    while (rodando) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

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
                        selecionar_fase(display, font);
                    } else if (menu_selecionado == MENU_CREDITOS) {
                        exibir_creditos(display, font);
                    } else if (menu_selecionado == MENU_SAIR) {
                        rodando = false;
                    }
                    break;
            }
        }
    }

    al_destroy_event_queue(queue);
    al_destroy_font(font);
    al_destroy_display(display);

    return 0;
}

