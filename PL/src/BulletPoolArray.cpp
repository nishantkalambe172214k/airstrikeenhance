#include "BulletPoolArray.h"
#include <iostream>

BulletPoolArray::BulletPoolArray() : nextBulletID(1) { reset(); }

void BulletPoolArray::reset() {
  for (int i = 0; i < MAX_BULLETS; ++i) {
    bullets[i].bulletID = 0;
    bullets[i].x = 0.0f;
    bullets[i].y = 0.0f;
    bullets[i].vx = 0.0f;
    bullets[i].vy = 0.0f;
    bullets[i].damage = 0;
    bullets[i].active = false;
  }
}

int BulletPoolArray::fireBullet(float x, float y, float vx, float vy,
                                int damage) {
  for (int i = 0; i < MAX_BULLETS; ++i) {
    if (!bullets[i].active) {
      bullets[i].bulletID = nextBulletID++;
      bullets[i].x = x;
      bullets[i].y = y;
      bullets[i].vx = vx;
      bullets[i].vy = vy;
      bullets[i].damage = damage;
      bullets[i].active = true;
      return bullets[i].bulletID;
    }
  }
  return -1;
}
// drv -TRAVERSAL + BULLET POOL

void BulletPoolArray::updateBullets(float minX, float maxX, float minY,
                                    float maxY) {
  for (int i = 0; i < MAX_BULLETS; ++i) {
    if (bullets[i].active) {
      bullets[i].x += bullets[i].vx;
      bullets[i].y += bullets[i].vy;

      if (bullets[i].x < minX || bullets[i].x > maxX || bullets[i].y < minY ||
          bullets[i].y > maxY) {
        bullets[i].active = false;
      }
    }
  }
}
// SR11 BULLET ARRAY BOUNDARY CHECK

int BulletPoolArray::searchBullet(int bulletID) const {
  for (int i = 0; i < MAX_BULLETS; ++i) {
    if (bullets[i].active && bullets[i].bulletID == bulletID) {
      return i;
    }
  }
  return -1;
}

bool BulletPoolArray::deactivateBullet(int bulletID) {
  int index = searchBullet(bulletID);
  if (index != -1) {
    bullets[index].active = false;
    return true;
  }
  return false;
}

void BulletPoolArray::displayActiveBullets() const {
  int activeCount = 0;
  for (int i = 0; i < MAX_BULLETS; ++i) {
    if (bullets[i].active) {
      activeCount++;
      std::cout << "Slot [" << i << "] ID=" << bullets[i].bulletID << " Pos=("
                << bullets[i].x << ", " << bullets[i].y << ")"
                << " Vel=(" << bullets[i].vx << ", " << bullets[i].vy << ")"
                << " Dmg=" << bullets[i].damage << "\n";
    }
  }
  if (activeCount == 0) {
    std::cout << "(No active bullets in pool)\n";
  }
}

int BulletPoolArray::getActiveCount() const {
  int count = 0;
  for (int i = 0; i < MAX_BULLETS; ++i) {
    if (bullets[i].active) {
      count++;
    }
  }
  return count;
}

int BulletPoolArray::getCapacity() const { return MAX_BULLETS; }

Bullet *BulletPoolArray::getBullets() { return bullets; }

const Bullet *BulletPoolArray::getBullets() const { return bullets; }
