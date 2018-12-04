#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


SoftwareSerial ESP8266(2, 3); // RX | TX
#define DEBUG true
int x1=0;
int sen1;
int sen2;
int sen3;
int sen4;

void setup()
 { 
   
  Serial.begin(9600);
  ESP8266.begin(9600);
  lcd.begin(16,2);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);

  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
 
 //EnvioDeDatos("AT+RST\r\n",2000,DEBUG); // resetea el  modulo
  EnvioDeDatos("AT+CWMODE=1\r\n",1000,DEBUG);                           // Cconfigura el modulo en Estación
  EnvioDeDatos("AT+CWJAP=\"internet\",\"\"\r\n",5000,DEBUG);        // datos de tu red.
  EnvioDeDatos("AT+CIFSR\r\n",2000,DEBUG);                              // Nos muestra nuestra Ip asignada.
  EnvioDeDatos("AT+CIPMUX=1\r\n",2000,DEBUG);                           // Configura multiples coneciones
  EnvioDeDatos("AT+CIPSERVER=1,80\r\n",2000,DEBUG);                     // Inicializa el servidor web
  EnvioDeDatos("AT+CIFSR\r\n",2000,DEBUG);  
 }

 
void loop()
 { 
  sen1=100-(analogRead(A1)*(100.0 / 1023.0));                      //Sensor de humedad
  sen2=100-(analogRead(A2)*(100.0 / 1023.0));     //EJEMPLO DE SENSOR CALIRADO DE 0 A 100 Sensor de lluvia
  sen3=(100.0*analogRead(A3)*5.0)/1023.0;   //EJEMPLO DE SENSOR CALIRADO DE 0 A 100 Sensor de temperatura
  sen4=analogRead(A4)*(100.0 / 1023.0);                      //Sensor de intensidad de luz

  // Cursor en la primera posición de la primera fila
  lcd.setCursor(0,0);
  lcd.print("S1:");
  lcd.print(sen1);
  lcd.print("%  ");   //"unidades"
  // Cursor en la 11° posición de la primera fila
  lcd.setCursor(9,0);
  lcd.print("S2:"); 
  lcd.print(sen2,1);//1 decimal
  lcd.print("%  ");   //"unidades"
  // Cursor en la primera posición de la 2° fila
  lcd.setCursor(0,1);
  lcd.print("S3:");
  lcd.print(sen3,1); //1 decimal
  lcd.print("C  "); // "unidades"
  // Cursor en la 11° posición de la 2° fila
  lcd.setCursor(9,1);
  lcd.print("S4:");
  lcd.print(sen4,1);
  lcd.print("%  ");   //"unidades"

  if(sen1>50 || sen2>50 || sen4>50 || sen3>45 )
  {
    digitalWrite(12,HIGH);
    digitalWrite(11,HIGH);
  }
  else
  {
    digitalWrite(12,LOW);
    digitalWrite(11,LOW);
  }
  
  if (ESP8266.available() >0 )                                        // se recibieron datos.
  {
    char c = ESP8266.read();                                              // lee los datos.
    Serial.print(c);                                                       // Los reenvia por el puerto hardware para que veamos que estamos recibiendo.
                                 // No dice que ha recibido una peticion.
    delay(100);
    codigoServidor();
  
  }

 }
void codigoServidor()
{ 
  delay(50);
  EnvioDeDatos("AT+CIFSR\r\n",2000,DEBUG); 
  delay(50);
  
  escribir("<!DOCTYPE HTML>");
  escribir("<html>");
  escribir("<head><title>LECTURAS ANALOGICAS.</title>");
  escribir("<meta http-equiv=\"refresh\" content=\"15\"></head>");
  escribir("<body><h1> Situacion Ambiente</h1>");
  
  
 
  escribir("Numero de Sensor: ");
  escribir( String(1));
  escribir("Humedad de Tierra: ");
  escribir(" es ");
  escribir(String(sen1));
  escribir("%");
  escribir("<br />");

  
  escribir("Numero de Sensor: ");
  escribir( String(2));
  escribir("Porcetaje de lluvia ");
  escribir(" es ");
  escribir(String(sen2));
  escribir("%");
  escribir("<br />");

  
  escribir("Numero de Sensor: ");
  escribir( String(3));
  escribir("Temperatura: ");
  escribir(" es ");
  escribir(String(sen3));
  escribir("C");
  escribir("<br />");

  
  escribir("Numero de Sensor: ");
  escribir( String(4));
  escribir("Intesidad de Luz: ");
  escribir(" es ");
  escribir(String(sen4));
  escribir("%");
  escribir("<br />");
  
  escribir("<p><em> Cada 5 minutos se muestra el valor del ambiente.</em></p></body></html>");
  

  delay(1000);
  
 // ESP8266.println("AT+CIPCLOSE=0");
  //delay(10);
  
  x1=1;
 }
void escribir(String text)
{ESP8266.print("AT+CIPSEND=0,"); 
 ESP8266.println(text.length());
 if (ESP8266.find(">")) // Si se recibe el mensaje
 {Serial.println(text);
 ESP8266.println(text); //mandamos el mensaje por el wifi 
 delay(10);
 while ( ESP8266.available() > 0 ) 
 {if ( ESP8266.find("SEND OK") ) //buscamos "ok" y luego salimos
 break; 
 }
 }
}
String EnvioDeDatos(String command, const int timeout, boolean debug)
{
    String cadena = "";
    ESP8266.print(command); // enviar el carácter leído al modulo
    long int time = millis();
    while( (time+timeout) > millis())
    {
      while(ESP8266.available())
      {
        // Muestra los Datos de salida del modulo por el seguimiento de la serie
        char c = ESP8266.read(); //Lee el proximo caracter
        cadena+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(cadena);
    }
    
    return cadena;
}
