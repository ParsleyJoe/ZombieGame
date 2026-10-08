#pragma once

#include <raylib.h>
#include <raymath.h>

#include <optional>
#include <functional>

class Bullet {
public:
	Bullet() = delete;
	Bullet(float damage, Vector2 velocity, Vector2 pos)
		: m_damage(damage), m_velocity(velocity), m_pos(pos)
	{
	}

	void update(float dt) {
		if (!m_active) { return; }
		m_pos.x += m_velocity.x * dt;
		m_pos.y += m_velocity.y * dt;
	}

	void draw() const {
		if (!m_active) { return; }
		DrawCircle(m_pos.x, m_pos.y, 8, Color{ 255, 200, 50, 40 });
		DrawCircle(m_pos.x, m_pos.y, m_radius, m_color);
	}

	Rectangle getHitbox() const {
		return Rectangle{ m_pos.x - m_radius, m_pos.y - m_radius, m_radius * 2.0f, m_radius * 2.0f };
	}

	bool getActive() const {
		return m_active;
	}
	void setActive(bool active) {
		m_active = active;
	}
	int getDamage() {
		return m_damage;
	}

private:
	float m_damage;
	Vector2 m_velocity;
	Vector2 m_pos;
	int m_radius = 5;
	Color m_color = YELLOW;
	bool m_active = true;
};

/// <summary>
///  GUN CODE
/// </summary>
struct GunStats {
	float damage;
	float spread;
	float fireRate;
	float bulletSpeed;
	int bulletCount;
};

class Gun {
public:
	Gun() = default;

	explicit Gun(GunStats stats)
		: m_stats(stats)
	{
	}

	void update(float dt) {
		m_fireTimer -= dt;
	}
	std::vector<Bullet> shoot(float dt, Vector2 dir, Vector2 pos) {
		std::vector<Bullet> bullets{};
		// return empty vector
		if (m_fireTimer > 0.0f)
			return bullets;

		m_fireTimer = 1.0f / m_stats.fireRate;
		for (int i = 0; i < m_stats.bulletCount; ++i) {
			float angle = 0.0f;
			if (m_stats.bulletCount > 1) {
				float t = static_cast<float>(i) / (m_stats.bulletCount - 1);

				angle = Lerp(-m_stats.spread, m_stats.spread, t);
			}
			else {
				angle = static_cast<float>(
					GetRandomValue(
						static_cast<int>(-m_stats.spread * 1000),
						static_cast<int>(m_stats.spread * 1000)) ) / 1000.0f;
			}

			Vector2 bulletDir = Vector2Rotate(dir, angle);
			Bullet blt{ m_stats.damage, bulletDir * m_stats.bulletSpeed, pos };
			bullets.emplace_back(blt);
		}
		return bullets;
	}

private:
	GunStats m_stats{ 4, 2, 3, 700, 20 };
	float m_fireTimer = 0.0f;
};

/// <summary>
/// PLAYER CODE
/// </summary>
class Player
{
public:
	Player(std::function<void(Bullet)> spawnBullet)
		: m_spawnBullets(spawnBullet)
	{
		GunStats pistol{
			.damage = 10,
			.spread = 0.0f,
			.fireRate = 4.0f,
			.bulletSpeed = 500.0f,
			.bulletCount = 1
		};
		GunStats shotgun{
			.damage = 4,
			.spread = 0.4f,
			.fireRate = 1.0f,
			.bulletSpeed = 600.0f,
			.bulletCount = 6
		};
		GunStats smg{
			.damage = 3,
			.spread = 0.15f,
			.fireRate = 12.0f,
			.bulletSpeed = 800.0f,
			.bulletCount = 1
		};

		m_guns.push_back(Gun{ pistol });
		m_guns.push_back(Gun{ shotgun });
		m_guns.push_back(Gun{ smg });
	}
	void update(float dt, Vector2 mouseWorldPos)
	{
		if (!m_alive) { return; }
		this->move(dt);
		for (auto& gun : m_guns) {
			// Because updat() is just fireTimer -= dt we can update them all 
			// to allow dmc/ultrakill style switching
			gun.update(dt);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 dir = Vector2Normalize(mouseWorldPos - m_pos);
			auto bullets = m_guns[m_currentGunIndex].shoot(dt, dir, m_pos);
			for (auto& blt : bullets) {
				m_spawnBullets(blt);
			}
		}

		if (IsKeyPressed(KEY_ONE)) {
			m_currentGunIndex = 0;
		}
		if (IsKeyPressed(KEY_TWO)) {
			m_currentGunIndex = 1;
		}
		if (IsKeyPressed(KEY_THREE)) {
			m_currentGunIndex = 2;
		}
	}

	// Not really push its like teleport by dx, dy
	void push(float dx, float dy) {
		m_pos.x += dx;
		m_pos.y += dy;
	}

	void takeDamage(int damage) {
		m_health -= damage;

		if (m_health <= 0) {
			m_alive = false;
		}
	}

	void draw() const {
		if (!m_alive) { return; }
		//DrawRectangle(m_pos.x, m_pos.y, m_width, m_height, m_color);

		Vector2 center{
			m_pos.x + m_width / 2.0f,
			m_pos.y + m_height / 2.0f };
		DrawCircleV(center, 25, RAYWHITE);
		DrawCircleV(center, 17, Color{ 45, 45, 48, 255 });

		// You know what this kinda looks cool in world space rn
		float gunTypeX = GetScreenWidth() * 0.8f;
		float gunTypeY = GetScreenHeight() * 0.2f;
		const char* gunName;
		switch (m_currentGunIndex) {
		case 0:
			gunName = "Pistol";
			break;
		case 1:
			gunName = "Shotgun";
			break;
		case 2:
			gunName = "SMG";
			break;
		default:
			gunName = "DEFAULT CASE";
			break;
		}
		int fontSize = 30;
		float offset = MeasureText(gunName, fontSize);
		DrawText(gunName, gunTypeX - offset, gunTypeY, fontSize, RAYWHITE);
	}

	int getHealth() const {
		return m_health;
	}
	int getMaxHealth() const {
		return m_maxHealth;
	}

	Vector2 getPosition() const {
		return m_pos;
	}
	Rectangle getHitbox() const {
		return m_hitbox;
	}
private:
	void move(float dt)
	{
		if (IsKeyDown(KEY_A)) {
			m_pos.x -= m_speed * dt;
		}
		if (IsKeyDown(KEY_D)) {
			m_pos.x += m_speed * dt;
		}
		if (IsKeyDown(KEY_W)) {
			m_pos.y -= m_speed * dt;
		}
		if (IsKeyDown(KEY_S)) {
			m_pos.y += m_speed * dt;
		}

		m_hitbox.x = m_pos.x;
		m_hitbox.y = m_pos.y;
	}

	Vector2 m_pos{ 400, 300 };
	Rectangle m_hitbox{ 400, 300, 50, 50 };
	float m_speed = 300.0;
	int m_width = 50, m_height = 50;
	Color m_color = RAYWHITE;

	std::vector<Gun> m_guns;
	int m_currentGunIndex = 0;
	std::function<void(Bullet)> m_spawnBullets;

	int m_health = 50;
	int m_maxHealth = 50;
	bool m_alive = true;
};
