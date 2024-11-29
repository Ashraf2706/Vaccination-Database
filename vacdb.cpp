// CMSC 341 - Spring 2024 - Project 4
#include "vacdb.h"
VacDB::VacDB(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    //size is prime range
    if(size < MINPRIME){
        m_currentCap = MINPRIME;
    }else if(size > MAXPRIME){
        m_currentCap = MAXPRIME;
    }else if(!isPrime(size)){
        m_currentCap = findNextPrime(size);
    }else{
        m_currentCap = size;
    }

    //Initialize the hash table
    m_hash = hash;
    m_currProbing = probing;
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_newPolicy = probing;
    m_currentTable = new Patient*[m_currentCap];
    for(int i = 0; i < m_currentCap; i++){
        m_currentTable[i] = nullptr; //set entries to nullptr to indicate empty slots
    }

    //The old table
    m_oldTable = nullptr;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = DEFPOLCY;

    m_transferIndex = 0;

}

VacDB::~VacDB(){
    //deallocate the current table
    for(int i = 0; i < m_currentCap; i++){
        delete m_currentTable[i];
    }
    delete [] m_currentTable;

    //deallocate old table
    if(m_oldTable != nullptr){
        for(int i = 0; i < m_oldCap; i++){
            delete m_oldTable[i];
        }
        delete [] m_oldTable;
    }
}

void VacDB::changeProbPolicy(prob_t policy){
    m_newPolicy = policy;
}

bool VacDB::insert(const Patient& patient){
    //Check if the patient serial number is in range
    if(patient.getSerial() < MINID || patient.getSerial() > MAXID){
        return false;
    }
    //Initial hash index
    int index = m_hash(patient.getKey()) % m_currentCap;

    int step = 0;
    int firstHash = index;

    while(m_currentTable[index] != nullptr && m_currentTable[index]->getUsed()){
        if(*m_currentTable[index] == patient){
            return false;
        }
        //Probing to find available slot
        switch (m_currProbing) {
            case LINEAR:
                index = (index + 1) % m_currentCap;
                break;
            case QUADRATIC:
                step++;
                index = (firstHash + step * step) % m_currentCap;
                break;
            case DOUBLEHASH:
                step++;
                int secondHash = 11 - (m_hash(patient.getKey()) % 11);
                index = (firstHash + step * secondHash) % m_currentCap;
                break;
        }
        //when we loop round means table is full
        if(index == firstHash){
            break;
        }
    }
    if(m_currentTable[index] == nullptr || !m_currentTable[index]->getUsed()) {
        m_currentTable[index] = new Patient(patient);
        m_currentSize++;

        //Check if rehashing is needed
        if (lambda() > 0.5f || deletedRatio() > 0.8f) {
            rehash();
            return true;
        }
        return true;
    }
    return false;
}

bool VacDB::remove(Patient patient){
    //Removing from current table
    bool found = false;
    int index = m_hash(patient.getKey()) % m_currentCap;
    int step = 0;
    int firstHash = index;

    //Patient foundPatient = getPatient(patient.getKey(), patient.getSerial());
    while(m_currentTable[index] != nullptr){
        if(m_currentTable[index]->getUsed() && m_currentTable[index]->getKey() == patient.getKey()
            && m_currentTable[index]->getSerial() ==  patient.getSerial()){
            m_currentTable[index]->setUsed(false); //marked as deleted
            found = true;
            m_currNumDeleted++;
            m_currentSize--;
            return true;
        }

        //Probing to find available slot
        switch (m_currProbing) {
            case LINEAR:
                index = (index + 1) % m_currentCap;
                break;
            case QUADRATIC:
                step++;
                index = (firstHash + step * step) % m_currentCap;
                break;
            case DOUBLEHASH:
                step++;
                int secondHash = 11 - (m_hash(patient.getKey()) % 11);
                index = (firstHash + step * secondHash) % m_currentCap;
                break;
        }
        //when we loop round means table is full
        if(index == firstHash){
            break;
        }
    }
    // Remove for old table
    if(!found && m_oldTable != nullptr){
        index = m_hash(patient.getKey()) % m_oldCap;
        step = 0;
        firstHash = index;
        while(m_oldTable[index] != nullptr){
            if(m_oldTable[index]->getUsed() && m_oldTable[index]->getKey() == patient.getKey()
               && m_oldTable[index]->getSerial() ==  patient.getSerial()){
                m_oldTable[index]->setUsed(false);
                found = true;
                m_oldNumDeleted++;
                m_oldSize--;
                if(deletedRatio() > 0.8 || lambda() > 0.5){
                    rehash();
                }
                return true;
            }
            //Probing to find available slot
            switch (m_oldProbing) {
                case LINEAR:
                    index = (index + 1) % m_oldCap;
                    break;
                case QUADRATIC:
                    step++;
                    index = (firstHash + step * step) % m_oldCap;
                    break;
                case DOUBLEHASH:
                    step++;
                    int secondHash = 11 - (m_hash(patient.getKey()) % 11);
                    index = (firstHash + step * secondHash) % m_oldCap;
                    break;
            }
            //when we loop round means table is full
            if(index == firstHash){
                break;
            }
        }
    }
    return false; //patient is not found in either table
}

const Patient VacDB::getPatient(string name, int serial) const{
    const Patient* patient = searchPatient(m_currentTable, m_currentCap, name, serial);
    if(patient){
        return *patient; //copy of found patient
    }
    //if not in current table, search old table
    if(m_oldTable != nullptr){
        patient = searchPatient(m_oldTable, m_oldCap, name, serial);
        if(patient){
            return *patient; //copy of found patient
        }
    }
    //Return empty patient object if not found
    return Patient();
}

bool VacDB::updateSerialNumber(Patient patient, int serial){
    if(serial < MINID || serial > MAXID)
        return false;
    //Update current table first
    if(updatePatient(m_currentTable, m_currentCap, patient.getKey(), patient.getSerial(), serial))
        return true;

    //if not found in current, update in old table
    if(m_oldTable != nullptr && updatePatient(m_oldTable, m_oldCap, patient.getKey(), patient.getSerial(), serial))
        return true;

    return false; //Patient not found in either table
}

float VacDB::lambda() const {
    return (1.0f * m_currentSize) / m_currentCap;
}

float VacDB::deletedRatio() const {
    return (1.0f * m_currNumDeleted) / m_currentSize;
}

void VacDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

bool VacDB::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int VacDB::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) {
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0)
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Patient* patient ) {
    if ((patient != nullptr) && !(patient->getKey().empty()))
        sout << patient->getKey() << " (" << patient->getSerial() << ", "<< patient->getUsed() <<  ")";
    else
        sout << "";
    return sout;
}

bool operator==(const Patient& lhs, const Patient& rhs){
    // since the uniqueness of an object is defined by name and serial number
    // the equality operator considers only those two criteria
    return ((lhs.getKey() == rhs.getKey()) && (lhs.getSerial() == rhs.getSerial()));
}

bool Patient::operator==(const Patient* & rhs){
    // since the uniqueness of an object is defined by name and serial number
    // the equality operator considers only those two criteria
    return ((getKey() == rhs->getKey()) && (getSerial() == rhs->getSerial()));
}

void VacDB::rehash() {

    int transferLimit = floor(0.25 * m_oldSize);
    if(transferLimit == 0){
        transferLimit = 1;
    }

    bool transfer = true;
    int count = 0;
    //Rehash if there are elements in table
    if(m_currentSize - m_currNumDeleted == 0)
        return;

    int newCapacity = findNextPrime((m_currentSize - m_currNumDeleted)*4);
    Patient** newTable = new Patient * [newCapacity];
    for(int i = 0; i < newCapacity; i++){
        newTable[i] = nullptr;
    }
    //Temporary hold data
    m_oldTable = m_currentTable;
    m_oldCap = m_currentCap;

    //Update current table
    m_currentTable = newTable;
    m_currentCap = newCapacity;
    m_currentSize = 0;
    m_currNumDeleted = 0;



    //Reinsert from old to new table
    for (int i = 0; i < m_oldCap; i++){
        if(m_oldTable[i] && m_oldTable[i]->getUsed()){
            insert(*m_oldTable[i]); //Re-insert the patient into new table
            m_oldTable[i]->setUsed(false);
            m_oldNumDeleted++;
            count++;
        }
        if(count == transferLimit){
            transfer = false;
            break;
        }
    }
    if(transfer){
        delete[] m_oldTable;
        m_oldTable = nullptr;
        m_oldCap = 0;
        m_oldSize = 0;
        m_oldNumDeleted = 0;
    }
}

const Patient* VacDB::searchPatient(Patient **table, int capacity, const std::string &key, int serial) const {
    for(int i = 0; i < capacity; i++){
        const Patient* patient = table[i];
        if(patient && patient->getUsed() && patient->getKey() == key && patient->getSerial() == serial){
            return patient;
        }
    }
    return nullptr;
}
bool VacDB::updatePatient(Patient **table, int capacity, const std::string &key, int serial, int newSerial) {
    for(int i = 0; i < capacity; i++){
        Patient *patientPtr = table[i];
        if(patientPtr && patientPtr->getUsed() && patientPtr->getKey() == key && patientPtr->getSerial() == serial){
            patientPtr->setSerial(newSerial); //Update serial number
            return true;
        }
    }
    return false; //Patient not found
}
