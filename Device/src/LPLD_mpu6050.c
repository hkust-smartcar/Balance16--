#include "common.h"
#include "mpu6050.h"

I2C_InitTypeDef MPU6050_init_struct;

/*
 *   MPU6050_WriteReg
 *   MPU6050写寄存器状态
 *
 *   参数：
 *    RegisterAddress 寄存器地址
 *    Data 所需要写得内容
 *
 *   返回值
 *    无
 */
void MPU6050_WriteReg(uint8_t RegisterAddress, uint8_t Data)
{
  //发送从机地址
  I2C_StartTrans(MPU6050_I2CX, SlaveAddress, I2C_MWSR);
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_ON);
  
  //写MPU6050寄存器地址
  I2C_WriteByte(MPU6050_I2CX, RegisterAddress);
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_ON);
  
  //向寄存器中写具体数据
  I2C_WriteByte(MPU6050_I2CX, Data);
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_ON);

  I2C_Stop(MPU6050_I2CX);
}

/*
 *   MPU6050_ReadReg
 *   MPU6050读寄存器状态
 *
 *   参数：
 *    RegisterAddress 寄存器地址
 *
 *   返回值
 *    所读寄存器状态
 */
uint8_t MPU6050_ReadReg(uint8_t RegisterAddress)
{
  uint8_t result;

  //发送从机地址
  I2C_StartTrans(MPU6050_I2CX, SlaveAddress, I2C_MWSR);
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_ON);
  // I2C_WaitAck(MPU6050_I2CX, I2C_ACK_OFF);

  //写MPU6050寄存器地址
  I2C_WriteByte(MPU6050_I2CX, RegisterAddress);
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_ON);
  // I2C_WaitAck(MPU6050_I2CX, I2C_ACK_OFF);
  
  //再次产生开始信号
  I2C_ReStart(MPU6050_I2CX);

  //发送从机地址和读取位
  I2C_WriteByte(MPU6050_I2CX, (SlaveAddress<<1)|I2C_MRSW);
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_ON);
  // I2C_WaitAck(MPU6050_I2CX, I2C_ACK_OFF);

  //转换主机模式为读
  I2C_SetMasterWR(MPU6050_I2CX, I2C_MRSW);

  //关闭应答ACK
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_OFF);//关闭ACK

  //读IIC数据
  result =I2C_ReadByte(MPU6050_I2CX);
  I2C_WaitAck(MPU6050_I2CX, I2C_ACK_ON);
  // I2C_WaitAck(MPU6050_I2CX, I2C_ACK_OFF);

  //发送停止信号
  I2C_Stop(MPU6050_I2CX);

  //读IIC数据
  result = I2C_ReadByte(MPU6050_I2CX);
  
  // DelayMs(1);

  return result;
}

/*
 *   MPU6050_GetResult
 *   获得MPU6050结果
 *
 *   参数：
 *    无
 *
 *   返回值
 *    转换结果 
 */
int16_t MPU6050_GetResult(uint8_t Regs_Addr)
{
  int16_t result,temp;
  result = MPU6050_ReadReg(Regs_Addr);
  temp   = MPU6050_ReadReg(Regs_Addr+1);
  result=result<<8;
  result=result|temp;
  return result;
}

/*
 *   MPU6050_Init
 *   初始化MPU6050，包括初始化MPU6050所需的I2C接口以及MPU6050的寄存器
 *
 *   参数：
 *    无
 *
 *   返回值
 *    无
 */
void MPU6050_Init()
{
  //初始化MPU6050
  MPU6050_init_struct.I2C_I2Cx = MPU6050_I2CX;          //在MPU6050.h中修改该值
  MPU6050_init_struct.I2C_IntEnable = true;
  MPU6050_init_struct.I2C_ICR = MPU6050_SCL_400KHZ;     //可根据实际电路更改SCL频率
  MPU6050_init_struct.I2C_SclPort = MPU6050_SCLPORT;
  MPU6050_init_struct.I2C_SclPin = MPU6050_SCLPIN;      //在MPU6050.h中修改该值
  MPU6050_init_struct.I2C_SdaPort = MPU6050_SDAPORT;
  MPU6050_init_struct.I2C_SdaPin = MPU6050_SDAPIN;      //在MPU6050.h中修改该值
  MPU6050_init_struct.I2C_Isr = NULL;
  I2C_Init(&MPU6050_init_struct);
  
  // DelayMs(1);
  
  // MPU6050_WriteReg(PWR_MGMT_1,0x00);    //解除休眠状态
  // // MPU6050_WriteReg(SMPLRT_DIV,0x07);    //陀螺仪采样率，典型值：0x07(125Hz)
  // MPU6050_WriteReg(SMPLRT_DIV,0x00);
  // // MPU6050_WriteReg(CONFIG,0x06);        //低通滤波频率，典型值：0x06(5Hz)
  // MPU6050_WriteReg(CONFIG,0x00);
  // MPU6050_WriteReg(GYRO_CONFIG,0x08);   //陀螺仪自检及测量范围，典型值：0x18(不自检，500deg/s)
  // MPU6050_WriteReg(ACCEL_CONFIG,0x09);  //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，4G，5Hz)
  MPU6050_WriteReg(PWR_MGMT_1, 0x00);
  MPU6050_WriteReg(SMPLRT_DIV, 0x0A);
  MPU6050_WriteReg(CONFIG, 0x00);
  MPU6050_WriteReg(AUX_VDDIO,0x80);
  MPU6050_WriteReg(GYRO_CONFIG, 0x08);
  MPU6050_WriteReg(ACCEL_CONFIG, 0x00);
  MPU6050_WriteReg(I2C_MST_CTRL, 0x00);
  MPU6050_WriteReg(INT_PIN_CFG, 0x02);
  
  // DelayMs(1);
}

void MPU6050_ReadAccel(int16_t * adata) {
  adata[0] = MPU6050_GetResult(ACCEL_XOUT_H);
  adata[1] = MPU6050_GetResult(ACCEL_YOUT_H);
  adata[2] = MPU6050_GetResult(ACCEL_ZOUT_H);
}

void MPU6050_ReadGyro(int16_t * gdata) {
  gdata[0] = MPU6050_GetResult(GYRO_XOUT_H);
  gdata[1] = MPU6050_GetResult(GYRO_YOUT_H);
  gdata[2] = MPU6050_GetResult(GYRO_ZOUT_H);
}
