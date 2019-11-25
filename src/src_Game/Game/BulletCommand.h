#pragma once

#include <math.h>
#include <vector>
#include <queue>
#include <utility>
#include <bulletmlrunner.h>

namespace math {
	template <size_t, typename> class Vector;
	using Vec2f = Vector<2, float>;
}

class BulletMLParser;
class BulletMLState;
class BulletCommand;

struct CollisionAgent;

enum class BulletType {
	Standard,
	Player
};

struct StatelessBullet {
	math::Vec2f position;
	float direction{ 0 };
	float aimDirection{ 0 };
	float speed{ 0 };
	float defaultSpeed{ 0 };
	float timeAlive{ 0 };
	bool enabled{ false };

	BulletType type{ BulletType::Standard };
	math::Vec2f vecDirection;
	CollisionAgent* collisionAgent{nullptr};
};

struct StatefulBullet : StatelessBullet {
	BulletMLState* state{nullptr};
	BulletCommand* command{nullptr};
};

class BulletCommand final : private BulletMLRunner {
public:

	BulletCommand(BulletMLParser* bp, std::queue<StatelessBullet>* toAddStateless, std::queue<StatefulBullet>* toAddStateful, math::Vec2f shipPosition);
	BulletCommand(StatefulBullet* b, std::queue<StatelessBullet>* toAddStateless, std::queue<StatefulBullet>* toAddStateful);
	~BulletCommand() = default;

	void Run();
	void SetPosition(math::Vec2f position);
	void SetAim(math::Vec2f playerPos);

private:
	void createSimpleBullet(double direction, double speed) override;
	void createBullet(BulletMLState* state, double direction, double speed) override;

	void doVanish() override;
	void doChangeSpeed(double speed) override;
	void doChangeDirection(double direction) override;

	double getBulletDirection() override;
	double getAimDirection() override;
	double getBulletSpeed() override;
	double getDefaultSpeed() override;
	double getRank() override;
	int getTurn() override;

public:
	static int rank_;
private:
	StatefulBullet* m_bullet;
	uint32_t m_turn;
	std::queue<StatelessBullet>* m_toAddStateless;
	std::queue<StatefulBullet>* m_toAddStateful;
};
