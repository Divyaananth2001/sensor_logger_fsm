#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

extern "C" {
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
}

#include "sensor.hpp"
#include "logger.hpp"
#include "fsm.hpp"
#include "cli_commands.hpp"

using namespace std;

QueueHandle_t sensor_data_q;
QueueHandle_t event_q;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        exit(0);
    }
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // Handle stack overflow (log, halt, reset, etc.)
    printf("\n[ERROR] Stack overflow in task: %s\n", pcTaskName);
    fflush(stdout);

    // Optionally halt execution for debugging
    taskDISABLE_INTERRUPTS();
    for (;;);  // Hang forever
}

int main() {
    std::signal(SIGINT, signal_handler);

    sensor_data_q = xQueueCreate(5, sizeof(sensor_data_t));
    if(!sensor_data_q) {
        cout << "Failed to create sensor_data_q!!!\n";
        return -1;
    }
    event_q = xQueueCreate(5, sizeof(Event));
    if(!event_q) {
        cout << "Failed to create event_q!!!\n";
        return -1;
    }

    sensorTask_init();
    fsmTask_init();
    loggerTask_init();
    cli_handler_init();

    vTaskStartScheduler();

    return 0;
}