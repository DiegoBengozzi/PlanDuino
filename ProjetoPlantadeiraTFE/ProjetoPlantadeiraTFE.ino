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
  byte estadoPistao;//variavel informa se a plantadeira esta esguida ou abaixada
  
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
  //int notas[] = { 524, 588, 660, 699, 785, 881, 989 }; // possivelmente usadas no sistema de alerta de erros e falhas
  
  void setup(){            
    pinMode(alerta, OUTPUT);    //INICIALIZACAO DA PORTA
    pinMode(chds, INPUT);
    pinMode(chdi, INPUT);
    pinMode(ches, INPUT);
    pinMode(chei, INPUT);
    pinMode(botao, INPUT);
    pinMode(b2, INPUT);
    pinMode(b3, INPUT);
    pinMode(pistao, INPUT);    
    Wire.begin();
    RTC.begin();
    lcd.begin(16, 2);
    ajusteRTC();    
    telaApresentacao();    
  }
  
  void loop(){ 
    
    unsigned long tempoFinal = millis() + 1000;
    
    contador = 0;
    test1 = 0;
    test2 = 0;
    test3 = 0;
    test4 = 0;
    
    byte estadoAnteriorChds = digitalRead(chds);
    byte estadoAnteriorChdi = digitalRead(chdi);
    byte estadoAnteriorChes = digitalRead(ches);
    byte estadoAnteriorChei = digitalRead(chei);
    byte estadoAnteriorBotao = digitalRead(botao);
    byte estadoAnteriorB2 = digitalRead(b2);
    byte estadoAnteriorB3 = digitalRead(b3);  
    estadoPistao = digitalRead(pistao);
    
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
        estadoAnteriorChds = estadoAtualChds;
      }
     
      if(estadoAtualChdi != estadoAnteriorChdi){ 
        test2++;
        estadoAnteriorChdi = estadoAtualChdi;
      } 
  
      if(estadoAtualChes != estadoAnteriorChes){ 
        test3++;
        estadoAnteriorChes = estadoAtualChes;
      }
      
      if(estadoAtualChei != estadoAnteriorChei){ 
        test4++;
        estadoAnteriorChei = estadoAtualChei;
      } 
      
      //Botao central - confirmacao/altera tela de configuracao/paraliza alarmes
      if(estadoAtualBotao != estadoAnteriorBotao && estadoAtualBotao == HIGH ){
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
           case 10:       
             break; 
           case 11:
             telaInicio = 1;       
             break; 
           case 12:
             telaInicio = 1;       
             break;             
           case 13:
             telaInicio = 1;       
             break;             
           case 14:
             telaInicio = 1;
             break;            
        }
        estadoAnteriorBotao = estadoAtualBotao;
      } 
      
      //Botao adicionar/acresentar/lado direito/avancar tela
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
             EEPROM.write(0, numeroLinha);
             EEPROM.write(1, espacamento);
             EEPROM.write(2, furoDisco);
             telaInicio = 4;
             break;
           case 9:
             telaInicio = 0;
             break;
           case 10:       
             break; 
           case 11:       
             break; 
           case 12:       
             break;             
           case 13:       
             break;             
           case 14:       
             break;             
         }
        estadoAnteriorB2 = estadoAtualB2;
      }
      
      //Botao decrementar/lado esquerdo/voltar tela
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
           case 10:       
             break; 
           case 11:       
             break; 
           case 12:       
             break;             
           case 13:       
             break;             
           case 14:       
             break;             
        }
        estadoAnteriorB3 = estadoAtualB3;
      } 
    }//fim while
                     /*
                      *TESTE PRINCIPAIS E REGRAS DE NEGOCIO.
                      */
                      
    //PISTAO ABAIXADO / MODO PLANTIO    
    if(estadoPistao == LOW){
      distanciaTotal = distanciaTotal + ((test1 + test3) / 2);//calculo media distacia percorrida
      float valorPorcento = test1/100*25;          
      if((test1 - test3 > valorPorcento)||(test3 - test1 > valorPorcento)){
        //diferença maior q 10% de velocidade
        telaInicio = 14;
      } 
      if(test1 == 0 || test3 ==0){
        //sensores de velocidade parados
         telaInicio = 12;         
      }
      if(test2 == 0 || test4 ==0){
        //sensores de semente parados        
        telaInicio = 12;
      }                                        
      kmh = ((2.5/20)*((test1 + test3) / 2))*3.6;          //calculo de km/h
      if(kmh > 10.0){                               //teste de velocidade alta
        alert(1);                                
      }else{ 
        alert(0);
      }     
    }
    
    //Pistao Erguido  - Modo Trasporte
    if(estadoPistao == HIGH){
      if(test1 > 0 || test2 > 0 || test3 > 0 || test4 > 0){
      //Erro movimento dos sensores com a plantadeira erguida
      telaInicio = 13;
    }
  }
  
    // 2,5 metros percorridos com 20 pulsos
    hectare = ( ( ( (2.5/20) * distanciaTotal)*(numeroLinha * espacamento/100) ) / 10000 ); 
    alqueire = ( ( ( (2.5/20) * distanciaTotal)*(numeroLinha * espacamento/100) ) / 24200 );
    
    telaLCD(telaInicio);
  }
  //fim loop
  
  void telaLCD(int aux){ //Metodo de escola da TELA
    lcd.clear(); 
    switch(aux){
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Hect.  -  Alq. >"); 
        lcd.setCursor(0, 1);     
        lcd.print(hectare, 2);       
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
        alert(0);
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
        lcd.print("-"); 
        lcd.print(test2);
        lcd.print("-"); 
        lcd.print(test4);
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
        lcd.print("    Plantando   ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");        
        break;
        
      case 10:
        lcd.setCursor(0, 0);
        lcd.print("     Parado     ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");        
        break;
        
      case 11:
        lcd.setCursor(0, 0);
        lcd.print("  Falha / Erro  ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        alert(1);        
        break;
        
      case 12:
        lcd.setCursor(0, 0);
        lcd.print("     Alerta     ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        alert(1);        
        break;
      
      case 13:
        lcd.setCursor(0, 0);
        lcd.print("   Transporte   ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        alert(1);        
        break;
      
      case 14:
        lcd.setCursor(0, 0);
        lcd.print("   Diferenca    ");
        lcd.setCursor(0, 1);
        lcd.print("  Lado A != B   ");
        alert(1);        
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
