#pragma once
#include "core/TRSTransform.h" 

struct raindrop {
	glm::vec3 pos; //change to vector later, only height now
	TRSTransformf get_transform() const
	{
		TRSTransformf t;
		float y = 6;//12 * (2.0f * rand() / RAND_MAX - 1.0f) - 4;

		t.SetScale({0.25, y, 0.25});
		t.SetTranslate(pos);
		return t;
	}
};

struct rain_effect {
	std::vector<raindrop> drops;
	static constexpr float start_pos = 1500;
	static constexpr float x_width = 800;
	static constexpr float z_width = 200;
	rain_effect()
	{
		int amount = 30000;
		for (int i = 0; i < amount; ++i) {
			float x = x_width *  (2.0f * rand() / RAND_MAX - 1.0f);
			float y = start_pos * rand() / RAND_MAX;
			float z = z_width * (2.0f * rand() / RAND_MAX - 1.0f);
			raindrop drop{{x, y, z}};
			drops.push_back(drop); 
		}
	}
	void step()
	{
		for (raindrop &drop : drops) {
			if (drop.pos.y < -1) {
				drop.pos.y = start_pos;
			}
			drop.pos.y -= 20;
		}
	}
};