/**
 * @file button.h
 * @brief Definições para o controle dos botões.
 *
 * Este arquivo contém as definições dos pinos, da tarefa dos botões
 * e do tempo de debounce.
 */

#ifndef BUTTON_H
#define BUTTON_H

// Pinos dos botões conforme o esquemático da BitDogLab V6
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// Tempo de debounce em milissegundos para evitar leituras múltiplas
#define DEBOUNCE_TIME_MS 200

/**
 * @brief Tarefa que monitora os botões.
 * @param pvParameters Parâmetros da tarefa (não utilizado).
 */
void button_task(void *pvParameters);

#endif // BUTTON_H