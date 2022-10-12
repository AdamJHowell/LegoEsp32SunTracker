/**
 * @brief The purpose of this file is to hold network-related functions which are device-agnostic.
 * This is not realistic because of the presence of onReceiveCallback.
 * Ideally, this file could be used by an ESP32, ESP8266, or similar boards.
 * Because memory capacity varies wildly from device to device, buffer sizes are declared as variables in the entry-point file.
 */
#include "LegoEsp32SunTracker.h"


/**
 * onReceiveCallback() handles MQTT subscriptions.
 * When a message comes in on a topic we have subscribed to, this function is executed.
 */
void onReceiveCallback( char *topic, byte *payload, unsigned int length )
{
	Serial.printf( "New message on topic '%s'\n", topic );
	// ToDo: Determine which commands this device should respond to.
	// void moveServo( Servo servoToMove, int pwm );
	// digitalWrite( MCU_LED, HIGH );
	if( length > 0 )
	{
		callbackCount++;
		// Create a document named callbackJsonDoc to hold the callback payload.
		StaticJsonDocument<JSON_DOC_SIZE> callbackJsonDoc;
		// Deserialize payload into callbackJsonDoc.
		deserializeJson( callbackJsonDoc, payload, length );

		if( callbackJsonDoc.containsKey( "command" ) )
		{
			// Store the command value.
			const char *command = callbackJsonDoc["command"];

			// Available commands are: moveServo and publishStats.
			if( strcmp( command, "moveServo" ) == 0 )
			{
				Serial.println( "TakePicture command processed." );
			}
			else if( strcmp( command, "publishStats" ) == 0 )
				publishStats();
			else
				Serial.printf( "Unknown command: '%s'\n", command );
		}
	}
} // End of onReceiveCallback() function.


/**
 * @brief configureOTA() will configure and initiate Over The Air (OTA) updates for this device.
 *
 */
void configureOTA()
{
	Serial.println( "Configuring OTA." );

#ifdef ESP8266
	// The ESP8266 hostname defaults to esp8266-[ChipID].
	// The ESP8266 port defaults to 8266.
	// ArduinoOTA.setPort( 8266 );
	// Authentication is disabled by default.
	// ArduinoOTA.setPassword( ( const char * ) "admin" );
#elif ESP32
	// The ESP32 hostname defaults to esp32-[MAC].
	// The ESP32 port defaults to 3232.
	// ArduinoOTA.setPort( 3232 );
	// Authentication is disabled by default.
	// ArduinoOTA.setPassword( "admin" );
	// Password can be set with it's md5 value as well.
	// MD5( admin ) = 21232f297a57a5a743894a0e4a801fc3.
	// ArduinoOTA.setPasswordHash( "21232f297a57a5a743894a0e4a801fc3" );
#else
	// ToDo: Verify how stock Arduino code is meant to handle the port, username, and password.
#endif

	// ArduinoOTA is a class-defined object.
	ArduinoOTA.setHostname( HOST_NAME );

	Serial.printf( "Using hostname '%s'\n", HOST_NAME );

	String type = "filesystem"; // SPIFFS
	if( ArduinoOTA.getCommand() == U_FLASH )
		type = "sketch";

	// Configure the OTA callbacks.
	// Port defaults to 8266.
	// ArduinoOTA.setPort( 8266 );

	// Hostname defaults to esp8266-[ChipID].
	// ArduinoOTA.setHostname( "myesp8266" );

	// No authentication by default.
	// ArduinoOTA.setPassword( "admin" );

	// Password can be set with it's md5 value as well.
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3.
	// ArduinoOTA.setPasswordHash( "21232f297a57a5a743894a0e4a801fc3" );

	ArduinoOTA.onStart( []()
							  {
								  String type;
								  if( ArduinoOTA.getCommand() == U_FLASH )
									  type = "sketch";
								  else // U_SPIFFS
									  type = "filesystem";

								  // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
								  Serial.println( "Start updating " + type ); } );
	ArduinoOTA.onEnd( []()
							{ Serial.println( "\nEnd" ); } );
	ArduinoOTA.onProgress( []( unsigned int progress, unsigned int total )
								  { Serial.printf( "Progress: %u%%\r", ( progress / ( total / 100 ) ) ); } );
	ArduinoOTA.onError( []( ota_error_t error )
							  {
								  Serial.printf( "Error[%u]: ", error );
								  if( error == OTA_AUTH_ERROR ) Serial.println( "Auth Failed" );
								  else if( error == OTA_BEGIN_ERROR ) Serial.println( "Begin Failed" );
								  else if( error == OTA_CONNECT_ERROR ) Serial.println( "Connect Failed" );
								  else if( error == OTA_RECEIVE_ERROR ) Serial.println( "Receive Failed" );
								  else if( error == OTA_END_ERROR ) Serial.println( "End Failed" ); } );

	// Start listening for OTA commands.
	ArduinoOTA.begin();

	Serial.println( "OTA is configured and ready." );
} // End of the configureOTA() function.


void wifiConnect()
{
	digitalWrite( MCU_LED, LOW );

	Serial.println( "\nEntering wifiConnect()" );

	// Announce the Wi-Fi parameters for this connection attempt.
	Serial.printf( "Attempting to connect to SSID '%s'.\n", wifiSsid );

	// Don't even try to connect if the SSID cannot be found.
	if( checkForSSID( wifiSsid ) )
	{
		// Set the Wi-Fi mode to station.
		Serial.printf( "Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode( WIFI_STA ) ? "" : "Failed!" );
		if( WiFi.setHostname( HOST_NAME ) )
			Serial.printf( "Network hostname set to '%s'\n", HOST_NAME );
		else
			Serial.printf( "Failed to set the network hostname to '%s'\n", HOST_NAME );
		WiFi.begin( wifiSsid, wifiPassword );

		unsigned long wifiConnectionStartTime = millis();
		Serial.printf( "Waiting up to %u seconds for a connection.\n", wifiConnectionTimeout / 1000 );
		/*
			WiFi.status() return values:
			WL_IDLE_STATUS      = 0,
			WL_NO_SSID_AVAIL    = 1,
			WL_SCAN_COMPLETED   = 2,
			WL_CONNECTED        = 3,
			WL_CONNECT_FAILED   = 4,
			WL_CONNECTION_LOST  = 5,
			WL_WRONG_PASSWORD   = 6,
			WL_DISCONNECTED     = 7
			*/
		while( WiFi.status() != WL_CONNECTED && ( millis() - wifiConnectionStartTime < wifiConnectionTimeout ) )
		{
			Serial.print( "." );
			delay( 1000 );
		}
		Serial.println( "" );

		if( WiFi.status() == WL_CONNECTED )
		{
			// Set the global 'networkIndex' to the index which successfully connected.
			networkIndex = networkArrayIndex;
			// Print that Wi-Fi has connected.
			Serial.println( "\nWiFi connection established!" );
			snprintf( ipAddress, 16, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
			Serial.printf( "IP address: %s\n", ipAddress );
			digitalWrite( MCU_LED, HIGH );
			return;
		}
		else
			Serial.println( "Unable to connect to WiFi!" );
	}
	Serial.println( "Exiting wifiMultiConnect()\n" );
}


int mqttConnect()
{
	digitalWrite( MCU_LED, LOW );

	Serial.println( "\nFunction mqttMultiConnect() has initiated." );
	if( WiFi.status() != WL_CONNECTED )
		wifiMultiConnect();
	else
		Serial.printf( "Wi-Fi is already connected with client address %s\n", ipAddress );

	Serial.printf( "Attempting to connect to the MQTT broker at '%s:%d' up to %d times.\n", mqttBroker, mqttPort, maxAttempts );

	int attemptNumber = 0;
	// Loop until MQTT has connected.
	while( !mqttClient.connected() && attemptNumber < maxAttempts )
	{
		// Put the macAddress and random number into clientId.
		char clientId[22];
		//		snprintf( clientId, 22, "%s-%03ld", macAddress, random( 999 ) );
		snprintf( clientId, 19, "%s", macAddress );
		// Connect to the broker using the MAC address for a clientID.  This guarantees that the clientID is unique.
		Serial.printf( "Connecting with client ID '%s'.\n", clientId );
		Serial.printf( "Attempt # %d....", ( attemptNumber + 1 ) );
		if( mqttClient.connect( clientId ) )
		{
			Serial.println( " connected." );
			digitalWrite( MCU_LED, HIGH );
			if( !mqttClient.setBufferSize( JSON_DOC_SIZE ) )
			{
				Serial.printf( "Unable to create a buffer %lu bytes long!\n", JSON_DOC_SIZE );
				Serial.println( "Restarting the device!" );
				ESP.restart();
			}
			publishStats();
			// Subscribe to the command topic.
			if( mqttClient.subscribe( MQTT_COMMAND_TOPIC ) )
				Serial.printf( "Successfully subscribed to topic '%s'.\n", MQTT_COMMAND_TOPIC );
			else
				Serial.printf( "Failed to subscribe to topic '%s'!\n", MQTT_COMMAND_TOPIC );
		}
		else
		{
			int mqttState = mqttClient.state();
			/*
				Possible values for client.state():
				MQTT_CONNECTION_TIMEOUT     -4		// Note: This also comes up when the clientID is already in use.
				MQTT_CONNECTION_LOST        -3
				MQTT_CONNECT_FAILED         -2
				MQTT_DISCONNECTED           -1
				MQTT_CONNECTED               0
				MQTT_CONNECT_BAD_PROTOCOL    1
				MQTT_CONNECT_BAD_CLIENT_ID   2
				MQTT_CONNECT_UNAVAILABLE     3
				MQTT_CONNECT_BAD_CREDENTIALS 4
				MQTT_CONNECT_UNAUTHORIZED    5
			*/
			Serial.printf( " failed!  Return code: %d\n", mqttState );
			if( mqttState == -4 )
				Serial.println( " - MQTT_CONNECTION_TIMEOUT" );
			else if( mqttState == 2 )
				Serial.println( " - MQTT_CONNECT_BAD_CLIENT_ID" );
			else
				Serial.println( "" );

			Serial.printf( "Trying again in %u seconds.\n\n", mqttReconnectInterval / 1000 );
			delay( mqttReconnectInterval );
		}
		attemptNumber++;
	}

	if( !mqttClient.connected() )
	{
		Serial.println( "Unable to connect to the MQTT broker!" );
		return 0;
	}

	Serial.println( "Function mqttMultiConnect() has completed.\n" );
	return 1;
}


/*
 * wifiMultiConnect() will iterate through 'wifiSsidArray[]', attempting to connect with the password stored at the same index in 'wifiPassArray[]'.
 *
 */
void wifiMultiConnect()
{
	digitalWrite( MCU_LED, LOW );

	Serial.println( "\nEntering wifiMultiConnect()" );
	for( size_t networkArrayIndex = 0; networkArrayIndex < sizeof( wifiSsidArray ); networkArrayIndex++ )
	{
		// Get the details for this connection attempt.
		const char *wifiSsid = wifiSsidArray[networkArrayIndex];
		const char *wifiPassword = wifiPassArray[networkArrayIndex];

		// Announce the Wi-Fi parameters for this connection attempt.
		Serial.printf( "Attempting to connect to SSID '%s'.\n", wifiSsid );

		// Don't even try to connect if the SSID cannot be found.
		if( checkForSSID( wifiSsid ) )
		{
			// Set the Wi-Fi mode to station.
			Serial.printf( "Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode( WIFI_STA ) ? "" : "Failed!" );
			if( WiFi.setHostname( HOST_NAME ) )
				Serial.printf( "Network hostname set to '%s'\n", HOST_NAME );
			else
				Serial.printf( "Failed to set the network hostname to '%s'\n", HOST_NAME );
			WiFi.begin( wifiSsid, wifiPassword );

			unsigned long wifiConnectionStartTime = millis();
			Serial.printf( "Waiting up to %u seconds for a connection.\n", wifiConnectionTimeout / 1000 );
			/*
			WiFi.status() return values:
			WL_IDLE_STATUS      = 0,
			WL_NO_SSID_AVAIL    = 1,
			WL_SCAN_COMPLETED   = 2,
			WL_CONNECTED        = 3,
			WL_CONNECT_FAILED   = 4,
			WL_CONNECTION_LOST  = 5,
			WL_WRONG_PASSWORD   = 6,
			WL_DISCONNECTED     = 7
			*/
			while( WiFi.status() != WL_CONNECTED && ( millis() - wifiConnectionStartTime < wifiConnectionTimeout ) )
			{
				Serial.print( "." );
				delay( 1000 );
			}
			Serial.println( "" );

			if( WiFi.status() == WL_CONNECTED )
			{
				// Set the global 'networkIndex' to the index which successfully connected.
				networkIndex = networkArrayIndex;
				// Print that Wi-Fi has connected.
				Serial.println( "\nWiFi connection established!" );
				snprintf( ipAddress, 16, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
				Serial.printf( "IP address: %s\n", ipAddress );
				digitalWrite( MCU_LED, HIGH );
				return;
			}
			else
				Serial.println( "Unable to connect to WiFi!" );
		}
	}
	Serial.println( "Exiting wifiMultiConnect()\n" );
} // End of wifiMultiConnect() function.


/*
 * checkForSSID() is used by wifiMultiConnect() to avoid attempting to connect to SSIDs which are not in range.
 * Returns 1 if 'ssidName' can be found.
 * Returns 0 if 'ssidName' cannot be found.
 */
int checkForSSID( const char *ssidName )
{
	byte networkCount = WiFi.scanNetworks();
	if( networkCount == 0 )
		Serial.println( "No WiFi SSIDs are in range!" );
	else
	{
		Serial.printf( "%d networks found.\n", networkCount );
		for( int i = 0; i < networkCount; ++i )
		{
			// Check to see if this SSID matches the parameter.
			if( strcmp( ssidName, WiFi.SSID( i ).c_str() ) == 0 )
				return 1;
			// Alternately, the String compareTo() function can be used like this: if( WiFi.SSID( i ).compareTo( ssidName ) == 0 )
		}
	}
	Serial.printf( "SSID '%s' was not found!\n", ssidName );
	return 0;
} // End of checkForSSID() function.


/*
 * mqttMultiConnect() will:
 * 1. Check the Wi-Fi connection, and reconnect Wi-Fi as needed.
 * 2. Attempt to connect the MQTT client designated in 'mqttBrokerArray[networkIndex]' up to 'maxAttempts' number of times.
 * 3. Subscribe to the topic defined in 'MQTT_COMMAND_TOPIC'.
 * If the broker connection cannot be made, an error will be printed to the serial port.
 */
int mqttMultiConnect( int maxAttempts )
{
	digitalWrite( MCU_LED, LOW );

	Serial.println( "\nFunction mqttMultiConnect() has initiated." );
	if( WiFi.status() != WL_CONNECTED )
		wifiMultiConnect();
	else
		Serial.printf( "Wi-Fi is already connected with client address %s\n", ipAddress );

	/*
	 * The networkIndex variable is initialized to 2112.
	 * If it is still 2112 at this point, then Wi-Fi failed to connect.
	 * This is only needed to display the name and port of the broker being used.
	 */
	if( networkIndex != 2112 )
		Serial.printf( "Attempting to connect to the MQTT broker at '%s:%d' up to %d times.\n", mqttBrokerArray[networkIndex], mqttPortArray[networkIndex], maxAttempts );
	else
		Serial.printf( "Attempting to connect to the MQTT broker up to %d times.\n", maxAttempts );


	int attemptNumber = 0;
	// Loop until MQTT has connected.
	while( !mqttClient.connected() && attemptNumber < maxAttempts )
	{
		// Put the macAddress and random number into clientId.
		char clientId[22];
		//		snprintf( clientId, 22, "%s-%03ld", macAddress, random( 999 ) );
		snprintf( clientId, 19, "%s", macAddress );
		// Connect to the broker using the MAC address for a clientID.  This guarantees that the clientID is unique.
		Serial.printf( "Connecting with client ID '%s'.\n", clientId );
		Serial.printf( "Attempt # %d....", ( attemptNumber + 1 ) );
		if( mqttClient.connect( clientId ) )
		{
			Serial.println( " connected." );
			digitalWrite( MCU_LED, HIGH );
			if( !mqttClient.setBufferSize( JSON_DOC_SIZE ) )
			{
				Serial.printf( "Unable to create a buffer %lu bytes long!\n", JSON_DOC_SIZE );
				Serial.println( "Restarting the device!" );
				ESP.restart();
			}
			publishStats();
			// Subscribe to the command topic.
			if( mqttClient.subscribe( MQTT_COMMAND_TOPIC ) )
				Serial.printf( "Successfully subscribed to topic '%s'.\n", MQTT_COMMAND_TOPIC );
			else
				Serial.printf( "Failed to subscribe to topic '%s'!\n", MQTT_COMMAND_TOPIC );
		}
		else
		{
			int mqttState = mqttClient.state();
			/*
				Possible values for client.state():
				MQTT_CONNECTION_TIMEOUT     -4		// Note: This also comes up when the clientID is already in use.
				MQTT_CONNECTION_LOST        -3
				MQTT_CONNECT_FAILED         -2
				MQTT_DISCONNECTED           -1
				MQTT_CONNECTED               0
				MQTT_CONNECT_BAD_PROTOCOL    1
				MQTT_CONNECT_BAD_CLIENT_ID   2
				MQTT_CONNECT_UNAVAILABLE     3
				MQTT_CONNECT_BAD_CREDENTIALS 4
				MQTT_CONNECT_UNAUTHORIZED    5
			*/
			Serial.printf( " failed!  Return code: %d\n", mqttState );
			if( mqttState == -4 )
				Serial.println( " - MQTT_CONNECTION_TIMEOUT" );
			else if( mqttState == 2 )
				Serial.println( " - MQTT_CONNECT_BAD_CLIENT_ID" );
			else
				Serial.println( "" );

			Serial.printf( "Trying again in %u seconds.\n\n", mqttReconnectInterval / 1000 );
			delay( mqttReconnectInterval );
		}
		attemptNumber++;
	}

	if( !mqttClient.connected() )
	{
		Serial.println( "Unable to connect to the MQTT broker!" );
		return 0;
	}

	Serial.println( "Function mqttMultiConnect() has completed.\n" );
	return 1;
} // End of mqttMultiConnect() function.


/*
 * publishStats() is called by mqttConnect() every time the device (re)connects to the broker, and every publishInterval milliseconds thereafter.
 * It is also called by the callback when the "publishStats" command is received.
 */
void publishStats()
{
	if( mqttClient.connected() )
	{
		char mqttStatsString[JSON_DOC_SIZE];
		// Create a JSON Document on the stack.
		StaticJsonDocument<JSON_DOC_SIZE> statsJsonDoc;
		// Add data: SKETCH_NAME, macAddress, ipAddress, rssi, publishCount
		statsJsonDoc["sketch"] = SKETCH_NAME;
		statsJsonDoc["mac"] = macAddress;
		statsJsonDoc["ip"] = ipAddress;
		statsJsonDoc["rssi"] = rssi;
		statsJsonDoc["publishCount"] = publishCount;

		// Serialize statsJsonDoc into mqttStatsString, with indentation and line breaks.
		serializeJsonPretty( statsJsonDoc, mqttStatsString );

		Serial.printf( "Publishing stats to the '%s' topic.\n", MQTT_STATS_TOPIC );

		if( mqttClient.publish( MQTT_STATS_TOPIC, mqttStatsString ) )
		{
			Serial.printf( "Published to this broker: '%s:%d' on this topic '%s'.\n", mqttBrokerArray[networkIndex], mqttPortArray[networkIndex], MQTT_STATS_TOPIC );
			Serial.println( mqttStatsString );
		}
		else
			Serial.print( "\n\nPublish failed!\n\n" );
	}
} // End of publishStats() function.


void readTelemetry()
{
	rssi = WiFi.RSSI();
}


void publishTelemetry()
{
}


void printTelemetry()
{
	Serial.printf( "MAC address: %s\n", macAddress );
	Serial.printf( "IP address: %s\n", ipAddress );
	Serial.printf( "RSSI: %ld\n", rssi );
	Serial.printf( "Host  name: %s\n", HOST_NAME );
	Serial.printf( "Sketch file name: %s\n", __FILE__ );
	Serial.printf( "Notes: %s\n", NOTES );
	Serial.printf( "Publish count: %ld\n", publishCount );
}
