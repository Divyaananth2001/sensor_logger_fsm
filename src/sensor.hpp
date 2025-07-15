#ifndef SRC_SENSOR_HPP_
#define SRC_SENSOR_HPP_

typedef struct sensor
{
    float acc;
    float temp;
    bool smoke_detector;
}sensor_data_t;

void sensorTask_init(void);
void sensor_start_acquiring(void);
void sensor_stop_acquiring(void);

#endif  //SRC_SENSOR_HPP_