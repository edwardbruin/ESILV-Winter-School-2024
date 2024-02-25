#include <ArduinoBLE.h>
#include <M5Core2.h>
#include <cmath>

int teamNum;
int team_min = 0;
int team_max = 9;
//this is what was used for the winter school:
//String endpointStr = "devincifr.xyz/zebox";
String endpointFront = "https://edwardbruin.github.io/.html";
String endpointEnd = "https://edwardbruin.github.io/.html";

// team can be hard coded by uncommenting the below line
//teamNum = 0;

void setup() {  
  Serial.begin(115200);
  Serial.print("begin sketch");
  
  M5.begin();
  m5.Lcd.setRotation(2);
  
  while (!Serial);
  if (!BLE.begin()) {
    m5.lcd.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  
  if (!teamNum){
	  teamNum = team_min;
	  //Notify user to select their team number
	  M5.Lcd.setTextSize(2);
	  M5.Lcd.setTextColor(WHITE,BLACK);
	  M5.Lcd.println("Select your team \nnumber");
	  
	  //Notify user on which buttons to press. Text will point to buttons B and C
	  M5.Lcd.setCursor(3, 35);
	  M5.Lcd.setTextColor(YELLOW,BLACK);
	  M5.Lcd.println("         Continue >\n\n\n\n\n\n           Change >");
	  
	  //Print the currently selected team number (starts on 0
	  M5.Lcd.setTextColor(RED,BLACK);
	  M5.Lcd.setCursor(3, 35);
	  M5.Lcd.setTextSize(7);
	  m5.lcd.print(teamNum);
	  
	  //loop the buttons until the user selects their team number
	  bool selecting = true;
	  while(selecting){
		m5.update();
		if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) {
		  teamNum = teamNum + 1;
		  if (teamNum>team_max) teamNum = team_min;
		  M5.Lcd.setCursor(3, 35);
		  M5.Lcd.print(teamNum);
		} else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
		  selecting = false;
		}
	  }
  }

  //Print the selected team and begin scanning
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setTextColor(WHITE,BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,0);
  m5.lcd.print("Scanning for SODAQ \nas team: ");
  m5.lcd.println(teamNum);
  BLE.scan();
}

void loop() {
  //retrieve a device
  BLEDevice peripheral = BLE.available();

  //if device exists, and it has a matching name prefix
  if (peripheral && 'D' == peripheral.localName()[0] && 'V' == peripheral.localName()[1] && '_' == peripheral.localName()[2]) {
	String teamNumStr = String(teamNum);
    	String deviceNumStr = String(peripheral.localName()[3]);
	//if you want to use device MAC address instead:
	peripheral.address();
	
	//create the URL for reporting the device found and retrieving the quiz 
	//this is what was used for the winter school:
    //String finalURL = endpointStr +"?found="+ deviceNumStr +"&finder="+ teamNumStr;
    String finalURL = endpointStr +"?device="+ deviceNumStr +"&team="+ teamNumStr;

	//display url as a QR code
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.qrcode( finalURL.c_str(), 0, 80, 240, 6);
    m5.lcd.println("SODAQ found");
    m5.lcd.println(peripheral.localName());
    
	//connect to device and test signal strength
    BLE.stopScan();
    connectPeripheral(peripheral);
	
	//reset and begin searching for device
	M5.Lcd.setTextSize(2);
	M5.Lcd.setCursor(0,0);
	m5.lcd.print("Scanning for SODAQ \nas team: ");
	m5.lcd.println(teamNum);
    BLE.scan();
  }
}

void connectPeripheral(BLEDevice peripheral) {
	//attempt to connect to device
	M5.Lcd.setCursor(0,0);
	M5.Lcd.fillRect(0, 0, 240, 16, WHITE); 
	m5.lcd.println("Connecting");
	
	if (peripheral.connect()) {
		//once connected, measure the relative signal strength indicator (RSSI) over 2 seconds
		M5.Lcd.fillRect(0, 0, 240, 16, WHITE); 
		M5.Lcd.setCursor(0,0);
		m5.lcd.println("Testing");
		m5.lcd.println();
		m5.lcd.println();
		int counter;
		int avgRSSI = 0;
		M5.Lcd.setTextSize(1);
		for(counter = 0;counter <= 19;counter++) {
			//measure the signal strength
			int currRSSI = abs(peripheral.rssi());
			
			// colour code the result 
			M5.Lcd.setTextColor(GREEN,BLACK);
			if (currRSSI> 50) M5.Lcd.setTextColor(YELLOW,BLACK);
			if (currRSSI> 70) M5.Lcd.setTextColor(ORANGE,BLACK);
			if (currRSSI> 90) M5.Lcd.setTextColor(RED,BLACK);
			
			// display result
			m5.lcd.print(currRSSI);
			m5.lcd.print(" ");
			avgRSSI = avgRSSI + abs(peripheral.rssi());
			delay(100);
		}
		//calculate the average RSSI
		avgRSSI = avgRSSI/20;
		M5.Lcd.setTextColor(WHITE,BLACK);
		M5.Lcd.setTextSize(2);
		M5.Lcd.setCursor(0,0);
		m5.lcd.print("Strength: ");
		m5.lcd.print(avgRSSI);
		
		//display average for 5 seconds before exiting
		delay(5000);
	} else {
		M5.Lcd.setCursor(0,0);
		m5.lcd.println("Failed to connect!");
		return;
	}
	peripheral.disconnect();
	return;
};
