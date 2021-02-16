#include "MCP3421.h"

MCP3421::MCP3421(uint8_t inAddress){
	mAddress = 0x68 | (inAddress & 0x7);
	
	// Setup the defaults, just to be sure	
	setConversionMode(mConversionMode);
	setSampleRate(mSampleRate);
	setGain(mGain);
}

void MCP3421::setConversionMode(ConversionMode_t inMode){
	configChangePending = true;
	
	mMode = inMode;
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
	size_t dataLength = (mSampleRate > 240_SPS ? 3 : 4);
	
	char inData[] = new char[dataLength]();
	
	ok = readData(inData, dataLength);
	
	if(ok){
		// Call back the settings
		// 0x9F == RDY | Continuous | 3.75 SPS | x8
		uint8_t config = inData[dataLength -1];
		
		bool dataReady = (config & 0x80) >> 7;			
		mConversionMode = (config & 0x10) >> 4;
		mSampleRate = (config & 0x0C) >> 2;
		mGain = (config & 0x3);
		
		ok = dataReady;
		
		if(dataReady){
			double value = (inData[0] << 24);

			if(dataLength == 4){
				value += (buffer[0] << 16) + (buffer[1] << 8) + buffer[2];
			}else{
				switch(mSampleRate){
					case 240_SPS:	// 12-bits
					{
						value += (buffer[0] << 10);
						break;
					}
					
					case 60_SPS:	// 14-bits
					{
						value += (buffer[0] << 12)
						break;
					}
					
					case 15_SPS:	// 16-bits
					{
						value += (buffer[0] << 14)
						break;
					}
				}				
				value += (buffer[1] << 8) + buffer[2];
			}
			lastValue = value;
		}		
	}
	
	return ok;	
}

double MCP3421::getValue(){
	return lastValue;
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

void MCP3421::readData(uint8_t* outData, size_t length){
	Wire.requestFrom(mAddress, length);
	
	size_t bytesRead = 0;
	while(Wire.available()){
		outData[bytesRead++] = Wire.read();
	}
	
	return (bytesRead == length);
}
