#include <Arduino.h>
#include "FSM.h"

#define quote(x) #x
#define ONE_WIRE_BUS 4

void State::setContext(Context *context)
{
    this->context_=context;
}

//----- Class CONTEXT definition -----
Context::Context(State* state) : state_(nullptr), oneWire(nullptr), sensors(nullptr), sensor1_buff(nullptr), sensor2_buff(nullptr)
{
    DeviceAddress sensor1;
    DeviceAddress sensor2;
    oneWire = new OneWire(ONE_WIRE_BUS);
    sensors = new DallasTemperature(oneWire);
    //ISR_INTERRUPT flags;
    ISR_REGISTER = ISR_ZERO;
    sensor1_buff = new Buffer(); 
    sensor2_buff = new Buffer();

    this->sensors->begin();
    if(this->sensors->getAddress(this->sensor1, 0))
    {
        Serial.print("\nFound device ");
        Serial.print("with address: ");
        this->printAddress(this->sensor1);

        Serial.println();
    }

    if(this->sensors->getAddress(this->sensor2, 1))
    {
        Serial.print("\nFound device ");
        Serial.print("with address: ");
        this->printAddress(this->sensor2);
        Serial.println();
    }

    this->transitionToState(state);
}
Context::~Context()
{
    delete state_;

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
    Serial.print(this->context_->sensor1_buff->avr());
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