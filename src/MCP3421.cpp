#include "MCP3421.h"

using namespace MCP3421_Constants;

MCP3421::MCP3421(uint8_t inAddress){
	mAddress = 0x68 | (inAddress & 0x7);
	
	// Setup the defaults, just to be sure	
	setConversionMode(mConversionMode);
	setSampleRate(mSampleRate);
	setGain(mGain);
}

void MCP3421::setConversionMode(ConversionMode_t inMode){
	configChangePending = true;
	
	mConversionMode = inMode;
}

void MCP3421::setSampleRate(SampleRate_t inSampleRate){
	configChangePending = true;
	
	mSampleRate = inSampleRate;
}

void MCP3421::setGain(Gain_t inGain){
	configChangePending = true;
	
	mGain = inGain;
}

bool MCP3421::isConversionComplete(){
	bool ok = false;
	
	// When in oneshot mode, writing the config will set the RDY bit to 1, initiating a new conversion
	if(configChangePending || mConversionMode == ConversionMode::OneShot)
		writeConfig();
		
	configChangePending = false;
	
	// The length depends on the SampleRate, which sets the resolution
	size_t dataLength = (mSampleRate > SPS_240 ? 3 : 4);
	
	uint8_t inData[dataLength];
	
	ok = readData(inData, dataLength);
	
	if(ok){
		// Call back the settings
		// 0x9F == RDY | Continuous | 3.75 SPS | x8
		uint8_t config = inData[dataLength - 1];
		
		bool dataReady = (config & 0x80) >> 7;			
		mConversionMode = (ConversionMode_t)(uint8_t)((config & 0x10) >> 4);
		mSampleRate = (SampleRate_t)(uint8_t)((config & 0x0C) >> 2);
		mGain = (Gain_t)(uint8_t)(config & 0x3);
		
		ok = dataReady;
		
		uint8_t numBits = 18; // Default

		if(dataReady){
			uint32_t value = 0;

			switch(mSampleRate){
				case SPS_240:	// 12-bits
				{
					numBits = 12;
					value = ((inData[0] & 0x0F) << 8) + inData[1];
					break; 
				}
				
				case SPS_60:	// 14-bits
				{
					numBits = 14;
					value = ((inData[0] & 0x3F) << 8) + inData[1];
					break;
				}
				
				case SPS_15:	// 16-bits
				{
					numBits = 16;
					value = ((inData[0] & 0xFF) << 8) + inData[1];
					break;
				}
				case SPS_3_75:	// 18-bits
				{
					numBits = 18;
					value = ((((uint16_t)inData[0]) & 0x03) << 16) + (((uint16_t)inData[1]) << 8) + inData[2];
					break;
				}
			}			

			if(value > (pow(2, numBits) / 2) - 1){
				value -= pow(2, numBits) - 1 ;
			}

			lastValue = value;
		}		
	}
	
	return ok;	
}

// Returns the last read raw value
uint32_t MCP3421::getValue(){
	return lastValue;
}

// Returns the last read voltage
float MCP3421::getVoltage(){

	uint8_t numBits = 18; // Default

	switch(mSampleRate){
		case SPS_240:	// 12-bits
		{
			numBits = 12;
			break; 
		}
		
		case SPS_60:	// 14-bits
		{
			numBits = 14;
			break;
		}
		
		case SPS_15:	// 16-bits
		{
			numBits = 16;
			break;
		}
		case SPS_3_75:	// 18-bits
		{
			numBits = 18;
			break;
		}
	}			

	uint32_t value = getValue();
	return (float(value) * 4.096) / float (pow(2, numBits));
}

void MCP3421::writeConfig(){
					// RDY | CONV_MODE 		| SAMPLE_RATE 	| PGA
	uint8_t config = (mConversionMode == ConversionMode::OneShot << 7) | (mConversionMode << 4) | (mSampleRate  << 2) | mGain;
	
	writeData(config);
}

void MCP3421::writeData(uint8_t inData){
	Wire.beginTransmission(mAddress);
	Wire.write(inData);
	Wire.endTransmission();
}

bool MCP3421::readData(uint8_t* outData, size_t length){
	Wire.requestFrom(mAddress, length);
	
	size_t bytesRead = 0;
	while(Wire.available()){
		outData[bytesRead++] = Wire.read();
	}
	
	return (bytesRead == length);
}