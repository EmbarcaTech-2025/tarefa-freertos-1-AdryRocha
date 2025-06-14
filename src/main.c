/**
 * @file main.c
 * @brief Ponto de entrada principal do sistema multitarefa para a BitDogLab.
 *
 * Este arquivo inicializa o sistema, cria as tarefas para o LED RGB,
 * o buzzer e os botões, e inicia o escalonador do FreeRTOS.
 */

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

// Inclui os cabeçalhos dos módulos de periféricos
#include "led_rgb.h"
#include "buzzer.h"
#include "button.h"

/**
 * @brief Ponto de entrada principal do programa.
 *
 * - Inicializa a E/S padrão (para depuração via USB).
 * - Cria três tarefas concorrentes:
 * 1. led_rgb_task: Controla o LED RGB.
 * 2. buzzer_task: Controla o buzzer.
 * 3. button_task: Monitora os botões para controlar as outras duas tarefas.
 * - Inicia o escalonador do FreeRTOS.
 *
 * @return int Nunca retorna, pois o controle é passado para o FreeRTOS.
 */
int main() {
    // Inicializa a comunicação serial USB para depuração (opcional)
    stdio_init_all();

    // Cria a tarefa para o LED RGB.
    // Parâmetros:
    // - led_rgb_task: A função da tarefa.
    // - "LED_Task": Nome da tarefa (para depuração).
    // - 256: Tamanho da pilha em palavras (256 * 4 bytes).
    // - NULL: Parâmetros para a tarefa (nenhum).
    // - 1: Prioridade da tarefa (prioridades mais baixas primeiro).
    // - &led_rgb_task_handle: Handle para controlar a tarefa.
    xTaskCreate(led_rgb_task, "LED_Task", 256, NULL, 1, &led_rgb_task_handle);

    // Cria a tarefa para o buzzer com os mesmos parâmetros.
    xTaskCreate(buzzer_task, "Buzzer_Task", 256, NULL, 1, &buzzer_task_handle);

    // Cria a tarefa para os botões.
    // Um handle não é necessário, pois esta tarefa não será controlada por outras.
    // A prioridade é 2, maior que as outras, para garantir que os botões
    // tenham resposta rápida.
    xTaskCreate(button_task, "Button_Task", 256, NULL, 2, NULL);

    // Inicia o escalonador do FreeRTOS.
    // A partir deste ponto, o FreeRTOS assume o controle do processador
    // e começa a executar as tarefas criadas.
    vTaskStartScheduler();

    // O código abaixo nunca será alcançado, a menos que haja um erro
    // grave e o escalonador pare (por exemplo, falta de memória).
    while (1) {
        // Loop infinito de segurança.
    };
}