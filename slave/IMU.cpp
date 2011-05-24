#include "IMU.h"

#include <QObject>
#include <QDebug>

IMU::IMU(Hardware *hardware):
  QObject()
{

}



IMU::~IMU()
{ 

}



bool IMU::enable(bool enable)
{

  qDebug() << "In" << __FUNCTION__ << ", Enable:" << enable;

  return true;
}



QByteArray *IMU::get9DoF(int accuracy_bytes)
{
  qDebug() << "In" << __FUNCTION__ << ", Accuracy in bytes:" << accuracy_bytes;

  static quint8 imu[9] = {0, 30, 60, 90, 120, 150, 180, 210, 240};

  for (int i = 0; i < 9; i++) {
	imu[i]++;
  }

  QByteArray *data = new QByteArray(imu, 9);

  return data;
}




QByteArray *IMU::get9DoFRaw(int accuracy_bytes)
{
  qDebug() << "In" << __FUNCTION__ << ", Accuracy in bytes:" << accuracy_bytes;

  static quint8 imu[9] = {0, 30, 60, 90, 120, 150, 180, 210, 240};

  for (int i = 0; i < 9; i++) {
	imu[i] += randInt(-10,10);
  }

  QByteArray *data = new QByteArray(imu, 9);

  return data;
}