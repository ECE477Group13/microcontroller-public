
#ifndef _LSM6DS_H
#define _LSM6DS_H

#include <stdio.h>

typedef uint8_t axis_t;
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

#define LSM6DS_I2CADDR_DEFAULT 0x6A 

#define LSM6DS_FUNC_CFG_ACCESS 0x1 
#define LSM6DS_INT1_CTRL 0x0D      
#define LSM6DS_INT2_CTRL 0x0E      
#define LSM6DS_WHOAMI 0x0F         
#define LSM6DS_CTRL1_XL 0x10       
#define LSM6DS_CTRL2_G 0x11        
#define LSM6DS_CTRL3_C 0x12    
#define LSM6DS_CTRL4_C 0x13    
#define LSM6DS_CTRL8_XL 0x17       
#define LSM6DS_CTRL10_C 0x19       
#define LSM6DS_WAKEUP_SRC 0x1B     
#define LSM6DS_STATUS_REG 0X1E     
#define LSM6DS_OUT_TEMP_L 0x20     
#define LSM6DS_OUTX_L_G 0x22       
#define LSM6DS_OUTX_L_A 0x28       
#define LSM6DS_OUTX_H_A 0x29       
#define LSM6DS_OUTY_L_A 0x2A       
#define LSM6DS_OUTY_H_A 0x2B       
#define LSM6DS_OUTZ_L_A 0x2C       
#define LSM6DS_OUTZ_H_A 0x2D       
#define LSM6DS_STEPCOUNTER 0x4B    
#define LSM6DS_TAP_CFG 0x58        
#define LSM6DS_WAKEUP_THS 0x5B 
#define LSM6DS_WAKEUP_DUR 0x5C 
#define LSM6DS_MD1_CFG 0x5E
#define LSM6DS_MD2_CFG 0x5F


#define LSM6DS_WAKE_UP_DUR 0x5C
#define LSM6DS_WAKE_UP_THS 0x5B
#define LSM6DS_TAP_CFG0 0x56
#define LSM6DS_TAP_CFG2 0x58

#define LSM6DS_EMB_FUNC_EN_A 0x04
#define LSM6DS_EMB_FUNC_INT1 0x0A
#define LSM6DS_EMB_FUNC_INT2 0x0E
#define LSM6DS_PAGE_RW 0x17
#define LSM6DS_EMB_FUNC_INIT_A 0x66


// SAD Addresses
#define LSM6DS_SAD0            0b1101010 // SA0 -> ground
#define LSM6DS_SAD1            0b1101011 // SA0 -> power

// register values
#define I2C_disable 0x04
#define LSM6DS_WHO 0x6C


typedef enum data_rate {
  LSM6DS_RATE_SHUTDOWN,
  LSM6DS_RATE_12_5_HZ,
  LSM6DS_RATE_26_HZ,
  LSM6DS_RATE_52_HZ,
  LSM6DS_RATE_104_HZ,
  LSM6DS_RATE_208_HZ,
  LSM6DS_RATE_416_HZ,
  LSM6DS_RATE_833_HZ,
  LSM6DS_RATE_1_66K_HZ,
  LSM6DS_RATE_3_33K_HZ,
  LSM6DS_RATE_6_66K_HZ,
} lsm6ds_data_rate_t;

typedef enum accel_range {
  LSM6DS_ACCEL_RANGE_2_G,
  LSM6DS_ACCEL_RANGE_16_G,
  LSM6DS_ACCEL_RANGE_4_G,
  LSM6DS_ACCEL_RANGE_8_G
} lsm6ds_accel_range_t;

typedef enum gyro_range {
  LSM6DS_GYRO_RANGE_125_DPS = 0b0010,
  LSM6DS_GYRO_RANGE_250_DPS = 0b0000,
  LSM6DS_GYRO_RANGE_500_DPS = 0b0100,
  LSM6DS_GYRO_RANGE_1000_DPS = 0b1000,
  LSM6DS_GYRO_RANGE_2000_DPS = 0b1100,
  ISM330DHCX_GYRO_RANGE_4000_DPS = 0b0001
} lsm6ds_gyro_range_t;

typedef enum hpf_range {
  LSM6DS_HPF_ODR_DIV_50 = 0,
  LSM6DS_HPF_ODR_DIV_100 = 1,
  LSM6DS_HPF_ODR_DIV_9 = 2,
  LSM6DS_HPF_ODR_DIV_400 = 3,
} lsm6ds_hp_filter_t;

typedef struct Acc{
  int16_t rawAccX;
  int16_t rawAccY;
  int16_t rawAccZ;
} Acc;      

#endif