/**
 * LegoEsp32SunTracker is a simple sun tracker built with an ESP32 and Lego Technic components.
 * See README.md for details.
 */
#include "LegoEsp32SunTracker.h"


// ToDo: Move the altitude proportional to the upperSum/lowerSum delta.
// ToDo: Move the azimuth proportional to the leftSum/RightSum delta.


/**
 * @brief readTelemetry() will gather basic device information and store those values in global variables.
 */
void readTelemetry()
{
   rssi = WiFi.RSSI();
   upperLeftValue = analogRead( upperLeftGPIO );
   upperRightValue = analogRead( upperRightGPIO );
   lowerLeftValue = analogRead( lowerLeftGPIO );
   lowerRightValue = analogRead( lowerRightGPIO );
} // End of readTelemetry() function.


/**
 * @brief printTelemetry() will print basic device information to the serial port.
 */
void printTelemetry()
{
   Serial.println();
   printCount++;
   Serial.println( __FILE__ );
   Serial.printf( "Print count %ld\n", printCount );
   Serial.println();

   Serial.println( "Network stats:" );
   Serial.printf( "  MAC address: %s\n", macAddress );
   int wifiStatusCode = WiFi.status();
   char buffer[29];
   lookupWifiCode( wifiStatusCode, buffer );
   if( wifiStatusCode == 3 )
   {
      Serial.printf( "  IP address: %s\n", ipAddress );
      Serial.printf( "  RSSI: %ld\n", rssi );
      Serial.print( "~~IP address: " );
      Serial.println( WiFi.localIP() );
   }
   Serial.printf( "  wifiConnectCount: %u\n", wifiConnectCount );
   Serial.printf( "  wifiCoolDownInterval: %lu\n", wifiCoolDownInterval );
   Serial.printf( "  Wi-Fi status text: %s\n", buffer );
   Serial.printf( "  Wi-Fi status code: %d\n", wifiStatusCode );
   Serial.println();

   Serial.println( "MQTT stats:" );
   Serial.printf( "  mqttConnectCount: %u\n", mqttConnectCount );
   Serial.printf( "  mqttCoolDownInterval: %lu\n", mqttCoolDownInterval );
   if( mqttClient.connected() )
      Serial.printf( "  Broker: %s:%d\n", mqttClient.getServerDomain(), mqttClient.getServerPort() );
   lookupMQTTCode( mqttClient.state(), buffer );
   Serial.printf( "  MQTT state: %s\n", buffer );
   Serial.printf( "  Publish count: %lu\n", publishCount );
   Serial.printf( "  Callback count: %lu\n", callbackCount );
   Serial.println();

   Serial.println( "Device stats:" );
   Serial.printf( "  Altitude servo speed: %d\n", altitudeSpeed );
   Serial.printf( "  Azimuth servo speed: %d\n", azimuthSpeed );
   Serial.println();
   int upperSum = upperLeftValue + upperRightValue;
   int lowerSum = lowerLeftValue + lowerRightValue;
   int leftSum = upperLeftValue + lowerLeftValue;
   int rightSum = upperRightValue + lowerRightValue;
   Serial.println( "  Analog readings:" );
   Serial.printf( " ------|------\n" );
   Serial.printf( "| %4d | %4d |-- %4d\n", upperLeftValue, upperRightValue, upperSum );
   Serial.printf( "|------|------|\n" );
   Serial.printf( "| %4d | %4d |-- %4d\n", lowerLeftValue, lowerRightValue, lowerSum );
   Serial.printf( " ---|--|---|--\n" );
   Serial.printf( "    |      |\n" );
   Serial.printf( "  %4d   %4d\n", leftSum, rightSum );
   Serial.println();

   Serial.printf( "Upper - Lower = %d\n", upperSum - lowerSum );
   Serial.printf( "Left - Right = %d\n", leftSum - rightSum );

   if( abs( upperSum - lowerSum ) > 50 )
   {
      if( upperSum < lowerSum )
         Serial.println( "Move up!" );
      else
         Serial.println( "Move down!" );
   }
   else
      Serial.println( "Altitude hold!" );

   if( abs( leftSum - rightSum ) > 50 )
   {
      if( leftSum < rightSum )
         Serial.println( "Move left!" );
      else
         Serial.println( "Move right!" );
   }
   else
      Serial.println( "Azimuth hold!" );

   Serial.printf( "inclinationStopPin: %d\n", digitalRead( inclinationStopPin ) );
   Serial.printf( "declinationStopPin: %d\n", digitalRead( declinationStopPin ) );
   Serial.printf( "altitudeSpeed: %d\n", altitudeSpeed );
   Serial.printf( "azimuthSpeed: %d\n", azimuthSpeed );

   char valueBuffer[25] = "";
   snprintf( valueBuffer, 25, "%d", altitudeSpeed );
   mqttClient.publish( "LegoSunTracker/altitudeSpeed", valueBuffer );
   snprintf( valueBuffer, 25, "%d", azimuthSpeed );
   mqttClient.publish( "LegoSunTracker/azimuthSpeed", valueBuffer );

   snprintf( valueBuffer, 25, "%d", upperLeftValue );
   mqttClient.publish( "LegoSunTracker/upperLeftValue", valueBuffer );
   snprintf( valueBuffer, 25, "%d", upperRightValue );
   mqttClient.publish( "LegoSunTracker/upperRightValue", valueBuffer );
   snprintf( valueBuffer, 25, "%d", lowerLeftValue );
   mqttClient.publish( "LegoSunTracker/lowerLeftValue", valueBuffer );
   snprintf( valueBuffer, 25, "%d", lowerRightValue );
   mqttClient.publish( "LegoSunTracker/lowerRightValue", valueBuffer );

   Serial.printf( "Next telemetry poll in %lu seconds\n", telemetryPrintInterval / MILLIS_IN_SEC );
   Serial.println( "\n" );
} // End of printTelemetry() function.


void setup()
{
   delay( 1000 );
   // Start the Serial communication to send messages to the computer.
   Serial.begin( 115200 );
   if( !Serial )
      delay( 1000 );
   Serial.println( "\n\nsetup() is beginning." );

   // Allocate all timers.
   ESP32PWM::allocateTimer( 0 );
   ESP32PWM::allocateTimer( 1 );
   ESP32PWM::allocateTimer( 2 );
   ESP32PWM::allocateTimer( 3 );

   // Standard 50 hz servo.
   altitudeServo.setPeriodHertz( 50 );
   // Connects the servo object to GPIO 22, sets 500 μsec as the minimum pulse width and 2500 μsec as the maximum pulse width.
   altitudeServo.attach( altitudeServoPin, minPulseWidth, maxPulseWidth );
   //	setAltitude( altitudePosition );
   setAltitudeSpeed( altitudeSpeed );

   // Standard 50 hz servo.
   azimuthServo.setPeriodHertz( 50 );
   // Connects the servo object to GPIO 23, sets 500 μsec as the minimum pulse width and 2500 μsec as the maximum pulse width.
   azimuthServo.attach( azimuthServoPin, minPulseWidth, maxPulseWidth );
   setAzimuthSpeed( azimuthSpeed );

   // Limit switches.
   pinMode( declinationStopPin, INPUT );
   pinMode( inclinationStopPin, INPUT );

   // Set GPIO 2 (MCU_LED) as an output.
   pinMode( MCU_LED, OUTPUT );
   // Turn the LED on.
   digitalWrite( MCU_LED, LED_ON );

   // Set the MAC address variable to its value.
   snprintf( macAddress, 18, "%s", WiFi.macAddress().c_str() );

   wifiMultiConnect();

   configureOTA();

   Serial.println( "setup() has finished.\n" );
} // End of setup() function.


/**
 * @brief toggleLED() will change the state of the LED.
 * This function does not manage any timings.
 */
void toggleLED()
{
   if( digitalRead( MCU_LED ) != LED_ON )
      digitalWrite( MCU_LED, LED_ON );
   else
      digitalWrite( MCU_LED, LED_OFF );
} // End of toggleLED() function.


void loop()
{
   // Process OTA requests.
   ArduinoOTA.handle();

   // Poll the first time.  Poll every interval.
   if( lastTelemetryPollTime == 0 || ( millis() - lastTelemetryPollTime > telemetryPollInterval ) )
   {
      readTelemetry();
      lastTelemetryPollTime = millis();
   }

   //	Check the MQTT connection, and reconnect if needed.
   if( !mqttClient.connected() )
      mqttConnect();
   // The MQTT client loop() function facilitates the receiving of messages and maintains the connection to the broker.
   mqttClient.loop();

   // Process the first time.  Process every interval.
   if( lastTelemetryProcessTime == 0 || ( millis() - lastTelemetryProcessTime > telemetryProcessInterval ) )
   {
      moveArm();
      lastTelemetryProcessTime = millis();

      // If Wi-Fi is connected, but MQTT is not, blink the LED.
      if( WiFi.status() == WL_CONNECTED )
      {
         if( mqttClient.state() != 0 )
            toggleLED();
         else
            digitalWrite( MCU_LED, LED_ON );
      }
   }

   // Print the first time.  Print every interval.
   if( lastTelemetryPrintTime == 0 || ( millis() - lastTelemetryPrintTime > telemetryPrintInterval ) )
   {
      printTelemetry();
      lastTelemetryPrintTime = millis();
   }
} // End of loop() function.
