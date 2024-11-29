// CMSC 341 - Spring 2024 - Project 4
#ifndef VACDB_H
#define VACDB_H
#include <iostream>
#include <string>
#include "math.h"
using namespace std;
const int MINID = 1000;     // serial number
const int MAXID = 9999;     // serial number
const int MINPRIME = 101;   // Min size for hash table
const int MAXPRIME = 99991; // Max size for hash table
typedef unsigned int (*hash_fn)(string); // declaration of hash function
enum prob_t {QUADRATIC, DOUBLEHASH, LINEAR}; // types of collision handling policy
#define DEFPOLCY QUADRATIC
class Grader;
class Tester;
class VacDB;
class Patient{
public:
    friend class Tester;
    friend class Grader;
    friend class VacDB;
    Patient(string name="", int serial=0, bool used=false){
        m_name = name; m_serial = serial; m_used = used;
    }
    string getKey() const {return m_name;}
    int getSerial() const {return m_serial;}
    bool getUsed() const {return m_used;}
    void setKey(string key) {m_name=key;}
    void setSerial(int serial) {m_serial = serial;}
    void setUsed(bool used) {m_used=used;}
    const Patient& operator=(const Patient& rhs){
        if (this != &rhs){
            m_name = rhs.m_name;
            m_serial = rhs.m_serial;
            m_used = rhs.m_used;
        }
        return *this;
    }
    // Overloaded insertion operator
    friend ostream& operator<<(ostream& sout, const Patient* patient );
    // Overloaded equality operator
    friend bool operator==(const Patient& lhs, const Patient& rhs);
    bool operator==(const Patient* & rhs);
private:
    string m_name;
    int m_serial;
    // the following variable is used for lazy delete scheme in hash table
    // if it is set to false, it means the bucket in the hash table is free for insert
    // if it is set to true, it means the bucket contains live data, and we cannot overwrite it
    bool m_used;
};
class VacDB{
public:
    friend class Grader;
    friend class Tester;
    VacDB(int size, hash_fn hash, prob_t probing);
    ~VacDB();
    // Returns Load factor of the new table
    float lambda() const;
    // Returns the ratio of deleted slots in the new table
    float deletedRatio() const;
    // insert only happens in the new table
    bool insert(const Patient& patient);
    // remove can happen from either table
    bool remove(Patient patient);
    // find can happen in either table
    const Patient getPatient(string name, int serial) const;
    // update the information
    bool updateSerialNumber(Patient patient, int serial);
    void changeProbPolicy(prob_t policy);
    void dump() const;

private:
    hash_fn    m_hash;          // hash function
    prob_t     m_newPolicy;     // stores the change of policy request

    Patient**  m_currentTable;  // hash table
    int        m_currentCap;    // hash table size (capacity)
    int        m_currentSize;   // current number of entries
    // m_currentSize includes deleted entries
    int        m_currNumDeleted;// number of deleted entries
    prob_t     m_currProbing;   // collision handling policy

    Patient**  m_oldTable;      // hash table
    int        m_oldCap;        // hash table size (capacity)
    int        m_oldSize;       // current number of entries
    // m_oldSize includes deleted entries
    int        m_oldNumDeleted; // number of deleted entries
    prob_t     m_oldProbing;    // collision handling policy

    int        m_transferIndex; // this can be used as a temporary place holder
    // during incremental transfer to scanning the table

    //private helper functions
    bool isPrime(int number);
    int findNextPrime(int current);

    /******************************************
    * Private function declarations go here! *
    ******************************************/
    void rehash();
    const Patient* searchPatient(Patient** table, int capacity, const string& key, int serial) const;
    bool updatePatient(Patient** table, int capacity, const string& key, int serial, int newSerial);

};
#endif