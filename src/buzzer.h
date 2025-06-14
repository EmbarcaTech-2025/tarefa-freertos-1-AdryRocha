/**
 * @file buzzer.h
 * @brief Definições para o controle do buzzer.
 *
 * Este arquivo contém as definições para inicialização e controle do buzzer,
 * incluindo a tarefa que gera o som.
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "FreeRTOS.h"
#include "task.h"

// Pino do Buzzer conforme o esquemático da BitDogLab V6
#define BUZZER_PIN 21

/**
 * @brief Handle para a tarefa do buzzer.
 * Usado para controlar a tarefa (suspender/retomar) a partir de outras tarefas.
 */
extern TaskHandle_t buzzer_task_handle;

/**
 * @brief Tarefa que controla o buzzer.
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 */
void buzzer_task(void *pvParameters);

#endif // BUZZER_H