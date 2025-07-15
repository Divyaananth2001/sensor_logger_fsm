#include <iostream>

#include "sensor.hpp"

extern "C" {
    #include "FreeRTOS.h"
    #include "queue.h"
}

extern QueueHandle_t sensor_data_q;

TaskHandle_t loggerTask_handle;

void logger_log_collected_data() {
    sensor_data_t data;
    xQueueReceive(sensor_data_q, reinterpret_cast<void *>(&data), portMAX_DELAY);
    std::cout << "Collected Data:\n";
    std::cout << "accelerometer = " << data.acc << "\n";
    std::cout << "temperature = " << data.temp << "\n";
    std::cout << "smoke detector = " << data.smoke_detector << "\n";
    std::cout << "---------------\n";
}

void loggerTask(void* pvParameters) {
    std::cout << "Logger Task Init\n";
    while (true) {
        logger_log_collected_data();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second
    }
}

void loggerTask_init() {
    BaseType_t status = xTaskCreate(loggerTask, "Logger", 1000, NULL, 1, &loggerTask_handle);
    if (pdPASS != status) {
        std::cout << "ERROR: Failed to create loggerTask\n";
    }
}