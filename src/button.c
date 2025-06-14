/**
 * @file button.c
 * @brief Implementação do controle dos botões.
 *
 * Este arquivo contém a implementação da tarefa que lê os botões A e B,
 * aplica um debounce de software e suspende/retoma as tarefas do
 * LED e do buzzer.
 */

#include "button.h"
#include "led_rgb.h"
#include "buzzer.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Inicializa os pinos dos botões.
 *
 * Configura os pinos dos botões A e B como entradas digitais
 * com resistores de pull-up internos ativados.
 */
void button_init(void) {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
}

/**
 * @brief Tarefa de monitoramento dos botões.
 *
 * Verifica o estado dos botões periodicamente (polling). Quando um botão
 * é pressionado, aplica um debounce e então alterna o estado (suspenso/resumido)
 * da tarefa correspondente (LED para botão A, Buzzer para botão B).
 *
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 */
void button_task(void *pvParameters) {
    button_init();

    bool a_pressed = false;
    bool b_pressed = false;

    while (1) {
        // Lógica para o Botão A (controla o LED)
        // O pino em nível baixo significa que o botão está pressionado (devido ao pull-up)
        if (!gpio_get(BUTTON_A_PIN) && !a_pressed) {
            a_pressed = true; // Marca o botão como pressionado para evitar repetição

            // Verifica o estado atual da tarefa do LED
            eTaskState led_state = eTaskGetState(led_rgb_task_handle);

            // Alterna o estado: se suspensa, resume; senão, suspende.
            if (led_state == eSuspended) {
                vTaskResume(led_rgb_task_handle);
            } else {
                vTaskSuspend(led_rgb_task_handle);
            }
            // Debounce: aguarda um tempo para o usuário soltar o botão
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));
        } else if (gpio_get(BUTTON_A_PIN)) {
            a_pressed = false; // Reseta o estado quando o botão é solto
        }

        // Lógica para o Botão B (controla o Buzzer)
        if (!gpio_get(BUTTON_B_PIN) && !b_pressed) {
            b_pressed = true;

            // Verifica o estado atual da tarefa do buzzer
            eTaskState buzzer_state = eTaskGetState(buzzer_task_handle);

            // Alterna o estado da tarefa do buzzer
            if (buzzer_state == eSuspended) {
                vTaskResume(buzzer_task_handle);
            } else {
                vTaskSuspend(buzzer_task_handle);
            }
            // Debounce
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));
        } else if (gpio_get(BUTTON_B_PIN)) {
            b_pressed = false;
        }

        // Pequeno delay para a tarefa de polling não consumir 100% da CPU
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}