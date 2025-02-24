//
// main.cpp
//
// Created by Darren Bowers on 1/24/2025, following Professor Manouchehr Mohandesi's examples.
//

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std; 

// IRQ Codes
const char IRQ_Timer_Upper = 'T';
const char IRQ_Timer_Lower = 't';
const char IRQ_Keyboard_Upper = 'K';
const char IRQ_Keyboard_Lower = 'k';
const char IRQ_Network_Upper = 'N';
const char IRQ_Network_Lower = 'n';
const char IRQ_Exit_Upper = 'E';
const char IRQ_Exit_Lower = 'e';

// Interrupt Vector Table (Mapping IRQ to interrupt vector)
int interruptVectorTable[] ={10, 11, 12}; // examples for interrupt vector

// Look up Table for ISR (routine)
void (*interruptLookupTable[3])()={nullptr};

// function to simulate raising an interrupt
void raiseInterrupt(char irqKey){
    int irq=-1;

    // Map IRQ to vector index (0, 1, or 2)
    if (irqKey == IRQ_Timer_Upper || irqKey == IRQ_Timer_Lower){
        irq = 0; 

    }else if (irqKey == IRQ_Keyboard_Upper || irqKey == IRQ_Keyboard_Lower){
        irq = 1;

    }else if (irqKey == IRQ_Network_Upper || irqKey == IRQ_Network_Lower){
        irq = 2;

    }

    // Handle valid IRQ
    if (irq!=-1){
        int interruptVector = interruptVectorTable[irq];
        cout << endl << "Interrupt raised! Vector: " << interruptVector << endl;
        if (interruptLookupTable[irq] != nullptr){
            interruptLookupTable[irq](); // Call the appropriate ISR
        }
    }else{
            // Invalid key, handle situation
            cout << endl << "Invalid interrupt key! valid keys include 'T: Timer', 'K: Keyboard', 'N: Network" << endl << endl;
        }
}

// Timer Interrupt ISR
void ISR_Timer(){
    cout << "Performing Timer Interrupt" << endl;
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
    cout << "System Date and time is: " << ctime(&timenow) << endl;
}
    
// Keyboard Interrupt ISR
void ISR_Keyboard(){
    cout << "Performing Keyboard Interrupt" << endl << endl;
}

// Network Interrupt ISR
void ISR_Network(){
    cout << "Performing Network Interrupt" << endl << endl;
}

// Function to check for keyboard input (Asynchronously)
void checkForInput(atomic<bool> & interruptSignal, char& inputKey){
    while (!interruptSignal){
        if(cin>>inputKey){
            interruptSignal = true; 
        }
    }
}


int main(){
    // initialize the interrupt look up table 
    interruptLookupTable[0]=ISR_Timer;
    interruptLookupTable[1]=ISR_Keyboard;
    interruptLookupTable[2]=ISR_Network;

    // variable to store the key pressed
    char key; 

    // Atomic flag to detect when an interrupt signal has been raised

    atomic<bool> interruptSignal(false);

    // Main loop to simulate normal system operation
    while(true){
        // Create a thread to check for keyboard input asynchronously
        thread inputThread(checkForInput, ref(interruptSignal), ref(key));

        // Loop to perform normal task until interrupt is signaled
        while (!interruptSignal){
            // Perform normal task while waiting for user input
            cout << "-=- System is performing normal tasks... -=- " << endl;
            this_thread::sleep_for(chrono::seconds (2)); // simulate 2 second delay to prevent spam in terminal
        }
        // Wait for the input thread to finish
        inputThread.join();

        // Raise an interrupt base on the key pressed
        if(key==IRQ_Exit_Upper || key==IRQ_Exit_Lower){
            cout <<"Exit key pressed. Ending the program!" << endl;
            break;

        }else{
            raiseInterrupt(key);
            interruptSignal = false;
            // Reset the interrupt signal to continue normal operations
        }

    }
    
    return 0;
}
