#include "MCP3421.h"

using namespace MCP3421_Constants;

MCP3421 adc(1);

void setup(){
	// Change sample rate to 240 60 SPS
	adc.setSampleRate(SampleRate::SPS_60);
	
	// Change conversion mode to oneshot
	adc.setConversionMode(ConversionMode::OneShot);
}

void loop(){
	if(adc.isConversionComplete()){
		// Print out the result
		Serial.printf("ADC Read: %f\n", adc.getValue());
	}
	
	delay(1000);
}