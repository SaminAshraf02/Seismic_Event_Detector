/* adxl345.c - ADXL345 driver over I2C0 (PTE24 = SCL, PTE25 = SDA)
 * Register map: Analog Devices ADXL345 datasheet.
 * I2C access: NXP MCUXpresso SDK fsl_i2c driver. */

#include "adxl345.h"
#include "board.h"
#include "fsl_i2c.h"
#include "fsl_clock.h"

#define ADXL_I2C          I2C0
#define ADXL_ADDR         0x53U      /* SDO tied to GND; use 0x1D if tied to 3.3 V */

#define REG_DEVID         0x00U
#define REG_BW_RATE       0x2CU
#define REG_POWER_CTL     0x2DU
#define REG_INT_SOURCE    0x30U
#define REG_DATA_FORMAT   0x31U
#define REG_DATAX0        0x32U

#define DEVID_VALUE       0xE5U
#define BW_RATE_100HZ     0x0AU      /* output data rate 100 Hz, normal power */
#define DATA_FMT_FULLRES_4G 0x09U    /* FULL_RES (bit 3) + range +/-4 g (bits 1:0 = 01) */
#define POWER_MEASURE     0x08U      /* Measure bit */
#define INT_DATA_READY    0x80U

#define READY_POLL_LIMIT  5000U      /* about 0.5 s of polling at 400 kHz */

static adxl_raw_t rest = {0, 0, 0};

static bool reg_write(uint8_t reg, uint8_t val)
{
    i2c_master_transfer_t x = {0};
    x.slaveAddress   = ADXL_ADDR;
    x.direction      = kI2C_Write;
    x.subaddress     = reg;
    x.subaddressSize = 1;
    x.data           = &val;
    x.dataSize       = 1;
    x.flags          = kI2C_TransferDefaultFlag;
    return I2C_MasterTransferBlocking(ADXL_I2C, &x) == kStatus_Success;
}

static bool reg_read(uint8_t reg, uint8_t *buf, size_t n)
{
    i2c_master_transfer_t x = {0};
    x.slaveAddress   = ADXL_ADDR;
    x.direction      = kI2C_Read;
    x.subaddress     = reg;
    x.subaddressSize = 1;
    x.data           = buf;
    x.dataSize       = n;
    x.flags          = kI2C_TransferDefaultFlag;
    return I2C_MasterTransferBlocking(ADXL_I2C, &x) == kStatus_Success;
}

bool adxl_init(void)
{
    uint8_t id = 0;

    /* pins: ALT5 = I2C0, open-drain */
    CLOCK_EnableClock(kCLOCK_PortE);
    PORTE->PCR[24] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;
    PORTE->PCR[25] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;

    i2c_master_config_t cfg;
    I2C_MasterGetDefaultConfig(&cfg);
    cfg.baudRate_Bps = 400000U;
    I2C_MasterInit(ADXL_I2C, &cfg, CLOCK_GetFreq(I2C0_CLK_SRC));

    if (!reg_read(REG_DEVID, &id, 1) || id != DEVID_VALUE) {
        return false;
    }
    if (!reg_write(REG_DATA_FORMAT, DATA_FMT_FULLRES_4G)) return false;
    if (!reg_write(REG_BW_RATE, BW_RATE_100HZ))           return false;
    if (!reg_write(REG_POWER_CTL, POWER_MEASURE))         return false;
    return true;
}

bool adxl_read_raw(adxl_raw_t *s)
{
    uint8_t  src = 0;
    uint8_t  b[6];
    uint32_t guard = READY_POLL_LIMIT;

    /* wait for the DATA_READY flag so every sample is read once */
    do {
        if (!reg_read(REG_INT_SOURCE, &src, 1)) return false;
    } while (((src & INT_DATA_READY) == 0U) && (--guard != 0U));
    if (guard == 0U) return false;

    if (!reg_read(REG_DATAX0, b, 6)) return false;

    s->x = (int16_t)(((uint16_t)b[1] << 8) | b[0]);
    s->y = (int16_t)(((uint16_t)b[3] << 8) | b[2]);
    s->z = (int16_t)(((uint16_t)b[5] << 8) | b[4]);
    return true;
}

bool adxl_calibrate(uint16_t n)
{
    adxl_raw_t s;
    int32_t sx = 0, sy = 0, sz = 0;

    for (uint16_t i = 0; i < 10; i++) {       /* discard settling samples */
        if (!adxl_read_raw(&s)) return false;
    }
    for (uint16_t i = 0; i < n; i++) {
        if (!adxl_read_raw(&s)) return false;
        sx += s.x;
        sy += s.y;
        sz += s.z;
    }
    rest.x = (int16_t)(sx / (int32_t)n);
    rest.y = (int16_t)(sy / (int32_t)n);
    rest.z = (int16_t)(sz / (int32_t)n);
    return true;
}

void adxl_get_rest(adxl_raw_t *r)
{
    *r = rest;
}

void adxl_apply_offset(const adxl_raw_t *raw, adxl_raw_t *corr)
{
    corr->x = (int16_t)(raw->x - rest.x);
    corr->y = (int16_t)(raw->y - rest.y);
    corr->z = (int16_t)(raw->z - rest.z);
}

int32_t adxl_to_mg(int32_t counts)
{
    return (counts * 39) / 10;     /* 3.9 mg/LSB in full-resolution mode */
}
