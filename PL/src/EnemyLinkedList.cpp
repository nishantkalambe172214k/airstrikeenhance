#include "EnemyLinkedList.h"
#include <iostream>

EnemyLinkedList::EnemyLinkedList() : head(nullptr), count(0) {}

EnemyLinkedList::~EnemyLinkedList() { clear(); }

void EnemyLinkedList::addEnemy(int id, float x, float y, float vx, float vy,
                               int health, int maxHealth, int scoreValue,
                               int collisionDamage, float angle) {
  Enemy e;
  e.id = id;
  e.x = x;
  e.y = y;
  e.vx = vx;
  e.vy = vy;
  e.health = health;
  e.maxHealth = maxHealth;
  e.scoreValue = scoreValue;
  e.collisionDamage = collisionDamage;
  e.angle = angle;
  addEnemy(e);
}

void EnemyLinkedList::addEnemy(const Enemy &enemy) {
  EnemyNode *newNode = new EnemyNode(enemy);
  newNode->next = head;
  head = newNode;
  count++;
}

bool EnemyLinkedList::removeEnemy(int id) {
  if (head == nullptr)
    return false;

  if (head->data.id == id) {
    EnemyNode *temp = head;
    head = head->next;
    delete temp;
    count--;
    return true;
  }

  // SR11 LINKED LIST

  EnemyNode *prev = head;
  EnemyNode *curr = head->next;
  while (curr != nullptr) {
    if (curr->data.id == id) {
      prev->next = curr->next;
      delete curr;
      count--;
      return true;
    }
    prev = curr;
    curr = curr->next;
  }
  return false;
}

void EnemyLinkedList::clear() {
  EnemyNode *curr = head;
  while (curr != nullptr) {
    EnemyNode *nextNode = curr->next;
    delete curr;
    curr = nextNode;
  }
  head = nullptr;
  count = 0;
}

EnemyNode *EnemyLinkedList::getHead() { return head; }

const EnemyNode *EnemyLinkedList::getHead() const { return head; }

int EnemyLinkedList::getCount() const { return count; }

bool EnemyLinkedList::isEmpty() const { return head == nullptr; }
