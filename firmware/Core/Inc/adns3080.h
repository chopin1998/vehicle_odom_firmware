#ifndef _ADNS3080
#define _ADNS3080 1

#define A_PID                    0x00
#define A_VID                    0x01
#define A_MOTION                 0x02
#define A_DX                     0x03
#define A_DY                     0x04
#define A_SQUAL                  0x05
#define A_PIXEL_SUM              0x06
#define A_MAX_PIXEL              0x07
#define A_CONF                   0x0a
#define A_EXT_CONF               0x0b
#define A_DATA_OUT_LOWER         0x0c
#define A_DATA_OUT_UPPER         0x0d
#define A_SHUTTER_LOWER          0x0e
#define A_SHUTTER_UPPER          0x0f
#define A_FRAME_PERIOD_LOWER     0x10
#define A_FRAME_PERIOD_UPPER     0x11
#define A_MOTION_CLEAR           0x12
#define A_FRAME_CAPTURE          0x13
#define A_SROM_ENABLE            0x14
#define A_FRAME_PERIOD_MAX_BOUND_LOWER 0x19
#define A_FRAME_PERIOD_MAX_BOUND_UPPER 0x1a
#define A_FRAME_PERIOD_MIN_BOUND_LOWER 0x1b
#define A_FRAME_PERIOD_MIN_BOUND_UPPER 0x1c
#define A_SHUTTER_MAX_BOUND_LOWER 0x1d
#define A_SHUTTER_MAX_BOUND_UPPER 0x1e
#define A_OBSERVATION            0x3d
#define A_PIV_INVERSE            0x3f
#define A_PIXEL_BURST            0x40
#define A_MOTION_BURST           0x50
#define A_SROM_LOAD              0x60


typedef struct {
    uint8_t motion;
    int8_t dx;
    int8_t dy;
    int8_t squal;
    int8_t shutter_upper;
    int8_t shutter_lower;
    int8_t max_pixel;

} motion_t;

void adns3080_init(void);
unsigned char adns3080_read_reg(unsigned char addr);
void adns3080_write_reg(unsigned char addr, unsigned char val);
void ands3080_reset(void);


void adns3080_motion(motion_t *motion);

void adns3080_capture(void);

#endif
