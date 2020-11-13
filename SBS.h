#ifndef SBS_h
#define SBS_h

#include <Wire.h>

#define Addr 			0x0B
#define REG_CAPACITY 	0x0E
#define REG_VOLTAGE 	0x09
#define REG_CURRENT 	0x0A
#define REG_TEMPERATURE	0x08
#define REG_FullChrgCap	0x10
#define C1_V 			0x3F
#define C2_V 			0x3E
#define C3_V 			0x3D
#define C4_V 			0x3C
#define REG_STATUS 		0x54
#define REG_SERIAL 		0x1C
#define REG_DATE 		0x1B
#define UNSEAL_KEY1 	0x4242 //command 0x60
#define UNSEAL_KEY2 	0x8484 
#define FA_KEY1 		0x5757 //command 0x61
#define FA_KEY2 		0x5757


class SBS
{
	public:
		uint16_t GetVol(void);
		int16_t GetCur(void);
		float GetTemp(void);
		uint8_t GetCap(void);
		uint16_t FullChargeCap(void);
		uint16_t GetC1(void);
		uint16_t GetC2(void);
		uint16_t GetC3(void);
		uint16_t GetC4(void);
		uint16_t OpStatus(void);
		String GetData(void);
		void SetSerial(uint16_t);
		void SetDate(uint16_t);
		uint16_t GetSerial(void);
		uint16_t GetDate(void);
		void Unseal(void);
		void Seal(void);
		void FullAccess(void);
		static const uint8_t scan();
	private:
		void write(uint8_t,uint8_t);
		uint8_t read(uint8_t);
		uint16_t read16u(uint8_t);
		int16_t read16(uint8_t);
		uint16_t read16u2(uint8_t);
		uint32_t read32u(uint8_t);
		//static void crc(int16_t);
};

#endif