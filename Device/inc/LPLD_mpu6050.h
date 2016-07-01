#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "board.h"
#include "common.h"
#include "i2c.h"

/********用户可修改值 开始***********/
//连接MPU6050用到的I2C通道
#ifndef MPU6050_I2CX
#define MPU6050_I2CX    (I2C0)
#endif

//连接MPU6050用到的SCL引脚
//I2C0 - PTB2、PTB0、PTD8
//I2C1 - PTC10、PTE1
#ifndef MPU6050_SCLPIN
#define MPU6050_SCLPIN  (0)
#endif
#ifndef MPU6050_SCLPORT
#define MPU6050_SCLPORT (HW_GPIOB)
#endif

//连接MPU6050用到的SDA引脚
//I2C0 - PTB3、PTB1、PTD9
//I2C1 - PTC11、PTE0
#ifndef MPU6050_SDAPIN
#define MPU6050_SDAPIN  (1)
#endif
#ifndef MPU6050_SDAPORT
#define MPU6050_SDAPORT (HW_GPIOB)
#endif
/********用户可修改值 结束***********/

//MPU6050内部寄存器
#define AUX_VDDIO               (0x01)
#define	SMPLRT_DIV				(0x19)	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG					(0x1A)	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG				(0x1B)	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	        (0x1C)	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define I2C_MST_CTRL            (0x24)
#define INT_PIN_CFG             (0x37)
#define	ACCEL_XOUT_H	        (0x3B)  //X轴加速度  加速度
#define	ACCEL_XOUT_L	        (0x3C)
#define	ACCEL_YOUT_H	        (0x3D)  //Y轴加速度
#define	ACCEL_YOUT_L	        (0x3E)
#define	ACCEL_ZOUT_H	        (0x3F)  //Z轴加速度
#define	ACCEL_ZOUT_L	        (0x40)
#define	TEMP_OUT_H				(0x41)
#define	TEMP_OUT_L				(0x42)
#define	GYRO_XOUT_H				(0x43)  //X轴角速度  陀螺仪
#define	GYRO_XOUT_L				(0x44)	
#define	GYRO_YOUT_H				(0x45)  //Y轴角速度
#define	GYRO_YOUT_L				(0x46)
#define	GYRO_ZOUT_H				(0x47)  //Z轴角速度
#define	GYRO_ZOUT_L				(0x48)
#define	PWR_MGMT_1				(0x6B)	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I				(0x75)	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	        (0x68)	//硬件I2C地址0x68 软件I2C地址0xD0

//定义SCL Bus Speed取值，外设总线为50Mhz时的计算结果
#define MPU6050_SCL_50KHZ       (0x33)
#define MPU6050_SCL_100KHZ      (0x2B)
#define MPU6050_SCL_150KHZ      (0x28)
#define MPU6050_SCL_200KHZ      (0x23)
#define MPU6050_SCL_250KHZ      (0x21)
#define MPU6050_SCL_300KHZ      (0x20)
#define MPU6050_SCL_400KHZ      (0x17)

enum accel_scale
{
    AFS_2G,
    AFS_4G,
    AFS_8G,
    AFS_16G
};

enum gyro_scale
{
    GFS_250DPS,
    GFS_500DPS,
    GFS_1000DPS,
    GFS_2000DPS
};

struct mpu_config
{
    enum accel_scale        afs;
    enum gyro_scale         gfs;
    bool                    aenable_self_test;
    bool                    genable_self_test;
};

//函数声明
void  MPU6050_WriteReg(uint8_t RegisterAddress, uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegisterAddress);
int16_t MPU6050_GetResult(uint8_t Regs_Addr);
void  MPU6050_Init(void);
void MPU6050_ReadAccel(int16_t* adata);
void MPU6050_ReadGyro(int16_t* gdata);

#endif
