#include <Arduino.h>
#include "FSM.h"

//#define quote(x) #x


//IPAddress ipServer(192, 168, 0, 106);
WiFiClient client;

void State::setContext(Context *context)
{
    this->context_=context;
}

//----- Class CONTEXT definition -----
Context::Context(State* state) : state_(nullptr), oneWire(nullptr), sensors(nullptr), sensor1_buff(nullptr), sensor2_buff(nullptr) //client(nullptr)
{
    //DeviceAddress sensor1;
    //DeviceAddress sensor2;
    oneWire = new OneWire(ONE_WIRE_BUS);
    sensors = new DallasTemperature(oneWire);
    ISR_REGISTER = ISR_ZERO;
    sensor1_buff = new Buffer(); 
    sensor2_buff = new Buffer();
    //client = new WiFiClient();
    
    

    this->transitionToState(state);
}
Context::~Context()
{
    delete state_;

}
void Context::sendHTTP(float x, float y)
{
    if(WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        Serial.println("Conencting to the server...");
        while(!client.connect(ipServer, 80))
        {
            Serial.print(".");
        }

        Serial.println("Http begin...");
        while(!(http.begin(client, serverName))){
            Serial.print(".");
            delay(200);
        }

        Serial.println("Http connected succesfull");

        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String httpRequestData = "api_key=" + apiKey + "&water_main=" + String(x, 0) + "&water_return=" + String(y, 0)+ "";
        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);

        int httpResponseCode = http.POST(httpRequestData);
        delay(400);

        if(httpResponseCode > 0) 
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
        }
        else 
        {
            Serial.print("Error code ");
            Serial.println(httpResponseCode);
        }

        http.end();
    }
}

void Context::listNetworks()
{
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    {
        Serial.println("Couldn't get a wifi connection");
        while (true);
    }
    Serial.print("number of available networks:");
    Serial.println(numSsid);
    for (int thisNet = 0; thisNet < numSsid; thisNet++) 
    {
        Serial.print(thisNet);
        Serial.print(") ");
        Serial.print(WiFi.SSID(thisNet));
        Serial.print("\tSignal: ");
        Serial.print(WiFi.RSSI(thisNet));
        Serial.print(" dBm");
        Serial.println("\tEncryption: ");
    }
}


void Context::printAddress(DeviceAddress deviceAddress) 
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if (deviceAddress[i] < 16) Serial.print("0");
            Serial.print(deviceAddress[i], HEX);
    }
}    

void Context::transitionToState(State* state)
{
    if(this->state_ != nullptr)
    {
        this->state_->exitFunction();
        Serial.print("\n------------------------\n");
        delete this->state_;
    }
    this->state_ = state;
    state->setContext(this);
    this->state_->entryFunction();
}

void Context::entryFunction()
{
    this->state_->entryFunction();
}

void Context::isrTimer()
{
    this->state_->isrTimer();
}

void Context::isrBufferFull()
{
    this->state_->isrBufferFull();
}

void Context::isrBufferEmpty()
{
    this->state_->isrBufferEmpty();
}
void Context::loopFunction()
{
    this->state_->loopFunction();
}
void Context::exitFunction()
{
    this->state_->exitFunction();
}

//----- Class IDLE definition -----
void Idle::entryFunction()
{
   Serial.print("\n entryFunction() IDLE state -> "); 

}
void Idle::isrTimer()
{
    Serial.print(" isrTimer() IDLE state -> ");
    this->context_->transitionToState(new DataRead);    
}

void Idle::isrBufferFull()
{
    Serial.print(" isrBufferFull() IDLE state -> ");
    this->context_->transitionToState(new DataSend);
}

void Idle::exitFunction()
{
    Serial.print(" exitFunction() IDLE state ||\n");
}

//----- Class DATAREAD definition -----
void DataRead::entryFunction()
{
    Serial.print("\n entryFunction() DATAREAD state -> ");
}

void DataRead::isrTimer()
{
    Serial.print(" isrTimer() DATAREAD state -> ");
    float temp1 = 0.0;
    float temp2 = 0.0;
    this->context_->sensors->requestTemperatures();
    temp1 = this->context_->sensors->getTempC(this->context_->sensor1);
    temp2 = this->context_->sensors->getTempC(this->context_->sensor2);
    this->context_->ISR_REGISTER = this->context_->ISR_ZERO;
    this->context_->sensor1_buff->push(temp1);
    this->context_->sensor2_buff->push(temp2);
    this->context_->transitionToState(new Idle);
}

void DataRead::isrBufferFull()
{
    Serial.print(" isrBufferFull() DATAREAD state -> ");
    this->context_->transitionToState(new DataSend);
}

void DataRead::exitFunction()
{
    Serial.print(" exitFunction() DATAREAD state ||\n");
    Serial.print("Buff1 -> isEmptyFlag = ");
    Serial.print(this->context_->sensor1_buff->isFullFlag );
    Serial.print("\nBuff2 -> isEmptyFlag = ");
    Serial.print(this->context_->sensor2_buff->isFullFlag );
    Serial.print("\n");
    this->context_->temptemp = 20;
    if(this->context_->sensor1_buff->isFullFlag &&
        this->context_->sensor2_buff->isFullFlag)
    {
        Serial.print("\n\t\t\t ! Przerewanie ISR_Full_Buffer ! \n");
        this->context_->ISR_REGISTER = this->context_->ISR_BufferFull;
    }
}

void DataSend::entryFunction()
{
    Serial.print("\n entryFunction() DATASEND state -> ");
}

void DataSend::isrBufferFull()
{
    Serial.print(" isrBufferFull() DATASEND state: (");
    //this->context_->sendSensorsData();
    //Serial.print(this->context_->sensor1_buff->avr());
    this->context_->sendHTTP(this->context_->sensor1_buff->avr(), this->context_->sensor2_buff->avr());
    Serial.print(" ,");
    Serial.print(this->context_->sensor2_buff->avr());
    Serial.print(" ) -> ");
    this->context_->transitionToState(new Idle);
}

void DataSend::exitFunction()
{
    Serial.print(" exitFunction() DATASEND state ||\n");
    Serial.print("Buff1 -> isEmptyFlag = ");
    Serial.print(this->context_->sensor1_buff->isEmptyFlag );
    Serial.print("/nBuff2 -> isEmptyFlag = ");
    Serial.print(this->context_->sensor2_buff->isEmptyFlag );
    Serial.print("\n");
    if(this->context_->sensor1_buff->isEmptyFlag &&
    this->context_->sensor2_buff->isEmptyFlag)
    {   Serial.print("\n\t\t\t Zerowanie ISR_Full_Buffer \n");
        this->context_->ISR_REGISTER = this->context_->ISR_ZERO;
    }
}

void Sensor1::entryFunction()
{
    Serial.print("\n entryFunction() SENSOR1 state \n");
}

void Sensor2::entryFunction()
{
    
    Serial.print("\n entryFunction() SENSOR2 state\n");
}