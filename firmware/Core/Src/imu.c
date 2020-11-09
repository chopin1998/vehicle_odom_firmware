#include "imu.h"
#include "usbd_cdc_if.h"

stmdev_ctx_t dev_imu, dev_mag;

int32_t _write_imu_reg(void *h, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    HAL_StatusTypeDef rev;

    rev = HAL_I2C_Mem_Write(&hi2c1, LSM9DS1_IMU_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);

    return rev;
}

int32_t _read_imu_reg(void *h, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    HAL_StatusTypeDef rev;

    rev = HAL_I2C_Mem_Read(&hi2c1, LSM9DS1_IMU_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);

    return rev;
}

int32_t _write_mag_reg(void *h, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    HAL_StatusTypeDef rev;

    rev = HAL_I2C_Mem_Write(&hi2c1, LSM9DS1_MAG_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);

    return rev;
}

int32_t _read_mag_reg(void *h, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    HAL_StatusTypeDef rev;

    rev = HAL_I2C_Mem_Read(&hi2c1, LSM9DS1_MAG_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);

    return rev;
}

uint32_t imu_init(void)
{
    lsm9ds1_id_t whoami;
    uint32_t rev = 0;

    // if (0)
    // {
    //     MotionFX_initialize();

    //     MFX_knobs_t knobs;
    //     MotionFX_getKnobs(&knobs);
    //     // tunning parameters
    //     knobs.LMode = 1;
    //     knobs.modx = 1;
    //     knobs.output_type = MFX_ENGINE_OUTPUT_ENU;
    //     // tunning parameters
    //     MotionFX_setKnobs(&knobs);

    //     MotionFX_enable_6X(MFX_ENGINE_DISABLE);
    //     MotionFX_enable_9X(MFX_ENGINE_ENABLE);
    // }

    dev_imu.handle = NULL;
    dev_imu.read_reg = _read_imu_reg;
    dev_imu.write_reg = _write_imu_reg;

    dev_mag.handle = NULL;
    dev_mag.read_reg = _read_mag_reg;
    dev_mag.write_reg = _write_mag_reg;

    // check dev id
    lsm9ds1_dev_id_get(&dev_mag, &dev_imu, &whoami);
    if (whoami.imu != LSM9DS1_IMU_ID || whoami.mag != LSM9DS1_MAG_ID)
    {
        return -1;
    }

    // restore default conf
    uint8_t rst;
    lsm9ds1_dev_reset_set(&dev_mag, &dev_imu, PROPERTY_ENABLE);
    do
    {
        lsm9ds1_dev_reset_get(&dev_mag, &dev_imu, &rst);
    } while (rst);

    // enable block data
    lsm9ds1_block_data_update_set(&dev_mag, &dev_imu, PROPERTY_ENABLE);

    // set full scale
    lsm9ds1_xl_full_scale_set(&dev_imu, LSM9DS1_4g);
    lsm9ds1_gy_full_scale_set(&dev_imu, LSM9DS1_2000dps);
    lsm9ds1_mag_full_scale_set(&dev_mag, LSM9DS1_16Ga);

    // config filter for accel
    lsm9ds1_xl_filter_aalias_bandwidth_set(&dev_imu, LSM9DS1_AUTO);
    // lsm9ds1_xl_filter_lp_bandwidth_set(&dev_imu, LSM9DS1_LP_ODR_DIV_50);
    lsm9ds1_xl_filter_lp_bandwidth_set(&dev_imu, LSM9DS1_LP_ODR_DIV_100);
    lsm9ds1_xl_filter_out_path_set(&dev_imu, LSM9DS1_LP_OUT);

    // config filter for gyro
    lsm9ds1_gy_filter_lp_bandwidth_set(&dev_imu, LSM9DS1_LP_ULTRA_LIGHT);
    lsm9ds1_gy_filter_hp_bandwidth_set(&dev_imu, LSM9DS1_HP_MEDIUM);
    lsm9ds1_gy_filter_out_path_set(&dev_imu, LSM9DS1_LPF1_HPF_LPF2_OUT);

    // set Output Data Rate/ power mode
    // lsm9ds1_imu_data_rate_set(&dev_imu, LSM9DS1_IMU_59Hz5);
    lsm9ds1_imu_data_rate_set(&dev_imu, LSM9DS1_IMU_119Hz);
    // lsm9ds1_mag_data_rate_set(&dev_mag, LSM9DS1_MAG_UHP_10Hz);
    lsm9ds1_mag_data_rate_set(&dev_mag, LSM9DS1_MAG_UHP_40Hz);

    lsm9ds1_pin_int1_route_t int1_reg;
    int1_reg.int1_ig_xl = 1;
    int1_reg.int1_drdy_xl = 1;
    lsm9ds1_pin_int1_route_set(&dev_imu, int1_reg);

    // lsm9ds1_pin_int2_route_t int2_reg;
    // int2_reg.int2_drdy_g = 1;
    // lsm9ds1_pin_int2_route_set(&dev_imu, int2_reg);
}

void imu_xl_gy_drdy(void)
{
    imu_xl_gy_drdy_flag = 1;
}

void imu_mag_drdy(void)
{
    imu_mag_drdy_flag = 1;
}

uint8_t is_imu_xl_gy_drdy(void)
{
    if (imu_xl_gy_drdy_flag)
    {
        imu_xl_gy_drdy_flag = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t is_imu_mag_drdy(void)
{
    if (imu_mag_drdy_flag)
    {
        imu_mag_drdy_flag = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

void imu_dump(void)
{
    lsm9ds1_status_t status;

    axis3bit16_t data_raw_acceleration;
    axis3bit16_t data_raw_angular_rate;
    axis3bit16_t data_raw_magnetic_field;

    float accel_mg[3], gyro_rate_mdps[3], mag_mgauss[3];
    uint8_t buf[256];
    uint16_t buf_len;

    lsm9ds1_dev_status_get(&dev_mag, &dev_imu, &status);

    if (status.status_imu.xlda && status.status_imu.gda)
    {
        memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
        memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));

        lsm9ds1_acceleration_raw_get(&dev_imu, data_raw_acceleration.u8bit);
        lsm9ds1_angular_rate_raw_get(&dev_imu, data_raw_angular_rate.u8bit);

        accel_mg[0] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[0]);
        accel_mg[1] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[1]);
        accel_mg[2] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[2]);

        gyro_rate_mdps[0] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[0]);
        gyro_rate_mdps[1] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[1]);
        gyro_rate_mdps[2] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[2]);

        buf_len = snprintf(buf, 256, "[accel]%4.2f\t%4.2f\t%4.2f | [gyro]%4.2f\t%4.2f\t%4.2f\r\n", accel_mg[0], accel_mg[1], accel_mg[2], gyro_rate_mdps[0], gyro_rate_mdps[1], gyro_rate_mdps[2]);
        CDC_Transmit_FS(buf, buf_len);
    }

    if (status.status_mag.zyxda)
    {
        memset(data_raw_magnetic_field.u8bit, 0x00, 3 * sizeof(int16_t));

        lsm9ds1_magnetic_raw_get(&dev_mag, data_raw_magnetic_field.u8bit);

        mag_mgauss[0] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[0]);
        mag_mgauss[1] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[1]);
        mag_mgauss[2] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[2]);

        buf_len = snprintf(buf, 256, "[mag]%4.2f\t%4.2f\t%4.2f\r\n", mag_mgauss[0], mag_mgauss[1], mag_mgauss[2]);
        CDC_Transmit_FS(buf, buf_len);
    }
}

void imu_getdata(MFX_input_t *input)
{
    lsm9ds1_status_t sreg;

    axis3bit16_t data_raw_acceleration;
    axis3bit16_t data_raw_angular_rate;
    axis3bit16_t data_raw_magnetic_field;

    // float accel_mg[3], gyro_rate_mdps[3], mag_mgauss[3];

    // MFX_input_t input;
    // MFX_output_t output;

    // lsm9ds1_dev_status_get(&dev_mag, &dev_imu, &sreg);

    // if (sreg.status_imu.xlda && sreg.status_imu.gda)
    {
        // memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
        // memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));

        lsm9ds1_acceleration_raw_get(&dev_imu, data_raw_acceleration.u8bit);
        lsm9ds1_angular_rate_raw_get(&dev_imu, data_raw_angular_rate.u8bit);

        input->acc[0] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[0]);
        input->acc[1] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[1]);
        input->acc[2] = lsm9ds1_from_fs4g_to_mg(data_raw_acceleration.i16bit[2]);

        input->gyro[0] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[0]);
        input->gyro[1] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[1]);
        input->gyro[2] = lsm9ds1_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[2]);
    }

    // if (sreg.status_mag.zyxda)
    {
        // memset(data_raw_magnetic_field.u8bit, 0x00, 3 * sizeof(int16_t));

        lsm9ds1_magnetic_raw_get(&dev_mag, data_raw_magnetic_field.u8bit);

        input->mag[0] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[0]);
        input->mag[1] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[1]);
        input->mag[2] = lsm9ds1_from_fs16gauss_to_mG(data_raw_magnetic_field.i16bit[2]);
    }
}
