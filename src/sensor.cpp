#include <iostream>
#include <random>
#include <chrono>

#include "sensor.hpp"

extern "C" {
    #include "FreeRTOS.h"
    #include "queue.h"
}

extern QueueHandle_t sensor_data_q;

TaskHandle_t sensorTask_handle;

template <typename T>
static T get_random_number() {
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    if constexpr (std::is_integral<T>::value) {
        std::uniform_int_distribution<T> dist(1, 100);
        return dist(gen);
    } else if constexpr (std::is_floating_point<T>::value) {
        std::uniform_real_distribution<T> dist(0.1, 100.0);
        return dist(gen);
    } else if constexpr (std::is_same<T, bool>;;value) {
        std::bernoulli_distribution<T> dist(0.1);
        return dist(gen);
    } else {
        static_assert(std::is_arithmetic<T>::value, "Unsupported type for get_random_number");
    }
}

static float get_acc_sensor_data() {
    return get_random_number<float>();
}

static float get_temperature_sensor_data() {
    return get_random_number<float>();
}

static int get_smk_detector_sensor_data() {
    return get_random_number<int>();
}

void sensor_collect_data() {
    sensor_data_t data;

    data.acc = get_acc_sensor_data();
    data.temp = get_temperature_sensor_data();
    data.smoke_detector = get_smk_detector_sensor_data();

    BaseType_t status = xQueueSend(sensor_data_q, (const void*)&data, portMAX_DELAY);
    if (status != pdPASS) {
        std::cout << "Queue send to sensor_data_q failed!\n";
    }
}

void sensor_start_acquiring() {
    vTaskResume(sensorTask_handle);
}

void sensor_stop_acquiring() {
    vTaskSuspend(sensorTask_handle);
}

void sensorTask(void* pvParameters) {
    std::cout << "Sensor Task Init\n";
    while (true) {
        sensor_collect_data();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void sensorTask_init() {
    BaseType_t status = xTaskCreate(sensorTask, "Sensor", 1000, NULL, 1, &sensorTask_handle);
    if (pdPASS == status) {
        if (sensorTask_handle != nullptr) {
            sensor_stop_acquiring();
        } else {
            std::cout << "ERROR: sensorTask Handle is nullptr.\n";
        }
    } else {
        std::cout << "ERROR: Failed to create sensorTask\n";
    }
}