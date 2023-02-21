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
	//{
	//"command":   "moveServo",
	//"servoName": "altitude",
	//"value":     42
	//}
	//
	//{
	//"command":   "moveServo",
	//"servoName": "azimuth",
	//"value":     -42
	//}
	// void moveServo( Servo servoToMove, int pwm );
	// digitalWrite( MCU_LED, LED_ON );
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
				if( callbackJsonDoc.containsKey( "servoName" ) )
				{
					const char *servoName = callbackJsonDoc["servoName"];
					if( strcmp( servoName, "azimuth" ) == 0 )
					{
						if( callbackJsonDoc.containsKey( "value" ) )
						{
							azimuthSpeed = callbackJsonDoc["value"];
							setAzimuthSpeed( azimuthSpeed );
						}
						else
							Serial.printf( "The 'moveServo' command requires the 'value' property to be set!" );
					}
					else if( strcmp( servoName, "altitude" ) == 0 )
					{
						if( callbackJsonDoc.containsKey( "value" ) )
						{
							altitudeSpeed = callbackJsonDoc["value"];
							setAltitude( altitudeSpeed );
						}
						else
							Serial.printf( "The 'moveServo' command requires the 'value' property to be set!" );
					}
					else
						Serial.printf( "Unknown servo name '%s'\n", servoName );
				}
				else
					Serial.printf( "The 'moveServo' command requires the 'servoName' property to be set!" );
			}
			else
				Serial.printf( "Unknown command: '%s'\n", command );
		}
	}
} // End of onReceiveCallback() function.


/**
 * @brief configureOTA() will configure and initiate Over The Air (OTA) updates for this device.
 */
void configureOTA()
{
#ifdef ESP8266
	Serial.println( "Configuring OTA for the ESP8266" );
	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname( hostName );

	// No authentication by default
	ArduinoOTA.setPassword( otaPass );

	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

	ArduinoOTA.onStart( []() {
								  String type;
								  if( ArduinoOTA.getCommand() == U_FLASH )
									  type = "sketch";
								  else // U_SPIFFS
									  type = "filesystem";

								  // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
								  Serial.println( "Start updating " + type );
							  } );
	ArduinoOTA.onEnd( []() {
								Serial.println( "\nEnd" );
							} );
	ArduinoOTA.onProgress( []( unsigned int progress, unsigned int total ) {
									  Serial.printf( "Progress: %u%%\r", ( progress / ( total / 100 ) ) );
								  } );
	ArduinoOTA.onError( []( ota_error_t error ) {
								  Serial.printf( "Error[%u]: ", error );
								  if( error == OTA_AUTH_ERROR ) Serial.println( "Auth Failed" );
								  else if( error == OTA_BEGIN_ERROR )
									  Serial.println( "Begin Failed" );
								  else if( error == OTA_CONNECT_ERROR )
									  Serial.println( "Connect Failed" );
								  else if( error == OTA_RECEIVE_ERROR )
									  Serial.println( "Receive Failed" );
								  else if( error == OTA_END_ERROR )
									  Serial.println( "End Failed" );
							  } );
#else
	Serial.println( "Configuring OTA for the ESP32" );
	// The ESP32 port defaults to 3232
	// ArduinoOTA.setPort( 3232 );
	// The ESP32 hostname defaults to esp32-[MAC]
	//	ArduinoOTA.setHostname( hostName );  // I'm deliberately using the default.
	// Authentication is disabled by default.
	ArduinoOTA.setPassword( otaPass );
	// Password can be set with it's md5 value as well
	// MD5( admin ) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash( "21232f297a57a5a743894a0e4a801fc3" );
	//	Serial.printf( "Using hostname '%s'\n", hostName );

	String type = "filesystem"; // SPIFFS
	if( ArduinoOTA.getCommand() == U_FLASH )
		type = "sketch";

	// Configure the OTA callbacks.
	ArduinoOTA.onStart( []() {
								  String type = "flash"; // U_FLASH
								  if( ArduinoOTA.getCommand() == U_SPIFFS )
									  type = "filesystem";
								  // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
								  Serial.print( "OTA is updating the " );
								  Serial.println( type );
							  } );
	ArduinoOTA.onEnd( []() { Serial.println( "\nTerminating OTA communication." ); } );
	ArduinoOTA.onProgress( []( unsigned int progress, unsigned int total ) { Serial.printf( "OTA progress: %u%%\r", ( progress / ( total / 100 ) ) ); } );
	ArduinoOTA.onError( []( ota_error_t error ) {
								  Serial.printf( "Error[%u]: ", error );
								  if( error == OTA_AUTH_ERROR ) Serial.println( "OTA authentication failed!" );
								  else if( error == OTA_BEGIN_ERROR ) Serial.println( "OTA transmission failed to initiate properly!" );
								  else if( error == OTA_CONNECT_ERROR ) Serial.println( "OTA connection failed!" );
								  else if( error == OTA_RECEIVE_ERROR ) Serial.println( "OTA client was unable to properly receive data!" );
								  else if( error == OTA_END_ERROR ) Serial.println( "OTA transmission failed to terminate properly!" ); } );
#endif

	// Start listening for OTA commands.
	ArduinoOTA.begin();

	Serial.println( "OTA is configured and ready." );
} // End of the configureOTA() function.


/**
 * @brief checkForSSID() will scan for all visible SSIDs, see if any match 'ssidName',
 * and return a count of how many matches were found.
 *
 * @param ssidName the SSID name to search for.
 * @return int the count of SSIDs which match the passed parameter.
 */
int checkForSSID( const char *ssidName )
{
	int ssidCount = 0;
	byte networkCount = WiFi.scanNetworks();
	if( networkCount == 0 )
		Serial.println( "No WiFi SSIDs are in range!" );
	else
	{
		//      Serial.printf( "WiFi SSIDs in range: %d\n", networkCount );
		for( int i = 0; i < networkCount; ++i )
		{
			// Check to see if this SSID matches the parameter.
			if( strcmp( ssidName, WiFi.SSID( i ).c_str() ) == 0 )
				ssidCount++;
		}
	}
	return ssidCount;
} // End of checkForSSID() function.


/**
 * @brief wifiConnect() will attempt to connect to a single SSID.
 */
bool wifiConnect( const char *ssid, const char *password )
{
	wifiConnectCount++;
	// Turn the LED off to show Wi-Fi is not connected.
	digitalWrite( MCU_LED, LED_OFF );

	Serial.printf( "Attempting to connect to Wi-Fi SSID '%s'", ssid );
	WiFi.mode( WIFI_STA );
	//   WiFi.setHostname( hostName );
	WiFi.begin( ssid, password );

	unsigned long wifiConnectionStartTime = millis();

	// Loop until connected, or until wifiConnectionTimeout.
	while( WiFi.status() != WL_CONNECTED && ( millis() - wifiConnectionStartTime < wifiConnectionTimeout ) )
	{
		Serial.print( "." );
		delay( 1000 );
	}
	Serial.println( "" );

	if( WiFi.status() == WL_CONNECTED )
	{
		// Print that Wi-Fi has connected.
		Serial.println( "Wi-Fi connection established!" );
		snprintf( ipAddress, 16, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
		// Turn the LED on to show that Wi-Fi is connected.
		digitalWrite( MCU_LED, LED_ON );
		return true;
	}
	Serial.println( "Wi-Fi failed to connect in the timeout period.\n" );
	return false;
} // End of the wifiConnect() function.

/**
 * @brief wifiMultiConnect() will iterate through the SSIDs in 'wifiSsidList[]', and then use checkForSSID() determine which are in range.
 * When a SSID is in range, wifiConnect() will be called with that SSID and password.
 */
void wifiMultiConnect()
{
	long time = millis();
	if( lastWifiConnectTime == 0 || ( time > wifiCoolDownInterval && ( time - wifiCoolDownInterval ) > lastWifiConnectTime ) )
	{
		Serial.println( "\nEntering wifiMultiConnect()" );
		digitalWrite( MCU_LED, LED_OFF ); // Turn the LED off to show that Wi-Fi is not yet connected.
		for( size_t networkArrayIndex = 0; networkArrayIndex < sizeof( wifiSsidArray ); networkArrayIndex++ )
		{
			// Get the details for this connection attempt.
			const char *wifiSsid = wifiSsidArray[networkArrayIndex];
			const char *wifiPassword = wifiPassArray[networkArrayIndex];

			// Announce the Wi-Fi parameters for this connection attempt.
			Serial.print( "Attempting to connect to to SSID \"" );
			Serial.print( wifiSsid );
			Serial.println( "\"" );

			// Don't even try to connect if the SSID cannot be found.
			int ssidCount = checkForSSID( wifiSsid );
			if( ssidCount > 0 )
			{
				// This is useful for detecting multiples APs.
				if( ssidCount > 1 )
					Serial.printf( "Found %d SSIDs matching '%s'.\n", ssidCount, wifiSsid );

				// If the Wi-Fi connection is successful, set the mqttClient broker parameters.
				if( wifiConnect( wifiSsid, wifiPassword ) )
				{
					mqttClient.setServer( mqttBrokerArray[networkArrayIndex], mqttPortArray[networkArrayIndex] );
					return;
				}
			}
			else
				Serial.printf( "Network '%s' was not found!\n\n", wifiSsid );
		}
		Serial.println( "Exiting wifiMultiConnect()\n" );
		lastWifiConnectTime = millis();
	}
} // End of wifiMultiConnect() function.


/*
 * mqttMultiConnect() will:
 * 1. Check the Wi-Fi connection, and reconnect Wi-Fi as needed.
 * 2. Attempt to connect the MQTT client designated in 'mqttBrokerArray[networkIndex]' up to 'maxAttempts' number of times.
 * 3. Subscribe to the topic defined in 'MQTT_COMMAND_TOPIC'.
 * If the broker connection cannot be made, an error will be printed to the serial port.
 */
int mqttMultiConnect( int maxAttempts )
{
	unsigned long time = millis();
	// Connect the first time.  Avoid subtraction overflow.  Connect every interval.
	if( lastMqttConnectionTime == 0 || ( ( time > mqttReconnectCooldown ) && ( time - mqttReconnectCooldown ) > lastMqttConnectionTime ) )
	{
		digitalWrite( MCU_LED, LED_OFF );
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
		{
			const char *mqttBroker = mqttBrokerArray[networkIndex];
			const int mqttPort = mqttPortArray[networkIndex];
			Serial.printf( "Attempting to connect to the MQTT broker at '%s:%d' up to %d times.\n", mqttBroker, mqttPort, maxAttempts );

			// Set the MQTT client parameters.
			mqttClient.setServer( mqttBroker, mqttPort );
			// Assign the onReceiveCallback() function to handle MQTT callbacks.
			mqttClient.setCallback( onReceiveCallback );
			Serial.printf( "Using MQTT broker: %s\n", mqttBroker );
			Serial.printf( "Using MQTT port: %d\n", mqttPort );


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
				// Connect with the client ID and a "clean" (non-persistent) session.
				if( mqttClient.connect( clientId ) )
				{
					Serial.println( " connected." );
					digitalWrite( MCU_LED, LED_ON );
					if( !mqttClient.setBufferSize( JSON_DOC_SIZE ) )
					{
						Serial.printf( "Unable to create a buffer %lu bytes long!\n", JSON_DOC_SIZE );
						Serial.println( "Restarting the device!" );
						ESP.restart();
					}
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
					Serial.printf( " failed!  Return code: %d", mqttState );
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
		}
		Serial.println( "Function mqttMultiConnect() has completed.\n" );
	}
	lastMqttConnectionTime = millis();
	return 1;
} // End of mqttMultiConnect() function.


/**
 * @brief lookupWifiCode() will return the string for an integer code.
 */
void lookupWifiCode( int code, char *buffer )
{
	switch( code )
	{
		case 0:
			snprintf( buffer, 26, "%s", "Idle" );
			break;
		case 1:
			snprintf( buffer, 26, "%s", "No SSID" );
			break;
		case 2:
			snprintf( buffer, 26, "%s", "Scan completed" );
			break;
		case 3:
			snprintf( buffer, 26, "%s", "Connected" );
			break;
		case 4:
			snprintf( buffer, 26, "%s", "Connection failed" );
			break;
		case 5:
			snprintf( buffer, 26, "%s", "Connection lost" );
			break;
		case 6:
			snprintf( buffer, 26, "%s", "Disconnected" );
			break;
		default:
			snprintf( buffer, 26, "%s", "Unknown Wi-Fi status code" );
	}
} // End of lookupWifiCode() function.


/**
 * @brief lookupMQTTCode() will return the string for an integer state code.
 */
void lookupMQTTCode( int code, char *buffer )
{
	switch( code )
	{
		case -4:
			snprintf( buffer, 29, "%s", "Connection timeout" );
			break;
		case -3:
			snprintf( buffer, 29, "%s", "Connection lost" );
			break;
		case -2:
			snprintf( buffer, 29, "%s", "Connect failed" );
			break;
		case -1:
			snprintf( buffer, 29, "%s", "Disconnected" );
			break;
		case 0:
			snprintf( buffer, 29, "%s", "Connected" );
			break;
		case 1:
			snprintf( buffer, 29, "%s", "Bad protocol" );
			break;
		case 2:
			snprintf( buffer, 29, "%s", "Bad client ID" );
			break;
		case 3:
			snprintf( buffer, 29, "%s", "Unavailable" );
			break;
		case 4:
			snprintf( buffer, 29, "%s", "Bad credentials" );
			break;
		case 5:
			snprintf( buffer, 29, "%s", "Unauthorized" );
			break;
		default:
			snprintf( buffer, 29, "%s", "Unknown MQTT state code" );
	}
} // End of lookupMQTTCode() function.
