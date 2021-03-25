#include "sbs.h"
#include "Arduino.h"

#define tim 500
//int Addr = 0;

static const uint8_t SBS::scan(){
	Wire.begin();
	byte count = 0;
	for (byte i = 1; i < 120; i++){
		Wire.beginTransmission (i);
		if (Wire.endTransmission () == 0){
			count++;
			delay (1);
			return i;
		}
	}
	return 0;
}

/*int SBS::type(){
	if(scan == 11){
		Addr = 11;
	}else if(scan == 52){res = 2;}
	return res;
}*/
	

void SBS::write(uint8_t address, uint8_t data)
{
	Wire.beginTransmission(Addr);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission(true);
}

uint8_t SBS::read(uint8_t address)
{
	uint8_t registerValue;
    Wire.beginTransmission(Addr);
 	Wire.write(address);
  	Wire.endTransmission();
  	Wire.requestFrom(Addr,1,true);
  	registerValue = Wire.read();
	return registerValue;
}

uint16_t SBS::read16u(uint8_t address)
{
  	uint16_t registerValue;
    Wire.beginTransmission(Addr);
   	Wire.write(address);
   	Wire.endTransmission(false);
   	Wire.requestFrom(Addr,2,true);
   	registerValue = Wire.read();
    registerValue |= (Wire.read()<<8); 
	//Wire.endTransmission(true);
    return registerValue;
}

int16_t SBS::read16(uint8_t address)
{
	int16_t registerValue;
    Wire.beginTransmission(Addr);
    Wire.write(address);
  	Wire.endTransmission(false);
  	Wire.requestFrom(Addr,2,true);
  	registerValue = Wire.read();
    registerValue += (Wire.read()*256);
	return registerValue;
}
    
uint16_t SBS::read16u2(uint8_t address)
{
  	uint16_t registerValue;
    Wire.beginTransmission(Addr);
   	Wire.write(address);
   	Wire.endTransmission(false);
   	Wire.requestFrom(Addr,4,true);
   	Wire.read();
    registerValue = Wire.read();
    registerValue |= (Wire.read()<<8);
    return registerValue;
}    
    
/*uint32_t SBS::read32u(uint8_t address)
{
  	uint32_t registerValue;
    Wire.beginTransmission(Addr);
  	Wire.write(address);
  	Wire.endTransmission(false);
  	Wire.requestFrom(Addr,5,true);
    registerValue = Wire.read();
    registerValue |= (Wire.read()<<8);
    registerValue |= (Wire.read() << 16);
    registerValue |= (Wire.read() << 24);		
    return registerValue;
}*/

bool SBS::writeWord()
{
	Wire.beginTransmission(Addr);
	Wire.write(0x77);
	Wire.write(lowByte(0x0040));
	Wire.write(highByte(0x0040));
	//Wire.write(0x40&0xFF);
	//Wire.write((0x40>>8)&0xFF);
	//Wire.write(0x39);
	if(Wire.endTransmission()==0){
		return true;
	}else{
		return false;
	}
}

uint8_t SBS::readCellsNum()
{
	if(writeWord()){
		uint8_t res = 0;
		uint8_t data_page1[32];
		Wire.beginTransmission(Addr);
		Wire.write(0x78);
		Wire.endTransmission(false);
		Wire.requestFrom(Addr, 32);
		if(Wire.available()<=32){
			for(int i=0; i<32; i++){
				data_page1[i] = Wire.read();      
			}
		}
		//res = (((res ^ data_page1[1])<<8)^data_page1[2]);
		//Serial.println(data_page1[2],HEX);
		res = data_page1[2]<<6;
		//Serial.println(res);
		Wire.endTransmission(true);
		if(res == 64){
			return 2;
		}else if(res == 128){
			return 3;
		}else if(res == 192){
			return 4;
		}
	}else{
		return 0;
	}
}

void SBS::Seal()
{
	Wire.beginTransmission(Addr);
    Wire.write(0x00);
    Wire.write(0x20);
	Wire.write(0x00);
    Wire.endTransmission(true);
}

void SBS::Unseal()
{
	Wire.beginTransmission(Addr);
    Wire.write(0x00);
    Wire.write(lowByte(UNSEAL_KEY1));
	Wire.write(highByte(UNSEAL_KEY1));
    Wire.endTransmission(true);
	Wire.beginTransmission(Addr);
    Wire.write(0x00);
    Wire.write(lowByte(UNSEAL_KEY2));
	Wire.write(highByte(UNSEAL_KEY2));
    Wire.endTransmission(true);
}

void SBS::FullAccess()
{
	Wire.beginTransmission(Addr);
    Wire.write(0x00);
    Wire.write(lowByte(FA_KEY1));
	Wire.write(highByte(FA_KEY1));
    Wire.endTransmission(true);
	Wire.beginTransmission(Addr);
    Wire.write(0x00);
    Wire.write(lowByte(FA_KEY2));
	Wire.write(highByte(FA_KEY2));
    Wire.endTransmission(true);
}

uint16_t SBS::OpStatus()
{
	return read16u(REG_STATUS);
}

void SBS::SetSerial(uint16_t serial)
{
	Wire.beginTransmission(Addr);
	Wire.write(REG_SERIAL);
	Wire.write(lowByte(serial));
	Wire.write(highByte(serial));
	Wire.endTransmission(true);
	delay(45);
}

void SBS::SetDate(uint16_t date)
{
	Wire.beginTransmission(Addr);
	Wire.write(REG_DATE);
	Wire.write(lowByte(date));
	Wire.write(highByte(date));
	Wire.endTransmission(true);
	delay(45);
}

uint16_t SBS::GetVol()
{
	uint16_t voltage;
	voltage = read16u(REG_VOLTAGE);
	if((voltage==0)||(voltage==65535)){
		delay(tim);
		voltage = read16u(REG_VOLTAGE);
	}
	return voltage;
}

uint16_t SBS::FullChargeCap()
{
	uint16_t capacity;
	capacity = read16u(REG_FullChrgCap);
	if((capacity==0)||(capacity==65535)){
		delay(tim);
		capacity = read16u(REG_FullChrgCap);
	}
	return capacity;
}

int16_t SBS::GetCur()
{
	uint16_t current;
	current = read16u(REG_CURRENT);
	if(current==65535){
		delay(tim);
		current = read16u(REG_CURRENT);
	}
	return current;
}

float SBS::GetTemp()
{
	float temperature;
	temperature = read16u(REG_TEMPERATURE);
	if((temperature==0.0)||(temperature==6553.5)||(temperature==543.06))
	{
		delay(tim);
		temperature = read16u(REG_TEMPERATURE);
	}
	temperature = (temperature/10)-272.13;
	return temperature;
}

uint8_t SBS::GetCap()
{
	uint8_t capacity;
	capacity = read(REG_CAPACITY);
	if(capacity>=255){
		delay(tim);
		capacity = read(REG_CAPACITY);
	}
	return capacity;
}

uint16_t SBS::GetSerial()
{
	return read16u(REG_SERIAL);
}

uint16_t SBS::GetDate()
{
	return read16u(REG_DATE);
}

uint16_t SBS::GetC1()
{
	uint16_t c1_voltage;
	c1_voltage = read16u(C1_V);
	if((c1_voltage == 65535)||(c1_voltage==0)){
		delay(tim);
		c1_voltage = read16u(C1_V);
	}
	return c1_voltage;
}

uint16_t SBS::GetC2()
{
	uint16_t c2_voltage;
	c2_voltage = read16u(C2_V);
	if((c2_voltage == 65535)||(c2_voltage==0)){
		delay(tim);
		c2_voltage = read16u(C2_V);
	}
	return c2_voltage;
}

uint16_t SBS::GetC3()
{
	uint16_t c3_voltage;
	c3_voltage = read16u(C3_V);
	if((c3_voltage == 65535)||(c3_voltage==0)){
		delay(tim);
		c3_voltage = read16u(C3_V);
	}
	return c3_voltage;
}

uint16_t SBS::GetC4()
{
	uint16_t c4_voltage;
	c4_voltage = read16u(C4_V);
	if((c4_voltage == 65535)||(c4_voltage==0)){
		delay(tim);
		c4_voltage = read16u(C4_V);
	}
	return c4_voltage;
}

String SBS::GetData()
{	
	String comm = ",";
	String data = "";
	uint16_t voltage,c1v,c2v,c3v,c4v,ser,fCap;
	float temperature;
	int16_t current;
	uint8_t capacity;
	uint16_t stat;
	uint16_t date;
	uint8_t  num = readCellsNum();
	
	voltage = GetVol();
	if(GetCur() != voltage){
		current = GetCur();
	}else{
		delay(45);
		current = GetCur();
	}
	capacity = GetCap();
	fCap = FullChargeCap();
	temperature = GetTemp();
	ser = GetSerial();
	stat = OpStatus();
	date = GetDate();
	if(num == 2){
		c1v = GetC1();
		c2v = GetC2();
		c3v = 0;
		c4v = 0;
	}else if(num ==3){
		c1v = GetC1();
		c2v = GetC2();
		c3v = GetC3();
		c4v = 0;
	}else if(num ==4){
		c1v = GetC1();
		c2v = GetC2();
		c3v = GetC3();
		c4v = GetC4();	
	}
	{
		data = voltage+comm+current+comm+temperature+comm+capacity+comm+c1v+comm+c2v+comm+c3v+comm+c4v+comm+ser+comm+stat+comm+date+comm+fCap;
	}
	return data;
}


/*static void crc(int16_t data)
{
	uint8_t pec = 0;
	unsigned char i;    // Counter for 8 shifts
	pec ^= data;        // Initial XOR
	i = 8;
	do{
		if (pec & 0x80){
		pec <<= 1;
		pec ^= 0x107;
		}else{
		pec <<= 1;
		}
	}
	while (--i);
	Serial.println(pec,HEX);
}*/