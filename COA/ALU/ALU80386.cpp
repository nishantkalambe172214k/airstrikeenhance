#include "ALU80386.h"
#include <iostream>

ALU80386::ALU80386()
    : eax(0), edx(0), zeroFlag(false), carryFlag(false), signFlag(false) {}

void ALU80386::setEAX(uint32_t val) { eax = val; }
void ALU80386::setEDX(uint32_t val) { edx = val; }
uint32_t ALU80386::getEAX() const { return eax; }
uint32_t ALU80386::getEDX() const { return edx; }

bool ALU80386::getZF() const { return zeroFlag; }
bool ALU80386::getCF() const { return carryFlag; }
bool ALU80386::getSF() const { return signFlag; }
bool ALU80386::getOF() const { return false; }

uint32_t ALU80386::executeADD(bool verbose) {
  uint64_t sum = static_cast<uint64_t>(eax) + static_cast<uint64_t>(edx);
  carryFlag = (sum > 0xFFFFFFFFULL);
  eax = static_cast<uint32_t>(sum);
  zeroFlag = (eax == 0);
  signFlag = false;

  if (verbose) {
    std::cout << "[ALU ADD] " << (sum - edx) << " + " << edx << " = " << eax
              << " (ZF=" << zeroFlag << ", CF=" << carryFlag << ")\n";
  }
  return eax;
}

uint32_t ALU80386::executeSUB(bool verbose) {
  carryFlag = (eax < edx);
  zeroFlag = (eax == edx);
  signFlag = (eax < edx);

  uint32_t prev = eax;
  if (eax >= edx) {
    eax -= edx;
  } else {
    eax = 0;
  }

  if (verbose) {
    std::cout << "[ALU SUB] " << prev << " - " << edx << " = " << eax
              << " (ZF=" << zeroFlag << ", SF=" << signFlag << ")\n";
  }
  return eax;
}

uint32_t ALU80386::computeScoreAdd(uint32_t currentScore, uint32_t increment,
                                   bool verbose) {
  setEAX(currentScore);
  setEDX(increment);
  return executeADD(verbose);
}

uint32_t ALU80386::computeHealthDamage(uint32_t currentHealth, uint32_t damage,
                                       bool &isGameOver, bool verbose) {
  setEAX(currentHealth);
  setEDX(damage);
  uint32_t remaining = executeSUB(verbose);

  if (zeroFlag || signFlag || carryFlag) {
    isGameOver = true;
    remaining = 0;
  } else {
    isGameOver = false;
  }
  return remaining;
}
