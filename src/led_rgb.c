/**
 * @file led_rgb.c
 * @brief Implementação do controle do LED RGB.
 *
 * Este arquivo contém a implementação da tarefa que alterna as cores do LED RGB.
 */

#include "led_rgb.h"      // Para protótipo da função e definições dos pinos
#include "pico/stdlib.h"  // Para as funções gpio_...
#include "FreeRTOS.h"     // Para os tipos do FreeRTOS
#include "task.h"         // Para vTaskDelay, TaskHandle_t, etc.

// Array com os pinos do LED para facilitar o acesso.
const uint8_t led_pins[] = {LED_R_PIN, LED_G_PIN, LED_B_PIN};

// Handle da tarefa, usado pela tarefa dos botões para suspender/retomar esta.
// A variável é declarada aqui, mas o main.c irá atribuir o valor a ela.
TaskHandle_t led_rgb_task_handle = NULL;

/**
 * @brief Tarefa que controla o LED RGB.
 *
 * Inicializa os pinos do LED e entra em um loop infinito para
 * alternar ciclicamente entre as cores vermelho, verde e azul
 * a cada 500ms. Esta tarefa pode ser suspensa e retomada pela tarefa dos botões.
 *
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 */
void led_rgb_task(void *pvParameters)
{
    // Inicializa os 3 pinos do LED como saídas digitais.
    for (int i = 0; i < 3; i++)
    {
        gpio_init(led_pins[i]);
        gpio_set_dir(led_pins[i], GPIO_OUT);
        gpio_put(led_pins[i], 0); // Garante que todos comecem apagados
    }

    int current_color_index = 0;

    // Loop infinito da tarefa
    while (1)
    {
        // Acende o LED da cor atual
        gpio_put(led_pins[current_color_index], 1);

        // Aguarda 500ms. vTaskDelay libera o processador para outras tarefas.
        vTaskDelay(pdMS_TO_TICKS(500));

        // Apaga o LED da cor atual antes de passar para a próxima
        gpio_put(led_pins[current_color_index], 0);

        // Avança para a próxima cor (0 -> 1 -> 2 -> 0)
        current_color_index++;
        if (current_color_index >= 3)
        {
            current_color_index = 0;
        }
    }
}
