#ifndef ENEMY_LINKED_LIST_H
#define ENEMY_LINKED_LIST_H

struct Enemy {
  int id;
  float x;
  float y;
  float vx;
  float vy;
  int health;
  int maxHealth;
  int scoreValue;
  int collisionDamage;
  float angle;
};

struct EnemyNode {
  Enemy data;
  EnemyNode *next;

  EnemyNode(const Enemy &enemy) : data(enemy), next(nullptr) {}
};

class EnemyLinkedList {
private:
  EnemyNode *head;
  int count;

public:
  EnemyLinkedList();
  ~EnemyLinkedList();

  void addEnemy(int id, float x, float y, float vx, float vy, int health,
                int maxHealth, int scoreValue, int collisionDamage,
                float angle = 0.0f);
  void addEnemy(const Enemy &enemy);
  bool removeEnemy(int id);
  void clear();

  EnemyNode *getHead();
  const EnemyNode *getHead() const;
  int getCount() const;
  bool isEmpty() const;
};

#endif // ENEMY_LINKED_LIST_H
