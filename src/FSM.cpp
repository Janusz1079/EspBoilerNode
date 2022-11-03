#include <Arduino.h>
#include "FSM.h"

#define quote(x) #x

void State::setContext(Context *context)
{
    this->context_=context;
}

Context::Context(State* state) : state_(nullptr)
{
    this->transitionToState(state);
}

void Context::transitionToState(State* state)
{
    Serial.print("\n\t* Contex transirion to new State: ");
    Serial.print(quote(state));
    Serial.print(" *\t\n");
    if(this->state_ != nullptr)
        delete state_;
    this->state_ = state;
    this->state_->setContext(this);
}

void Context::IsrTimer()
{
    this->state_->IsrTimer();
}

void Context::IsrBufferFull()
{
    this->state_->IsrBufferFull();
}

void Idle::IsrTimer()
{
    this->context_->transitionToState(new DataReceive);    
}

void Idle::IsrBufferFull()
{
    
}

void DataRead::IsrTimer()
{
    Serial.print("\nOdczytuje wartości czujnikiow znajdujacych sie na liscie\n");
}

void DataRead::IsrBufferFull()
{
    Serial.println("\nBufforDanychPełny");
    this->context_->transitionToState(new DataSend);
}

void DataSend::IsrTimer()
{

}

void DataSend::IsrBufferFull()
{

}

void DataReceive::IsrTimer()
{

}

void DataReceive::IsrBufferFull()
{

}