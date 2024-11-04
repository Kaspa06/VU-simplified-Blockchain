# Supaprastintos blokų grandinės (blockchain) kūrimas

Šios užduoties tikslas – sukurti supaprastintą blokų grandinę (angl. blockchain), kurios duomenų sluoksnio struktūra pavaizduota žemiau:
![image](https://github.com/user-attachments/assets/7a5c5534-75df-4f36-a55e-9321f404bb1a)

## Turinys

-   [Įžanga](#Įžanga)
-   [Pagrindinės Funkcijos](#Pagrindinės-Funkcijos)
-   [Projekto Struktūra](#Projekto-Struktūra)
-   [Diegimas ir Paleidimas](#Diegimas-ir-Paleidimas)
-   [Naudojimas](#Naudojimas)
-   [Objektinio Programavimo Praktikos](#Objektinio-Programavimo-Praktikos)

---

### Įžanga

Projekte siekiama sukurti supaprastintą blokų grandinę, kurioje centralizuotai valdomi visi blokai ir transakcijos.

---

### Pagrindinės Funkcijos

1. **Blokų Generavimas**: Blokai generuojami, kai surenkama pakankamai transakcijų. Kiekvienas blokas turi ankstesnio bloko maišos kodą, dabartinį maišos kodą ir Merkle Root Hash.
2. **Transakcijų Tikrinimas**: Prieš įtraukiant transakciją į bloką, tikrinama, ar siuntėjo balansui pakanka lėšų ir ar transakcijos maišos kodas sutampa su nurodytu.
3. **Proof-of-Work (PoW)**: Siekiant užtikrinti blokų vientisumą ir apsaugoti nuo manipuliacijos, kiekvieno bloko generavimas vykdomas per PoW algoritmą.
4. **Merkle Root Hash**: Naudojama supaprastinta Merkle medžio versija – vietoj dvejetainio Merkle medžio suformuojama maišos reikšmė, kuri susiejama su visų bloko transakcijų ID.
5. **Centralizuotas Blokų Valdymas**: Vartotojai ir transakcijos yra generuojami ir valdomi per centralizuotą mazgą, kuris saugo ir apdoroja visą informaciją.

---

### Projekto Struktūra

-   **`block.cpp` / `block.h`**: Bloko struktūra ir funkcijos, atsakingos už bloko generavimą, maišos skaičiavimą ir PoW algoritmą.

    ```cpp
    Block::Block(const std::string& previousHash, const std::vector<Transaction>& transactions, int difficultyTarget)
    : previousHash(previousHash), transactions(transactions), difficultyTarget(difficultyTarget) {
    this->timestamp = std::to_string(std::time(0)); // Initialize timestamp with current Unix time
    this->nonce = 0;
    MerkleTree merkleTree(transactions);
    this->merkleRootHash = merkleTree.createMerkleRootHash(); // Use existing MerkleTree class
    this->version = "1.0"; // Example version, change as needed
    }
    ```

-   **`Transaction.cpp` / `transactions.h`**: Transakcijų klasės ir metodai, įskaitant transakcijos ID, siuntėjo ir gavėjo viešuosius raktus bei sumos valdymą.

    ```cpp
    std::string Transaction::getTransactionID() const { return transactionID; }
    std::string Transaction::getSenderPublicKey() const { return senderPublicKey; }
    ```

-   **`main.cpp`**: Pagrindinė programa, generuojanti vartotojus, transakcijas ir blokų grandinę.

    ```cpp
    int main() {
        std::vector<User> users = generateUsers(userNumber);
        std::vector<Transaction> transactionPool = generateTransactions(transactionNumber, users);
    }
    ```

-   **`user.cpp` / `user.h`**: Klasės ir metodai, valdomi vartotojų duomenis ir balansus.

---

### Diegimas ir Paleidimas

1. **Kompiliavimas**:

    ```bash
    g++ main.cpp block.cpp hash.cpp Transaction.cpp user.cpp -o blockchain
    ```

2. **Paleidimas**:
    ```bash
    ./blockchain
    ```

---

### Naudojimas

Paleidus programą, bus rodoma pagrindinio meniu sąsaja su parinktimis:

-   **1. generateUsers(userNumber)**: Kuria vartotojus ir priskiria pradines balansų reikšmes.
-   **2. generateTransactions(transactionNumber, users)**: Leidžia kurti naujas transakcijas tarp vartotojų.
-   **3. mineBlockchain(transactionPool, users)**: Suformuoja naują bloką ir patikrina transakcijų vientisumą.
-   **4. findBlock(blockID, blockchain)**: Randa ir pateikia informaciją apie pasirinktą bloką.
-   **5. findTransaction(transactionID, blockchain)**: Pateikia informaciją apie pasirinktą transakciją.
-   **6. findUser(userName, users)**: Parodo vartotojo balansą ir operacijų istoriją.

---

### Transakcijų ir blokų atvaizdavimas

Sukurti blokai saugomi `blockchain.txt` faile, kur yra saugoma informacija:

1. **Block Hash**:
2. **Merkle Root Hash**
3. **Timestamp**
4. **Difficulty Target**
5. **Version**
6. **Nonce**
7. **Number of Transactions**
8. **Transactions**

![image](https://github.com/user-attachments/assets/f57d6713-6d5f-4b42-be04-e5f58e0fb516)

Atmestos transackcijos dėl nepakankamų lėšų ar dėl neteisingai įvesto vartotojo vardo saugomos faile `failedTransactions.txt`
![image](https://github.com/user-attachments/assets/6c7306a1-b3c4-41d8-aadb-d12ea3b7fb43)


---

### Objektinio Programavimo Praktikos

Siekiant užtikrinti blokų grandinės stabilumą ir saugumą, projekte taikomos šios objektinio programavimo praktikos:

-   **Enkapsuliacija**: Kiekvienos klasės duomenys yra pasiekiami tik per viešus metodus, užtikrinant saugią prieigą prie duomenų.

    ```cpp
    class User {
        std::string name;
        std::string publicKey;
        int balance;

     public:
        User(const std::string& name, const std::string& publicKey, int balance);
        std::string getName() const;
        int getBalance() const;
    };
    ```

-   **Konstruktoriai ir RAII Idėja**: Kiekvienas objektas turi konstruktorinį metodą, kuris automatiškai užkrauna visus reikiamus išteklius. RAII principas užtikrina, kad resursai bus atlaisvinami, kai objektas nebebus reikalingas.

---
### Papildomos užduotys

**Lygiagretus blokų kasimo proceso realizavimas v0.2 versijoje (+0.5 balo)**:

Pridėta lygiagretaus blokų kasimo proceso realizacija naudojant OpenMP biblioteką. Ši realizacija leidžia naudoti kelių gijų kasimo algoritmą, kuris paspartina PoW (Proof-of-Work) procesą.

```cpp
#include <omp.h>

void Block::mineBlock() {
    std::string target(difficultyTarget, '0'); // Sukuria eilutę su 'difficultyTarget' skaičiumi nulinių bitų

    bool found = false;
    #pragma omp parallel num_threads(4)
    {
        while (!found) {
            #pragma omp critical
            {
                nonce++;
                currentHash = calculateBlockHash();
                if (currentHash.substr(0, difficultyTarget) == target) {
                    blockID = currentHash;
                    found = true;
                    #pragma omp flush(found) // Užtikrina, kad visos gijos žino apie atnaujintą 'found' reikšmę
                }
                if (nonce % 100000 == 0) {
                    std::cout << "Still mining... Current Nonce: " << nonce << ", Current Hash: " << currentHash << std::endl;
                }
            }
        }
    }
}
```

Šioje versijoje:
- `#pragma omp parallel num_threads(4)` naudojama keturių gijų kasimui.
- `#pragma omp critical` užtikrina, kad vienu metu tik viena gija atlieka nonce padidinimą ir hash skaičiavimą, taip užkertant kelią konkurenciniam prieigai prie bendrų duomenų.
- `#pragma omp flush(found)` padeda sinchronizuoti `found` kintamojo reikšmę tarp visų gijų, kai rastas tinkamas hash'as.

---
