 //CMSC 341 - Spring 2024 - Project 4
#include "vacdb.h"
#include <math.h>
#include <random>
#include <vector>
#include <algorithm>
#include <ctime>     //used to get the current time
// We can use the Random class to generate the test data randomly!
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
class Tester{
public:
    bool testInsertion();
    bool testFindErrorCase();
    bool testFindNonCollidingKeys();
    bool testFindCollidingKeysNoRehash();
    bool removeNonCollidingKeys();
    bool removeCollidingKeysNoRehash();
    bool testRehashAfterInsertions();
    bool testRehashDueToLoadFactor();
    bool testRehashAfterRemoval();
    bool testRehashDueToDeleteRatio();
    bool testChangeProbingPolicy();
};

unsigned int hashCode(const string str);

string namesDB[6] = {"john", "serina", "mike", "celina", "alexander", "jessica"};

int main(){
    Tester tester;
    {
        cout << "Testing the insertion operation in the hash table: ";
        if(tester.testInsertion())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the find operation for an error case: " ;
        if(tester.testFindErrorCase())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the find operation with several non-colliding keys: " ;
        if(tester.testFindNonCollidingKeys())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the find operation with several colliding keys without triggering a rehash: ";
        if(tester.testFindCollidingKeysNoRehash())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the remove operation with a few non-colliding keys: ";
        if(tester.removeNonCollidingKeys())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the remove operation with a number of colliding keys without triggering a rehash: ";
        if(tester.removeCollidingKeysNoRehash())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the rehashing is triggered after a descent number of data insertion: ";
        if(tester.testRehashAfterInsertions())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the rehash completion after triggering rehash due to load factor: ";
        if(tester.testRehashDueToLoadFactor())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the rehash is triggered after a descent number of data removal: ";
        if(tester.testRehashAfterRemoval())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing the rehash completion after triggering rehash due to delete ratio: ";
        if(tester.testRehashDueToDeleteRatio())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
        cout << "Testing change in the probing policy: ";
        if(tester.testChangeProbingPolicy())
            cout << "PASSED!" << endl;
        else
            cout << "FAILED!" << endl;
    }

    return 0;
}
unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for (unsigned int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}

bool Tester::testInsertion() {
    bool result = true;
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);

    // for non colliding patients
    vector<Patient> nonCollidingPatients;
    for (int i = 0; i < 1; i++) {
        string name = namesDB[RndName.getRandNum()];
        int serial = RndID.getRandNum();
        Patient newPatient(name, serial, true);
        int index = (hashCode(newPatient.getKey())) % vacdb.m_currentCap;
        vacdb.insert(newPatient);
        result = result && vacdb.m_currentTable[index]->getUsed();
        result = result && (vacdb.m_currentSize == 1) && (vacdb.m_currNumDeleted == 0)
                 && (vacdb.m_oldTable == nullptr) && (vacdb.m_oldSize == 0) && (vacdb.m_oldNumDeleted == 0);
        nonCollidingPatients.push_back(newPatient);
    }
    //for colliding patients
    vector<Patient> collidingPatients;
    for(int i = 0; i < 60; i++){
        string name = namesDB[RndName.getRandNum()];
        int serial = RndID.getRandNum();
        Patient newPatient(name, serial, true);
        if(!vacdb.insert(newPatient)){
            cout << "Insertion failed for patient: " << newPatient.getKey() << endl;
            result = false;
        }

        collidingPatients.push_back(newPatient);
    }
    //Verify if all patients can be retrieved correctly
    for (unsigned int i = 0; i < nonCollidingPatients.size(); i++) {
        Patient& patient = nonCollidingPatients[i];
        Patient retrieved = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if (retrieved.getKey().empty() || retrieved.getSerial() != patient.getSerial()) {
            cout << "Error retrieving non-colliding patient: " << patient.getKey() << endl;
            result = false;
        }
    }
    //Verify if all colliding patients can be retrieved correctly
    for (unsigned int i = 0; i < collidingPatients.size(); i++) {
        Patient &patient = collidingPatients[i];
        Patient retrieved = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if (retrieved.getKey().empty() || retrieved.getSerial() != patient.getSerial()) {
            cout << "Error retrieving colliding patient: " << patient.getKey() << " with serial " << patient.getSerial()
                 << endl;
            result = false;
        }
    }

    return result;
}

bool Tester::testFindErrorCase() {
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);

    for (int i = 0; i < 10; i++) {
        string name = namesDB[RndName.getRandNum()];
        int serial = RndID.getRandNum();
        Patient newPatient(name, serial, true);
        vacdb.insert(newPatient);
    }
    //Check if invalid patient is in database
    if(vacdb.getPatient("Ashraf", 2706).getKey() != ""){
        cout << "Error: Ashraf was found in the database." << endl;
        return false;
    }
    return true;
}

bool Tester::testFindNonCollidingKeys() {
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    bool allFound = true;
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);

    // for non-colliding patients
    vector<Patient> nonCollidingPatients;
    for (int i = 0; i < 10; i++) {
        string name = namesDB[RndName.getRandNum()];
        int serial = RndID.getRandNum();
        Patient newPatient(name, serial, true);
        vacdb.insert(newPatient);
        nonCollidingPatients.push_back(newPatient);
    }
    for(unsigned int i = 0; i < nonCollidingPatients.size(); i++){
        Patient& patient = nonCollidingPatients[i];
        Patient found = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if(found.getKey().empty() || found.getSerial() != patient.getSerial()){
            allFound = false;
            cout << "Failed to find patient" << endl;
        }
    }
    return allFound;
}

bool Tester::testFindCollidingKeysNoRehash() {
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    bool allFound = true;
    VacDB vacdb(MINPRIME, hashCode, LINEAR);

    // for colliding patients (insertion)
    vector<Patient> collidingPatients;
    for (int i = 0; i < 50; i++) {
        string name = namesDB[RndName.getRandNum()];
        int serial =  RndID.getRandNum(); // patients have the same serial number
        Patient newPatient(name, serial, true);
        vacdb.insert(newPatient);
        collidingPatients.push_back(newPatient);
    }
    //Check whether patient can be found
    for(unsigned int i = 0; i < collidingPatients.size(); i++){
        Patient& patient = collidingPatients[i];
        Patient found = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if(found.getKey().empty() || found.getSerial() != patient.getSerial()){
            allFound = false;
            cout << "Failed to find patient" << endl;
        }
    }
    return allFound;
}

bool Tester::removeNonCollidingKeys() {
    bool allRemoved = true;
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array

    // for non-colliding patients
    vector<Patient> nonCollidingPatients;
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
    for (int i = 0; i < 10; i++) {
        string name = namesDB[RndName.getRandNum()];
        int serial =  RndID.getRandNum();
        Patient newPatient(name, serial, true);
        vacdb.insert(newPatient);
        nonCollidingPatients.push_back(newPatient);
    }
    // Removing each patient and verifying the status immediately
    for (unsigned int i = 0; i < nonCollidingPatients.size(); i++) {
        Patient& patient = nonCollidingPatients[i];
        if (!vacdb.remove(patient)) {
            cout << "Failed to remove patient: " << patient.getKey() << endl;
            allRemoved = false;
        } else {
            // Immediately check if the table spot is correctly marked as empty or deleted
            Patient found = vacdb.getPatient(patient.getKey(), patient.getSerial());
            if (!found.getKey().empty()) {
                cout << "Patient still found after removal: " << patient.getKey() << endl;
                allRemoved = false;
            }
        }
    }
    return allRemoved;
}

bool Tester::removeCollidingKeysNoRehash() {
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    bool allRemoved = true;
    VacDB vacdb(MINPRIME, hashCode, LINEAR);

    // for colliding patients
    vector<Patient> collidingPatients;
    for (int i = 0; i < 50; i++) {
        string name = namesDB[RndName.getRandNum()];
        int serial =  RndID.getRandNum();
        Patient newPatient(name, serial, true);
        vacdb.insert(newPatient);
        collidingPatients.push_back(newPatient);
    }
    //removing patients and ensuring if they are still able to seen
    for(unsigned int i = 0; i < collidingPatients.size(); i++){
        Patient& patient = collidingPatients[i];
        vacdb.remove(patient);
        Patient found = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if(!found.getKey().empty()){
            allRemoved = false;
            cout << "Failed to remove patient: " << patient.getKey() << endl;
        }
    }
    return allRemoved;
}

bool Tester::testRehashAfterInsertions() {
    bool result = true;
    vector<Patient> patients;
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);

    //insert patients
    for (int i = 0; i < 150; i++) {
        Patient newPatient = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
        vacdb.insert(newPatient);
        patients.push_back(newPatient);
    }
    // ensure all items are in the table
    bool allFound = false;
    for (unsigned int i = 0; i < patients.size(); i++) {
        Patient& patient = patients[i];
        Patient found = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if(found.getKey().empty() || found.getSerial() != patient.getSerial()){
            allFound = true;
            result = result && allFound;
        }
    }
    return result;
}

bool Tester::testRehashDueToLoadFactor() {
    bool res = true;
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array

    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);

    //insert patients
    for(int i = 0; i < 52; i++){
        Patient newPatient = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
        vacdb.insert(newPatient);

        if(vacdb.m_currentSize <= 50 && vacdb.m_oldTable == nullptr){
            res = res && (vacdb.m_currentSize == i + 1);
        }else{
            res = res && (vacdb.m_oldTable != nullptr);
        }
    }
    return res;
}

bool Tester::testRehashAfterRemoval() {
    bool result = true;
    vector<Patient> patients;
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);

    //insert patients
    for(int i = 0; i < 120; i++){
        Patient newPatient = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
        vacdb.insert(newPatient);
        patients.push_back(newPatient);
    }
    //removing 80% of patients
    for(int i = 0; i < 96; i++) {
        Patient& patient = patients[i];
        Patient found = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if(found.getKey().empty() || found.getSerial() != patient.getSerial()){
            result = result && vacdb.remove(patients[i]);
        }
    }
    //check if rehashing has been triggered and validate remaining patients
    for (int i = 96; i < 120; i++){
        Patient found = vacdb.getPatient(patients[i].getKey(), patients[i].getSerial());
        if (found.getKey().empty() || found.getSerial() != patients[i].getSerial()) {
            //cout << "Rehash error: Patient data mismatch after rehashing." << endl;
            result = false;
        }
    }
    return result;
}

bool Tester::testRehashDueToDeleteRatio() {
    vector<Patient> patients;
    bool result = true;
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);
    for(int i = 0; i < 40; i++) {
        Patient newPatient = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
        vacdb.insert(newPatient);
        patients.push_back(newPatient);
    }
    //Removing 80%
    int index =0;
    for(; index <= 40 * 0.8; index++){
        result = result && vacdb.remove(patients[index]);
    }
    // checks if all patients are transferred to new table and compares size to remaining data
    result = result && ( vacdb.m_oldTable == nullptr) && (vacdb.m_currentTable != nullptr) && (vacdb.m_currentSize == (40 * 0.2) -1);

    for (; index < 40*0.2; index++){
        result = result && (patients[index] == vacdb.getPatient(patients[index].getKey(), patients[index].getSerial()));
    }
    return result;
}

bool Tester::testChangeProbingPolicy() {
    vector<Patient> patients;
    bool result = true;
    Random RndID(MINID, MAXID);
    Random RndName(0, 5);// selects one from the namesDB array
    VacDB vacdb(MINPRIME, hashCode, DOUBLEHASH);

    //Insert initial set of patients with default probing policy
    for(int i = 0; i < 50; i++) {
        Patient newPatient = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
        vacdb.insert(newPatient);
        patients.push_back(newPatient);
    }
    //Changing probing policy and insert more patients
    vacdb.changeProbPolicy(QUADRATIC);
    for(int i = 0; i < 50; i++){
        Patient newPatient = Patient(namesDB[RndName.getRandNum()], RndID.getRandNum(), true);
        vacdb.insert(newPatient);
        patients.push_back(newPatient);
    }

    //Verify all patients are still retrievable
    for (Patient& patient : patients) {
        Patient retrieved = vacdb.getPatient(patient.getKey(), patient.getSerial());
        if (retrieved.getKey().empty() || retrieved.getSerial() != patient.getSerial()) {
            cout << "Error: Patient not found or wrong data after changing probing policy." << endl;
            result = false;
        }
    }
    return result;
}