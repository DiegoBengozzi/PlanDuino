  //Projeto PlanDuino por Diego Bengozzi de Brito
  #include <Wire.h> //biblioteca para o relogio I2C
  #include <RTClib.h>
  #include <LiquidCrystal.h>
  #include <EEPROM.h> // incluir a biblioteca 
  
  LiquidCrystal lcd(12, 11, 5, 4, 3, 2);                           
  RTC_DS1307 RTC;
  
  const byte alerta = 9;// saida para sistema de alerta
  
  const byte chds = 8;//teste na protobord CHave Direito Superior
  const byte chdi = 6;//CHave Direito Inferior  
  const byte ches = 13;//CHave Esquerda Superior
  const byte chei = 10;//CHave Esqueda Inferior
  
  const byte botao = 7;//botão do painel para passa as telas
  const int b2 = A0; //botão adicionar
  const int b3 = A1; //botão decramentar
  
  const int pistao = A2; //'botao' q determina se a plantadeira esta erguida ou abaixada
  byte estadoPistao;
  
  byte telaInicio = 9;      //VARIAVEIS                                              
  long contador = 0;
  long distanciaTotal =0;
  float hectare = 0;
  float alqueire = 0;
  float kmh = 0;
  int test1;
  int test2;
  int test3;
  int test4;

  
  byte numeroLinha = EEPROM.read(0);
  byte espacamento = EEPROM.read(1);
  byte furoDisco = EEPROM.read(2);
  
  //int notas[] = { 524, 588, 660, 699, 785, 881, 989 };
  
  
  void setup(){
    
         //INICIALIZACAO DA PORTA
    pinMode(alerta, OUTPUT);
    pinMode(chds, INPUT);
    pinMode(chdi, INPUT);
    pinMode(ches, INPUT);
    pinMode(chei, INPUT);
    pinMode(botao, INPUT);
    pinMode(b2, INPUT);
    pinMode(b3, INPUT);
    pinMode(pistao, INPUT);
    Serial.begin(9600);
    Wire.begin();
    RTC.begin();
    lcd.begin(16, 2);
    ajusteRTC();
    telaApresentacao();
    
  }
  
  void loop(){ 
    
    unsigned long tempoFinal = millis() + 500;
    
    byte estadoAnteriorBotao = digitalRead(botao);
    byte estadoAnteriorB2 = digitalRead(b2);
    byte estadoAnteriorB3 = digitalRead(b3);
    
    estadoPistao = digitalRead(pistao);
    
    byte estadoAnteriorChds = digitalRead(chds);
    byte estadoAnteriorChdi = digitalRead(chdi);
    byte estadoAnteriorChes = digitalRead(ches);
    byte estadoAnteriorChei = digitalRead(chei);
      
    contador = 0;
    test1 = 0;
    test2 = 0;
    test3 = 0;
    test4 = 0;
    
    while(tempoFinal > millis()){
      
      byte estadoAtualChds = digitalRead(chds);
      byte estadoAtualChdi = digitalRead(chdi);
      byte estadoAtualChes = digitalRead(ches);
      byte estadoAtualChei = digitalRead(chei);
      
      byte estadoAtualBotao = digitalRead(botao);
       byte estadoAtualB2 = digitalRead(b2);
        byte estadoAtualB3 = digitalRead(b3);
      
  
      if(estadoAtualChds != estadoAnteriorChds){ // teste de pulsos da chave optica
        test1++;
        contador++;
        distanciaTotal++;
        estadoAnteriorChds = estadoAtualChds;
      }
     
      if(estadoAtualChdi != estadoAnteriorChdi){ // teste de pulsos da chave optica   
        test2++;
        contador++;
        distanciaTotal++;
        estadoAnteriorChdi = estadoAtualChdi;
      } 
  
      if(estadoAtualChes != estadoAnteriorChes){ // teste de pulsos da chave optica
        test3++;
        contador++;
        distanciaTotal++;
        estadoAnteriorChes = estadoAtualChes;
      }
      
      if(estadoAtualChei != estadoAnteriorChei){ // teste de pulsos da chave optica    
        test4++;
        contador++;
        distanciaTotal++;
        estadoAnteriorChei = estadoAtualChei;
      }
  
         
      if(estadoAtualBotao != estadoAnteriorBotao && estadoAtualBotao == HIGH ){// teste o botao q passa as telas do sistema
        switch(telaInicio){
           case 0:
             telaInicio = 4;
             break;
           case 1:
             telaInicio = 4;
             break;
           case 2:
             telaInicio = 4;
             break;
           case 3:
             telaInicio = 4;
             break;
           case 4:
             break;
           case 5:
             telaInicio = 6;
             break;
           case 6:
             telaInicio = 7;
             break;
           case 7:
             telaInicio = 8;
             break;
           case 8:
             break;
           case 9:
            digitalWrite(alerta, LOW);
             break;
             
        }
        estadoAnteriorBotao = estadoAtualBotao;
      } 
      
      //botao adicionar
       if(estadoAtualB2 != estadoAnteriorB2 && estadoAtualB2 == HIGH ){
         switch(telaInicio){
           case 0:
             telaInicio++;
             break;
           case 1:
             telaInicio++;
             break;
           case 2:
             telaInicio++;
             break;
           case 3:
             break;
           case 4:
             telaInicio++;
             break;
           case 5:
             numeroLinha++;
             break;
           case 6:
             espacamento++;
             break;
           case 7:
             furoDisco = furoDisco + 5;
             break;
           case 8:
             //numeroLinha = 5;
             //espacamento = 75;
             //furoDisco = 28;
             EEPROM.write(0, numeroLinha);
             EEPROM.write(1, espacamento);
             EEPROM.write(2, furoDisco);
             telaInicio = 4;
             break;
           case 9:
             telaInicio = 0;
             break;
         }
        estadoAnteriorB2 = estadoAtualB2;
      }
      
      //botao decrementar
      if(estadoAtualB3 != estadoAnteriorB3 && estadoAtualB3 == HIGH ){
        switch(telaInicio){
           case 0:
             telaInicio = 9;
             break;
           case 1:
             telaInicio--;
             break;
           case 2:
             telaInicio--;
             break;
           case 3:
             telaInicio--;
             break;
           case 4:
             telaInicio--;
             break;
           case 5:
             numeroLinha--;
             break;
           case 6:
             espacamento--;
             break;
           case 7:
             furoDisco--;
             break;
           case 8:
             numeroLinha = EEPROM.read(0);
             espacamento = EEPROM.read(1);
             furoDisco = EEPROM.read(2);
             telaInicio = 9;
             break;
           case 9:
             break;           
        }
        estadoAnteriorB3 = estadoAtualB3;
      } 
    }//fim while
                                                                                                       /*
                                                                                                          TESTE PRINCIPAIS E REGRAS DE NEGOCIO.
                                                                                                      */
        
        if(estadoPistao == LOW){//PISTAO ABAIXADO / MODO PLANTIO
          //distanciaTotal = distanciaTotal + ((test1 + test3) / 2);//calculo media distacia percorrida
          
          float valorPorcento = test1/100*10;          
          if((test1 - test3 > valorPorcento)||(test3 - test1 > valorPorcento)){
            //diferença maior q 10% de velocidade
          } 
          if(test1 == 0 || test3 ==0){
            //sensores de velocidade parados         
          }
          if(test2 == 0 || test4 ==0){
            //sensores de semente parados        
          } 
     
                                                  
          kmh = ((2.5/20)*contador)*3.6;          //calculo de km/h
          if(kmh > 10.0){                         //teste de velocidade alta
            alert(1);                                
          }else{ 
            alert(0);
          }     
        }
        
        if(estadoPistao == HIGH){//Pistao Erguido  - Modo Trasporte
          if(test1 > 0 || test2 > 0 || test3 > 0 || test4 > 0){
            //Erro movimento dos sensores com a plantadeira erguida
          }
          
        }
        
             

        
        
        
        
        
        
    telaLCD(telaInicio);
  }                                                                           //fim loop
  
  void telaLCD(int aux){ //Metodo de escola da TELA
    lcd.clear(); 
    switch(aux){
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Hect.  -  Alq. >"); // 20 pulsos = 2,5 metros percorridos = 10000 m2 = CM²
        lcd.setCursor(0, 1);
        hectare = ( ( ( (2.5/20) * distanciaTotal)*(numeroLinha * espacamento/100) ) / 10000 ); // ok aparentemente funcionando
        lcd.print(hectare, 2);
        alqueire = ( ( ( (2.5/20) * distanciaTotal)*(numeroLinha * espacamento/100) ) / 24200 ); // ok aparentemente funcionando
        lcd.setCursor(8, 1);
        lcd.print(alqueire, 2);
        break;
        
      case 1: //tela de informacao da data e hora
        lcd.setCursor(0, 0);
        lcd.print("<");
        hora();
        lcd.print(">"); 
        lcd.setCursor(0, 1);
        data();
        break;
      
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("< Veloc. Km/h  >");
        lcd.setCursor(0, 1);
        lcd.print(kmh);
        if(kmh > 6){
          lcd.print("  Alta");
        }else{ 
          lcd.print("  Normal");
        }
        break;    
        
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("< Semente Metro");
        lcd.setCursor(0, 1);
        lcd.print(distanciaTotal);     
        break;
        
      case 4:
        lcd.setCursor(0, 0);
        lcd.print("  CONFIGURACAO  ");
        lcd.setCursor(0, 1);
        lcd.print("VOLTAR        OK");
        break;
     
     case 5:
        lcd.setCursor(0, 0);
        lcd.print("Numero de Linhas");
        lcd.setCursor(0, 1);
        lcd.print("-     ");
        lcd.print(numeroLinha);
        lcd.print("     +");
        break;
        
      case 6:
         lcd.setCursor(0, 0);
         lcd.print("Espacamento - Cm");
         lcd.setCursor(0, 1);
         lcd.print("-     ");
         lcd.print(espacamento);
         lcd.print("     +");
         break;
  
      case 7:
         lcd.setCursor(0, 0);
         lcd.print("Nun. Furos Disco");
         lcd.setCursor(0, 1);
         lcd.print("-     ");
         lcd.print(furoDisco);
         lcd.print("     +");
         break;
  
      case 8:
        lcd.setCursor(0, 0);
        lcd.print("     SALVAR?    ");
        lcd.setCursor(0, 1);
        lcd.print("NAO          SIM");
        break;
        
      case 9:
        lcd.setCursor(0, 0);
        lcd.print("Plantando...");
        lcd.setCursor(0, 1);
        lcd.print("...");        
        break;
        
      case 10:
        lcd.setCursor(0, 0);
        lcd.print("Parado...");
        lcd.setCursor(0, 1);
        lcd.print("Plant. Erguida");        
        break;
        
      case 11:
        lcd.setCursor(0, 0);
        lcd.print("Alerta de ");
        lcd.setCursor(0, 1);
        lcd.print("Velocidade");        
        break;
        
      case 12:
        lcd.setCursor(0, 0);
        lcd.print("Erro...");
        lcd.setCursor(0, 1);
        lcd.print("...");        
        break;
      
      case 13:
        lcd.setCursor(0, 0);
        lcd.print("Exesso de");
        lcd.setCursor(0, 1);
        lcd.print("Semente");        
        break;
      
      case 14:
        lcd.setCursor(0, 0);
        lcd.print("Diferença");
        lcd.setCursor(0, 1);
        lcd.print("Lado A - B");        
        break;
      
      case 15:
        lcd.setCursor(0, 0);
        lcd.print("linha1");
        lcd.setCursor(0, 1);
        lcd.print("linha2");        
        break;
        case 16:
        lcd.setCursor(0, 0);
        lcd.print("linha1");
        lcd.setCursor(0, 1);
        lcd.print("linha2");        
        break;
        case 17:
        lcd.setCursor(0, 0);
        lcd.print("linha1");
        lcd.setCursor(0, 1);
        lcd.print("linha2");        
        break;
        case 18:
        lcd.setCursor(0, 0);
        lcd.print("linha1");
        lcd.setCursor(0, 1);
        lcd.print("linha2");        
        break;

        
   
    }
  }
  void alert(byte n){ //chamado quando algo estiver errado...saida para disposistivos
    switch(n){
      case 0:
        digitalWrite(alerta, LOW);
        break;
      case 1:
        digitalWrite(alerta, HIGH);
        break;
    }
//    for (int i = 0; i < 7; i++) {
//      tone(alerta, notas[i], 1000);
//      delay(1000);
//    }
//    delay(1000);
  }
  
   
  void telaApresentacao(){ // Tela de Inicio do Sistema
    do{
      lcd.setCursor(0,0);   
      lcd.write("Monitor de plantio PLANDUINO!");
      lcd.setCursor(0,1);
      lcd.print("Linhas:");
      lcd.print(numeroLinha);
      lcd.print(" -Espac:");
      lcd.print(espacamento);
      lcd.print(" -Furos:");
      lcd.print(furoDisco);
      lcd.scrollDisplayLeft();
      telaInicio++;
      delay(400);
    }while(telaInicio < 19);
    lcd.clear();
    telaInicio = 0;
  }
  
  void data(){ // Exibe a Data na Tela
     DateTime now = RTC.now();
     lcd.print("Data:");
     lcd.print(now.day());
     lcd.print("/");
     lcd.print(now.month());
     lcd.print("/");
     lcd.print(now.year());
  }
  
  void hora(){ // Exibe a hora na tela
    DateTime now = RTC.now();
    lcd.print("Hora:");
    lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute());
    lcd.print(":");
    lcd.print(now.second());
  }
  
  void ajusteRTC(){ //Se o RTC nao estive executando ele configura a hora da compilacao
      if (!RTC.isrunning()) { //encaso da hora estiver errada somente tirar o ponto de explamacao e compilar
          Serial.println("RTC is NOT running!");
          RTC.adjust(DateTime(__DATE__, __TIME__));
      }
  }

