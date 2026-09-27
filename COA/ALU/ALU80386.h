#ifndef ALU_80386_H
#define ALU_80386_H

#include <iostream>
#include <cstdint>

class ALU80386 {
private:
    uint32_t eax;
    uint32_t edx;
    bool zeroFlag;
    bool carryFlag;
    bool signFlag;

public:
    ALU80386();

    void setEAX(uint32_t val);
    void setEDX(uint32_t val);
    uint32_t getEAX() const;
    uint32_t getEDX() const;

    bool getZF() const;
    bool getCF() const;
    bool getSF() const;
    bool getOF() const;

    uint32_t executeADD(bool verbose = false);
    uint32_t executeSUB(bool verbose = false);

    uint32_t computeScoreAdd(uint32_t currentScore, uint32_t increment, bool verbose = false);
    uint32_t computeHealthDamage(uint32_t currentHealth, uint32_t damage, bool& isGameOver, bool verbose = false);
};

#endif // ALU_80386_H
