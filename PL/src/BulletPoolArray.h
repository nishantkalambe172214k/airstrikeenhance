#ifndef BULLET_POOL_ARRAY_H
#define BULLET_POOL_ARRAY_H

const int MAX_BULLETS = 64;

struct Bullet {
  int bulletID;
  float x;
  float y;
  float vx;
  float vy;
  int damage;
  bool active;
};

class BulletPoolArray {
private:
  Bullet bullets[MAX_BULLETS];
  int nextBulletID;

public:
  BulletPoolArray();

  int fireBullet(float x, float y, float vx, float vy, int damage);
  void updateBullets(float minX, float maxX, float minY, float maxY);
  int searchBullet(int bulletID) const;
  bool deactivateBullet(int bulletID);
  void displayActiveBullets() const;
  int getActiveCount() const;
  int getCapacity() const;
  Bullet *getBullets();
  const Bullet *getBullets() const;
  void reset();
};

#endif // BULLET_POOL_ARRAY_H
