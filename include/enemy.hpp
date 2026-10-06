#pragma once
#include <raylib.h>

class Enemy {
public:
	void update(float dt);

	void draw() const;

	void takeDamage(int damage);

	Vector2 getPosition() const { return m_pos; }
	Rectangle getHitbox() const { return Rectangle{ m_pos.x, m_pos.y, m_width, m_height}; }
private:
	Vector2 m_pos{ 200, 200 };
	float m_width = 30, m_height = 30;
	float m_speed = 300;
	bool m_active = true;
	int m_health = 10;
};