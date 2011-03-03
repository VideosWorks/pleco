
#include "Message.h"

#include <QDebug>

#include <stdint.h>

Message::Message(QByteArray data):
  bytearray(data)
{
  qDebug() << "in" << __FUNCTION__;

  qDebug() << __FUNCTION__ << ": Created a package with type " << (quint8)bytearray.at(TYPE_OFFSET_TYPE);
}

Message::Message(quint8 type, quint8 subType):
  bytearray()
{
  qDebug() << "in" << __FUNCTION__;

  bytearray.resize(length(type));
  if (bytearray.size() < TYPE_OFFSET_PAYLOAD) {
	return; // Invalid message, must have space at least for mandatory
			// data before payload
  }

  bytearray.fill(0); // Zero data

  bytearray[TYPE_OFFSET_TYPE] = type;
  bytearray[TYPE_OFFSET_SUBTYPE] = subType;
  
  setCRC();

  qDebug() << __FUNCTION__ << ": Created a package with type" << (quint8)bytearray.at(TYPE_OFFSET_TYPE)
		   << ", sub type" << (quint8)bytearray.at(TYPE_OFFSET_SUBTYPE);
}



Message::~Message()
{
  qDebug() << "in" << __FUNCTION__;
}



void Message::setACK(Message &msg)
{

  quint8 type = msg.type();
  quint8 subType = msg.subType();

  // Make sure the size matches ACK message
  bytearray.resize(length(MSG_TYPE_ACK));

  bytearray.fill(0); // Zero data

  bytearray[TYPE_OFFSET_TYPE] = MSG_TYPE_ACK;

  // Set the type and possible subtype we are acking
  bytearray[TYPE_OFFSET_PAYLOAD + 0] = type;
  bytearray[TYPE_OFFSET_PAYLOAD + 1] = subType;

  // Copy the CRC of the message we are acking
  QByteArray *data = msg.data();
  bytearray[TYPE_OFFSET_PAYLOAD + 2] = (*data)[TYPE_OFFSET_CRC + 0];
  bytearray[TYPE_OFFSET_PAYLOAD + 3] = (*data)[TYPE_OFFSET_CRC + 1];

  setCRC(); 
}



quint8 Message::getAckedType(void)
{

  // Return none as the type if the packet is not ACK packet
  if (type() != MSG_TYPE_ACK) {
	return MSG_TYPE_NONE;
  }

  // Return none as the type if the packet is too short
  if (bytearray.size() < length(MSG_TYPE_ACK)) {
	return MSG_TYPE_NONE;
  }

  // return the acked type
  return bytearray.at(TYPE_OFFSET_PAYLOAD);
}



quint8 Message::getAckedSubType(void)
{

  // Return none as the type if the packet is not ACK packet
  if (type() != MSG_TYPE_ACK) {
	return MSG_TYPE_NONE;
  }

  // Return none as the type if the packet is too short
  if (bytearray.size() < length(MSG_TYPE_ACK)) {
	return MSG_TYPE_NONE;
  }

  // Return the acked sub type
  return bytearray.at(TYPE_OFFSET_PAYLOAD + 1);
}



quint16 Message::getAckedFullType(void)
{
  return (((quint16)getAckedType()) << 8) + getAckedSubType();
}



bool Message::isValid(void)
{

  // Size must be at least big enough to hold mandatory headers before payload
  if (bytearray.size() < TYPE_OFFSET_PAYLOAD) {
	qWarning() << "Invalid message length:" << bytearray.size() << ", discarding";
	return false;
  }

  // Size must be at least the minimum size for the type
  if (bytearray.size() < length(type())) {
	qWarning() << "Invalid message length (" << bytearray.size() << ") for type" << type() <<  ", discarding";
	return false;
  }

  // CRC inside the package must match the calculated CRC
  return validateCRC();
}



bool Message::isHighPriority(void)
{
  // Package is considered a high priority, if the type < MSG_HP_TYPE_LIMIT
  return type() < MSG_HP_TYPE_LIMIT;
}



quint8 Message::type(void)
{
  qDebug() << "in" << __FUNCTION__;

  return (quint8)bytearray.at(TYPE_OFFSET_TYPE);
}



quint8 Message::subType(void)
{
  qDebug() << "in" << __FUNCTION__;

  return (quint8)bytearray.at(TYPE_OFFSET_SUBTYPE);
}



quint16 Message::fullType(void)
{
  qDebug() << "in" << __FUNCTION__;

  return (((quint16)bytearray.at(TYPE_OFFSET_TYPE)) << 8) + 
	(quint8)bytearray.at(TYPE_OFFSET_SUBTYPE);
}



int Message::length(void)
{
  qDebug() << "in" << __FUNCTION__;

  return length(type());
}



int Message::length(quint8 type)
{
  qDebug() << "in" << __FUNCTION__;

  switch(type) {
  case MSG_TYPE_PING:
	return TYPE_OFFSET_PAYLOAD + 0; // no payload
  case MSG_TYPE_C_A_S:
	return TYPE_OFFSET_PAYLOAD + 4; // + CAMERA X + Y + MOTOR RIGHT + LEFT
  case MSG_TYPE_MEDIA:
	return TYPE_OFFSET_PAYLOAD + 0; // + payload of arbitrary lenght
  case MSG_TYPE_VALUE:
	return TYPE_OFFSET_PAYLOAD + 2; // + 16 bit value
  case MSG_TYPE_STATS:
	return TYPE_OFFSET_PAYLOAD + 3; // + UPTIME + LOAD AVG + WLAN
  case MSG_TYPE_ACK:
	return TYPE_OFFSET_PAYLOAD + 4; // + type + sub type + 16 bit CRC
  default:
	qWarning() << "Message length for type" << type << "not known";
	return 0;
  }
}



QByteArray *Message::data(void)
{
  qDebug() << "in" << __FUNCTION__;
  return &bytearray;
}



void Message::setCRC(void)
{
  // Zero CRC field in data before calculating new 16bit CRC
  *((quint16 *)(bytearray.data())) = 0;

  // Calculate 16bit CRC
  quint16 crc = qChecksum(bytearray.data(), bytearray.size());

  // Set 16bit CRC
  *((quint16 *)(bytearray.data())) = crc;
}



quint16 Message::getCRC(void)
{
  return *((quint16 *)(bytearray.data()));
}



bool Message::validateCRC(void)
{

  quint16 crc = getCRC();

  // FIXME: would the CRC be unchanged if we start from index 2
  // compared to case where we start from index 0 with two zeros in
  // front?

  // Zero CRC field in data before calculating new CRC
  *((quint16 *)(bytearray.data())) = 0;

  // Calculate CRC
  quint16 calculated = qChecksum(bytearray.data(), bytearray.size());

  // Set old CRC back
  *((quint16 *)(bytearray.data())) = crc;

  bool isValid = (crc == calculated);

  return isValid;
}



void Message::setPayload16(quint16 value)
{
  setQuint16(TYPE_OFFSET_PAYLOAD, value);
}



void Message::setQuint16(int index, quint16 value)
{
  // FIXME: index must be divisable by 2 for proper aligment

  char *p = bytearray.data();
  
  quint16 *p16 = (quint16 *)(&p[index]);

  p[index] = value;
}



