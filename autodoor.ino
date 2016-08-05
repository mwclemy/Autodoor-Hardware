// libraries
#include <GSM.h>
//#include <ArduinoJson.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "internet" // replace your GPRS APN
#define GPRS_LOGIN     ""    // replace with your GPRS login
#define GPRS_PASSWORD  "" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;
GSM_SMS sms;


// variables to help in parsing json received
String currentLine = "";
String command = "";
boolean foundIt = false;

// variables to help in sending the message
char remoteNumber[20]="0736465825";  // telephone number to send sms
char txtMsg[200]="The door has been opened";  // text message

// variables to help in repeatdly connection
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

// The output led
int led = 8;

 // A function to help connect to the web server.
 void connectToServer(char method[], char server[] ,char path[] ,int port) {
  
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  
 Serial.println("connecting...");
// if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.print(method);
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
    
    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 
}   


// A function to help sending an SMS to a given mobile handset.
void sendSMS(){

  Serial.print("Message to mobile number: ");
  Serial.println(remoteNumber);

  // sms text
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNumber);
  delay(1000);
  sms.print(txtMsg);
  delay(1000);
  sms.endSMS();
  delay(1000);
  Serial.println("\nCOMPLETE!\n");  
}

void setup() {
  // put your setup code here, to run once:
 pinMode(led, OUTPUT);
 // reserve space for the strings:
  currentLine.reserve(256);
  command.reserve(50);
  
// initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
}

 Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (notConnected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      notConnected = false;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
 

}
void loop() {
// put your main code here, to run repeatedly:
// if there are incoming bytes available
  // from the server, read them and print them:
  command = "";
  
 while (client.available()) {
    char c = client.read();


   currentLine += c;
   if (c == '\n') {
    currentLine = "";
   }
if ( currentLine.endsWith("{\"message\":\"")) {
       foundIt = true;
        
      // break out of the loop so this character is not added to command;
      return;
  }

    // if you are currently reading the bytes of the commnad indicating command sent,
   // add them to the command String:
    if (foundIt) {
     if (c != '"') {
      command += c;
      
   }
else {
        // if you got a '"' character, you have read the string representing the command:
       foundIt = false;
       Serial.println(command+" comand.");
       // blink the led 
        if (command == "Open")
        {
          //turn the led on:
         digitalWrite(led, HIGH);
         Serial.println("LED High");
        delay(1);
      
       // send the information back to the server
       // that the door has been successfully opened.

     connectToServer("GET ", "android.autodoor.comli.com","/change_status.php?device_id=1&event_status=1",80);
      
  }
   else {
          // turn the led off:
          digitalWrite(led, LOW);
          Serial.println("LED Low");
         delay(1); 
        
         // send the information back to the server
       // that the door has been successfully closed.
     connectToServer("GET ", "android.autodoor.comli.com","/change_status.php?device_id=1&event_status=1",80);
       }
      
     }
   }
 delay(1);
  }

// if one second have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
   // connect to techxlab.comli.com:
   
   connectToServer("GET ", "android.autodoor.comli.com","/check_command.php?device_id=1",80);
  }

}



