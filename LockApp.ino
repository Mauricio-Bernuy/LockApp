#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Servo.h>

Servo servomotor;
SoftwareSerial SIM900(7,8); // RX, TX
TinyGPSPlus gps;
SoftwareSerial LMG(4,3); // RX, TX

String valor;  //Variable para indicar que llega una orden
String estado;
char x;
int pos = 0; 
//int estado=5;
int i=0;
int j=0;
String latitud;
String longitud;
String maps, datos;
float lati,longi=0;
char DAT;
char DAT_dos;
char DAT_GPS;
float flat, flon;
unsigned long age;
char coordenada_gps;
String datosSERIAL;
String stringSERIAL;
String currentNUMERO;

//autenticacion
String password;
String MasterKey="1234";
bool isLoggedIn;

//codigos de entrada
String clave_gps="GPS";
String clave_abrir="ABR";
String clave_cerrar="CER";
String clave_setup="Setup";
String clave_login="Login";
String clave_logout="Logout";
String clave_cambiar_pswd="ChgPswd";
String clave_auth = "TryAuth";

//codigos de salida
String confirm="&true";
String deny="&false";
String key_out=("&masterkey "+ MasterKey);
String authrequired = "&authrequired";

bool chk = false;
int count = 0;
int count2 = 101;

void setup()
{
  
  LMG.begin(9600);
  SIM900.begin(9600);
  //delay(10000);
  Serial.begin(9600);
  borrarSERIAL();
  isLoggedIn = false;
  
  configuracion_inicial();
  Serial.println("Sistema encendido");
  
  delay(1000);

  //servomotor.attach(10); //Pin PWM 6 del Arduino para el servo
  //servomotor.write(0);  //Inicia el servo en una posición de 90 grados
  //delay(1000);
  //servomotor.detach();
}

void loop() 
{
  
  Serial.println("esperando mensaje");
  while(true)
  {
    /*LMG.listen();
    Serial.println(LMG.available());
   while (LMG.available()>0){//count > 3000){
    gps.encode(LMG.read());
   //if(count >30000){
      Serial.println("read gps");
    if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
      count =0;
    }
   //} */
   LMG.listen();
   delay(10);
   if (LMG.available() > 0 && count2 >100){
    while(count<300){
    LMG.listen();
    //Serial.println(LMG.available());
    //Serial.println(LMG.read());
    gps.encode(LMG.read());
    //Serial.println(gps.location.lat());
    delay(1);
    if (gps.location.lat()!=0.00){
      Serial.println("location updated");
      //Serial.print("Latitude= "); 
      flat=(gps.location.lat());
      latitud=String(flat,6);
      //Serial.println(latitud);
      //Serial.print(" Longitude= "); 
      flon=gps.location.lng(),6;
      longitud=String(flon,6);
      //Serial.println(longitud);
      count =0;
      count2 = 0;
      break;
    }
    count++;
    }
    count=0;
  }
  //delay(100);
    

    //SIM900.listen();
    leer_mensaje();

    entrar();

    //detectarENTRADA();
    if (isLoggedIn == true) //if logged_in == True
    {
      detectarENTRADA();
    }
    count2++;
    // Serial.println(count2);  
  

  }
  
}

/////

/////////////////////
void configuracion_inicial()//configura codigos de lectura
{
  Serial.println("espere");
  delay(300);
  SIM900.println("AT+IPR=9600");//modo texto
  Serial.println("AT+IPR=9600");//modo texto
  delay(400);
  SIM900.println("AT+CMGF=1");//MODO TEXTO
  Serial.println("AT+CMGF=1");//MODO TEXTO
  delay(400);
  SIM900.println("AT+CMGR=?");//ACTIVAMOS CODIGO PARA RECIBIR
  Serial.println("AT+CMGR=?");//RECIBIR MENSAJE
  delay(400);
  SIM900.println("AT+CNMI=2,2,0,0");//VER MENSAJE
  Serial.println("AT+CNMI=2,2,0,0");//VER MENSAJE
  delay(400);
  SIM900.println("AT+CGNSPWR=1");//ACTIVAS GPS
  Serial.println("AT+CGNSPWR=1");//ACTIVAS GPS
  Serial.println("Configuracion terminada");
  delay(400);
}

/////////////////////////////////////////////////////////////////
void serv1()
{
  Serial.println("abrir");
  servomotor.attach(10); //Pin PWM 10 del Arduino para el servo
  for (pos = 0; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servomotor.write(pos);              // tell servo to go to position in variable 'pos'
    delay(4);                       // waits 15ms for the servo to reach the position
  }
  delay(500); 
  servomotor.detach();
}

void serv2()  
{
  Serial.println("cerrar");
  servomotor.attach(10); //Pin PWM 10 del Arduino para el servo
  for (pos = 120; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    servomotor.write(pos);              // tell servo to go to position in variable 'pos'
    delay(4);                       // waits 15ms for the servo to reach the position
  }
  delay(500); 
  servomotor.detach();
}

void abrir()
{
  if(datosSERIAL==clave_abrir)
   {
    Serial.println("codigo correcto abrir");
    if(valor!="abierto"){
      serv1();
      valor="abierto";   
      Serial.println("lectura abrir enviada");
    }else{
    Serial.println("candado ya abierto");
  }
 }
}

void cerrar()
{
  if(datosSERIAL==clave_cerrar)
  {
    Serial.println("codigo correcto cerrar");
    if(valor!="cerrado"){
      serv2();
      valor="cerrado";
      Serial.println("lectura cerrar enviada");
    }else{
      Serial.println("candado ya cerrado");
    }
  }
}

void detectarENTRADA(){
  abrir();
  cerrar();
  gps_on();
}


//////////////////////////////////////////////////////////////////////

void separar(String part1, String part2, int n){ //separa la informnación de datosSERIAL en 2 strings con una división determinada

  part1 = datosSERIAL.substring(0, n);
  part2 = datosSERIAL.substring(n);
  
}

void unir(String &unido, char &part1, char &part2) {
  unido = part1 + " " + part2;
}


/*void cambiar_pswd(){
  
  char clave[];
  char newpass[];
  separar(clave[], password, 9);
  
  if(clave==clave_cambiar_pswd)
  {
  password=newpass;  
  enviar_msg(confirm);
  Serial.println("codigo SMS correcto");
  }
}*/

int nueva_pswd(){
  
  String datos = datosSERIAL.substring(0, 7);
  String newpass = datosSERIAL.substring(8);
  //separar(datos, newpass, 9);

  if(datos==clave_cambiar_pswd){
    password=newpass;  
    enviar_msg(confirm);
    Serial.println("codigo SMS correcto");
    Serial.println("nueva password = " + password);
    return 0;
  } else{return 1;}
  
}

int authenticate(){
  String datos = datosSERIAL.substring(0, 7);
  String checkpass = datosSERIAL.substring(8);
  //Serial.println(datos);
  //Serial.println(checkpass);
  if(datos == clave_auth){
    Serial.println("Autenticando");
    if(password==checkpass || MasterKey==checkpass){
      Serial.println("Verificado");
      enviar_msg(confirm);
      return 0;
    } else if(checkpass!=password and checkpass!=MasterKey){
        Serial.println("Error de verificacion");
        enviar_msg(deny);
        return 1;
    }
  }else{return 1;}
}

void login(){
  String datos = datosSERIAL.substring(0, 5);
  String checkpass = datosSERIAL.substring(6);
  if(datos==clave_login){
    Serial.println("Loggeando");
    if(password==checkpass || MasterKey==checkpass){
      Serial.println("Verificado");
      enviar_msg(confirm);
      isLoggedIn = true;
      Serial.print("loggeado: ");
      Serial.print(isLoggedIn);
      Serial.print("\n");
    } else if(checkpass!=password and checkpass!=MasterKey){
        Serial.println("Error de verificacion");
        enviar_msg(deny);
        isLoggedIn = false;
        Serial.print("loggeado: ");
        Serial.print(isLoggedIn);
        Serial.print("\n");
    }
  }
}

void logout(){
  if(datosSERIAL==clave_logout){
    isLoggedIn = false;
    Serial.print("loggeado: ");
    Serial.print(isLoggedIn);
    Serial.print("\n");
  }
}

void enviar_msg(String msg)//envia un string por SMS a currentNUMERO
{
    Serial.println("ENVIANDO MENSAJE");
    SIM900.listen();
    SIM900.println("AT+CMGF=1");//MODO TEXTO, IMPRIME PUERTO SERIE ASCII
    delay(1000);
    SIM900.println("AT + CMGS = \"" + currentNUMERO + "\"");//COMANDO ENVIO DE MENSAJE, PARSEAR EL NUMERO TELEFONICO
    delay(1000);//tiempo de respuesta
    SIM900.print(msg);
    SIM900.println((char)26);//ponemos simbolo ascii 26, correspondiente a CTRL+Z
    delay(1000);//tiempo de respuesta
    SIM900.println();
    Serial.println("ENVIADO");
}

void entrar()//detecta codigos de setup y login
{
  if(datosSERIAL==clave_setup)
  {
    Serial.println("detectado @Setup");
    
    if(password==""){ 
      enviar_msg(key_out);
    
      while(nueva_pswd()==1){
        //Serial.print(".");
        leer_mensaje();
        nueva_pswd();   
      }
      isLoggedIn = true;
      Serial.print("loggeado: ");
      Serial.print(isLoggedIn);
      Serial.print("\n");
    } else if(password!=""){
        Serial.println("Se necesita autenticacion");
        enviar_msg(authrequired);
        //begin auth process
        while(authenticate()==1){
          //Serial.print(".");
          leer_mensaje();
          authenticate();
        }
        isLoggedIn = true;
        Serial.print("loggeado: ");
        Serial.print(isLoggedIn);
        Serial.print("\n");
    }   
  }
  
  login();

  logout();
  
  //login if possible  
    //else{
  //detectarENTRADA();
 //}
}

void envio_mensaje_gps_coordenada()
{
    SIM900.listen();
    Serial.println(" ");
    Serial.println("Datos que se enviaran");
    Serial.println("LAT=");
    Serial.println(lati,6);
    Serial.println("LONG=");
    Serial.println(longi,6);
    String latitud= String(lati,6);//obtenemos latitud
    String longitud= String(longi,6);//obtenemos longitud
    //String mapa= "https://www.google.com/maps?q="; //obtenemos link
    String datos= latitud + "," + longitud;//obtenemos url
    Serial.println(datos);
    SIM900.println("AT+CMGF=1");//MODO TEXTO, IMPRIME PUERTO SERIE ASCII
    delay(1000);
    SIM900.println("AT + CMGS = \"+51943415196\"");//COMANDO ENVIO DE MENSAJE
    delay(1000);//tiempo de respuesta
    SIM900.println(datos);
    SIM900.println((char)26);//ponemos simbolo ascii 26, correspondiente a CTRL+Z
    delay(1000);//tiempo de respuesta
    SIM900.println();
    Serial.print("ENVIADO");
}

////////////////////////////////////////////////////////////////

void activacion_gps()
{
    if(latitud!=""){
    String datos= latitud + "," + longitud;//obtenemos mensaje
    Serial.println(datos);
    enviar_msg(datos);
    
    } else {Serial.println("location not yet updated");}
}

void gps_on()
{
  if(datosSERIAL==clave_gps)
  {
    int count2 = 101;
    Serial.println("codigo correcto gps");
    activacion_gps();
    Serial.println("lectura gps enviada");
    //lati,longi=0;//datos borrados
  }
}


void leer_mensaje()//Detecta la llegada de un mensaje, decodifica el numero que lo envia, detecta un arroba de comando y decodifica el comando
{
  SIM900.listen();
  delay(150);
  borrarSERIAL();
  //datosSERIAL =Serial.read();
  //Serial.println(datosSERIAL);
  if(SIM900.available()>0)
  {
    stringSERIAL = SIM900.readString();
    Serial.println("Lectura serial:");
    Serial.println(stringSERIAL);
    if (stringSERIAL.substring(9,10)== "+"){
      currentNUMERO = stringSERIAL.substring(9, 21);
      Serial.println("numero recibido:");
      Serial.println(currentNUMERO);
    
      if (stringSERIAL.substring(50,51)== "@"){
        datosSERIAL = stringSERIAL.substring(51);
        datosSERIAL.remove(datosSERIAL.length()-1);
        datosSERIAL.remove(datosSERIAL.length()-1);
      
      } else{datosSERIAL = "error de codificacion";}
    
      Serial.println("mensaje recibido:");
      Serial.println(datosSERIAL);
    }
  }
}

void borrarSERIAL()//Limpia los strings obtenidos en leer_mensaje() (necesario para evitar bugs y loops)
{        
  datosSERIAL = '\0';
  stringSERIAL = '\0';
  currentNUMERO = '\0';
}
