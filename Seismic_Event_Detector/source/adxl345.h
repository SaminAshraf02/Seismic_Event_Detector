#ifndef ADXL345_H
#define ADXL345_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} adxl_raw_t;

bool    adxl_init(void);                 /* pins + I2C + ID check + config */
bool    adxl_read_raw(adxl_raw_t *s);    /* waits for new data, then reads X/Y/Z */
bool    adxl_calibrate(uint16_t n);      /* average n samples at rest */
void    adxl_get_rest(adxl_raw_t *r);    /* read back the stored rest offset */
void    adxl_apply_offset(const adxl_raw_t *raw, adxl_raw_t *corr);
int32_t adxl_to_mg(int32_t counts);      /* LSB counts to milli-g */

#endif
