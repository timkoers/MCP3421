#include "MCP3421.h"

MCP3421 adc;

void setup(){
	// Change sample rate to 240 3.75 SPS
	adc.setSampleRate(SampleRate::3_75_SPS);
	
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