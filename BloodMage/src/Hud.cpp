#include "Hud.h"
#include <string>


Hud::Hud(HudDef hd) : 
	screenDimentions(hd.screenDimentions),
	font(*hd.fontPtr),
	playerPtr(hd.playerPtr),
	bloodBars(textBatch, *hd.fontPtr) {
	float unit = screenDimentions.y / 9;
	// Fire vial.
	bloodBars.fire.vialId = frontBatch.makeSprite(hd.vials.fire);
	frontBatch[bloodBars.fire.vialId].setDimensions(unit, 1.5 * unit);
	frontBatch[bloodBars.fire.vialId].setPosition(0.5 * unit, 6.25 * unit);
	// Water vial.
	bloodBars.water.vialId = frontBatch.makeSprite(hd.vials.water);
	frontBatch[bloodBars.water.vialId].setDimensions(unit, 1.5 * unit);
	frontBatch[bloodBars.water.vialId].setPosition(0.5 * unit, 3.25 * unit);
	// Earth vial.
	bloodBars.earth.vialId = frontBatch.makeSprite(hd.vials.earth);
	frontBatch[bloodBars.earth.vialId].setDimensions(unit, 1.5 * unit);
	frontBatch[bloodBars.earth.vialId].setPosition(0.5 * unit, 1.75 * unit);
	// Air vial.
	bloodBars.air.vialId = frontBatch.makeSprite(hd.vials.wind);
	frontBatch[bloodBars.air.vialId].setDimensions(unit, 1.5 * unit);
	frontBatch[bloodBars.air.vialId].setPosition(0.5 * unit, 4.75 * unit);

	// Fire blood.
	bloodBars.fire.bloodId = backBatch.makeSprite(hd.sterileBlood);
	backBatch[bloodBars.fire.bloodId].setDimensions(0.25 * unit, 0);
	backBatch[bloodBars.fire.bloodId].setColor(255, 0, 0, 255);
	backBatch[bloodBars.fire.bloodId].setPosition(0.4851 * unit, 6.25 * unit);
	bloodBars.fire.baseBloodHeight = 5.94 * unit;
	// Water blood.
	bloodBars.water.bloodId = backBatch.makeSprite(hd.sterileBlood);
	backBatch[bloodBars.water.bloodId].setDimensions(0.25 * unit, 0);
	backBatch[bloodBars.water.bloodId].setColor(0, 0, 255, 255);
	backBatch[bloodBars.water.bloodId].setPosition(0.4851 * unit, 3.23 * unit);
	bloodBars.water.baseBloodHeight = 2.94 * unit;
	// Earth blood.
	bloodBars.earth.bloodId = backBatch.makeSprite(hd.sterileBlood);
	backBatch[bloodBars.earth.bloodId].setDimensions(0.25 * unit, 0);
	backBatch[bloodBars.earth.bloodId].setColor(0, 255, 0, 255);
	backBatch[bloodBars.earth.bloodId].setPosition(0.4851 * unit, 1.75 * unit);
	bloodBars.earth.baseBloodHeight = 1.44 * unit;
	// Air blood.
	bloodBars.air.bloodId = backBatch.makeSprite(hd.sterileBlood);
	backBatch[bloodBars.air.bloodId].setDimensions(0.25 * unit, 0);
	backBatch[bloodBars.air.bloodId].setColor(255, 255, 0, 255);
	backBatch[bloodBars.air.bloodId].setPosition(0.4851 * unit, 4.75 * unit);
	bloodBars.air.baseBloodHeight = 4.44 * unit;

	for (int i = 0; i < 8; ++i) {
		// Fire casting blood.
		bloodBars.fire.castIds.push_back(backBatch.makeSprite(hd.castingBlood));
		backBatch[bloodBars.fire.castIds.back()].setDimensions(0.8 * unit, 1.5 * unit);
		backBatch[bloodBars.fire.castIds.back()].setColor(255, 0, 0, 0);
		backBatch[bloodBars.fire.castIds.back()].setPosition(0.5 * unit, 6.2 * unit);
		backBatch[bloodBars.fire.castIds.back()].setRotationAxis(0.49 * unit, 5.75 * unit);
		// Water casting blood.
		bloodBars.water.castIds.push_back(backBatch.makeSprite(hd.castingBlood));
		backBatch[bloodBars.water.castIds.back()].setDimensions(0.8 * unit, 1.5 * unit);
		backBatch[bloodBars.water.castIds.back()].setColor(0, 0, 255, 0);
		backBatch[bloodBars.water.castIds.back()].setPosition(0.5 * unit, 3.2 * unit);
		backBatch[bloodBars.water.castIds.back()].setRotationAxis(0.49 * unit, 2.75 * unit);
		// Earth casting blood.
		bloodBars.earth.castIds.push_back(backBatch.makeSprite(hd.castingBlood));
		backBatch[bloodBars.earth.castIds.back()].setDimensions(0.8 * unit, 1.5 * unit);
		backBatch[bloodBars.earth.castIds.back()].setColor(0, 255, 0, 0);
		backBatch[bloodBars.earth.castIds.back()].setPosition(0.5 * unit, 1.7 * unit);
		backBatch[bloodBars.earth.castIds.back()].setRotationAxis(0.49 * unit, 1.25 * unit);
		// Air casting blood.
		bloodBars.air.castIds.push_back(backBatch.makeSprite(hd.castingBlood));
		backBatch[bloodBars.air.castIds.back()].setDimensions(0.8 * unit, 1.5 * unit);
		backBatch[bloodBars.air.castIds.back()].setColor(255, 255, 0, 0);
		backBatch[bloodBars.air.castIds.back()].setPosition(0.5 * unit, 4.7 * unit);
		backBatch[bloodBars.air.castIds.back()].setRotationAxis(0.49 * unit, 4.25 * unit);
	}

	// Fire dial.
	bloodBars.fire.dialId = frontBatch.makeSprite(hd.dial);
	frontBatch[bloodBars.fire.dialId].setDimensions(0.15 * unit, 0.15 * unit);
	frontBatch[bloodBars.fire.dialId].setColor(255, 255, 255, 255);
	frontBatch[bloodBars.fire.dialId].setPosition(0.135 * unit, 6.47 * unit);
	frontBatch[bloodBars.fire.dialId].setRotationAxis(0.155 * unit, 6.49 * unit);
	// Water dial.
	bloodBars.water.dialId = frontBatch.makeSprite(hd.dial);
	frontBatch[bloodBars.water.dialId].setDimensions(0.15 * unit, 0.15 * unit);
	frontBatch[bloodBars.water.dialId].setColor(255, 255, 255, 255);
	frontBatch[bloodBars.water.dialId].setPosition(0.135 * unit, 3.47 * unit);
	frontBatch[bloodBars.water.dialId].setRotationAxis(0.155 * unit, 3.49 * unit);
	// Earth dial.
	bloodBars.earth.dialId = frontBatch.makeSprite(hd.dial);
	frontBatch[bloodBars.earth.dialId].setDimensions(0.15 * unit, 0.15 * unit);
	frontBatch[bloodBars.earth.dialId].setColor(255, 255, 255, 255);
	frontBatch[bloodBars.earth.dialId].setPosition(0.135 * unit, 1.97 * unit);
	frontBatch[bloodBars.earth.dialId].setRotationAxis(0.155 * unit, 1.99 * unit);
	// Air dial.
	bloodBars.air.dialId = frontBatch.makeSprite(hd.dial);
	frontBatch[bloodBars.air.dialId].setDimensions(0.15 * unit, 0.15 * unit);
	frontBatch[bloodBars.air.dialId].setColor(255, 255, 255, 255);
	frontBatch[bloodBars.air.dialId].setPosition(0.135 * unit, 4.97 * unit);
	frontBatch[bloodBars.air.dialId].setRotationAxis(0.155 * unit, 4.99 * unit);

	// Enchant text.
	rightEnchants.emplace_back(textBatch, font);
	rightEnchants.back().setText(
		std::string("right enchant 1"),
		glm::vec2(unit * 0.2),
		fk::Justification::RIGHT
	);
	rightEnchants.back().setPosition(glm::vec2(15.8 * unit, 0.5 * unit), fk::Justification::RIGHT);
	rightEnchants.emplace_back(textBatch, font);
	rightEnchants.back().setText(
		std::string("right enchant 2"),
		glm::vec2(unit * 0.2),
		fk::Justification::RIGHT
	);
	rightEnchants.back().setPosition(glm::vec2(15.8 * unit, 0.25 * unit), fk::Justification::RIGHT);
	leftEnchants.emplace_back(textBatch, font);
	leftEnchants.back().setText(
		std::string("left enchant 1"),
		glm::vec2(unit * 0.2)
	);
	leftEnchants.back().setPosition(glm::vec2(0.2 * unit, 0.5 * unit));
	leftEnchants.emplace_back(textBatch, font);
	leftEnchants.back().setText(
		std::string("left enchant 2"),
		glm::vec2(unit * 0.2)
	);
	leftEnchants.back().setPosition(glm::vec2(0.2 * unit, 0.25 * unit));

	// Items.
	for (int i = 0; i < 5; ++i) {
		itemSlots.emplace_back(textBatch, font);
		itemSlots.back().slotId = backBatch.makeSprite(hd.itemSlot);
		backBatch[itemSlots.back().slotId].setDimensions(unit, unit);
		backBatch[itemSlots.back().slotId].setPosition(15.5 * unit, 1.5 * unit + i * 1 * unit);
	}

	// Stat effects.
	for (int i = 0; i < 10; ++i) {
		statEffects.emplace_back(textBatch, font);
		statEffects.back().setText(
			std::string("stat effect"),
			glm::vec2(unit * 0.2),
			fk::Justification::RIGHT
		);
		statEffects.back().setPosition(
			glm::vec2(15.8 * unit, 8.75 * unit - i * 0.25 * unit),
			fk::Justification::RIGHT
		);
	}
}
Hud::~Hud() {

}

void Hud::update() {
	float unit = screenDimentions.y / 9;
	// Get blood info.
	auto blood = playerPtr->getBlood();
	// Fire counter.
	if ((int)blood.fire > 0) {
		bloodBars.fire.text.setText(std::string(std::to_string((int)blood.fire)), glm::vec2(unit * 0.24));
		bloodBars.fire.text[0].setColor(255, 0, 0, 255);
		bloodBars.fire.text.setPosition(glm::vec2(0.16 * unit, 6.13 * unit));
	} else {
		bloodBars.fire.text.setText(std::string(""), glm::vec2(unit * 0.24));
	}
	// Water counter.
	if ((int)blood.water > 0) {
		bloodBars.water.text.setText(std::string(std::to_string((int)blood.water)), glm::vec2(unit * 0.24));
		bloodBars.water.text[0].setColor(0, 0, 255, 255);
		bloodBars.water.text.setPosition(glm::vec2(0.16 * unit, 3.13 * unit));
	} else {
		bloodBars.water.text.setText(std::string(""), glm::vec2(unit * 0.24));
	}
	// Earth counter.
	if ((int)blood.earth > 0) {
		bloodBars.earth.text.setText(std::string(std::to_string((int)blood.earth)), glm::vec2(unit * 0.24));
		bloodBars.earth.text[0].setColor(0, 255, 0, 255);
		bloodBars.earth.text.setPosition(glm::vec2(0.16 * unit, 1.63 * unit));
	} else {
		bloodBars.earth.text.setText(std::string(""), glm::vec2(unit * 0.24));
	}
	// Air counter.
	if ((int)blood.air > 0) {
		bloodBars.air.text.setText(std::string(std::to_string((int)blood.air)), glm::vec2(unit * 0.24));
		bloodBars.air.text[0].setColor(255, 255, 0, 255);
		bloodBars.air.text.setPosition(glm::vec2(0.16 * unit, 4.63 * unit));
	} else {
		bloodBars.air.text.setText(std::string(""), glm::vec2(unit * 0.24));
	}
	// Update casting HUD.
	float spin = floor(blood.fire);
	for (auto&& id : bloodBars.fire.castIds) {
		if (blood.fire >= 1) {
			--blood.fire; 
			if (backBatch[id].canvas.color.a != 255) { backBatch[id].canvas.color.a += 5; }
		} else {
			if (backBatch[id].canvas.color.a) { backBatch[id].canvas.color.a -= 5; }
		}
		backBatch[id].canvas.rotationAngle += 0.01 * spin;
	}
	spin = floor(blood.water);
	for (auto&& id : bloodBars.water.castIds) {
		if (blood.water >= 1) {
			--blood.water;
			if (backBatch[id].canvas.color.a != 255) { backBatch[id].canvas.color.a += 5; }
		} else {
			if (backBatch[id].canvas.color.a) { backBatch[id].canvas.color.a -= 5; }
		}
		backBatch[id].canvas.rotationAngle += 0.01 * spin;
	}
	spin = floor(blood.earth);
	for (auto&& id : bloodBars.earth.castIds) {
		if (blood.earth >= 1) {
			--blood.earth;
			if (backBatch[id].canvas.color.a != 255) { backBatch[id].canvas.color.a += 5; }
		} else {
			if (backBatch[id].canvas.color.a) { backBatch[id].canvas.color.a -= 5; }
		}
		backBatch[id].canvas.rotationAngle += 0.01 * spin;
	}
	spin = floor(blood.air);
	for (auto&& id : bloodBars.air.castIds) {
		if (blood.air >= 1) {
			--blood.air;
			if (backBatch[id].canvas.color.a != 255) { backBatch[id].canvas.color.a += 5; }
		} else {
			if (backBatch[id].canvas.color.a) { backBatch[id].canvas.color.a -= 5; }
		}
		backBatch[id].canvas.rotationAngle += 0.01 * spin;
	}
	// Update dials.
	frontBatch[bloodBars.fire.dialId].canvas.rotationAngle =
		-(fk::TAU * 0.72) / (60 * 20) * playerPtr->drainTimer.fire;
	frontBatch[bloodBars.water.dialId].canvas.rotationAngle =
		-(fk::TAU * 0.72) / (60 * 20) * playerPtr->drainTimer.water;
	frontBatch[bloodBars.earth.dialId].canvas.rotationAngle =
		-(fk::TAU * 0.72) / (60 * 20) * playerPtr->drainTimer.earth;
	frontBatch[bloodBars.air.dialId].canvas.rotationAngle =
		-(fk::TAU * 0.72) / (60 * 20) * playerPtr->drainTimer.air;
	// Scale.
	blood.fire *= unit * 0.6;
	blood.water *= unit * 0.6;
	blood.earth *= unit * 0.6;
	blood.air *= unit * 0.6;
	// Update sterile info.
	float changeRate = 0.02;
	// Fire.
	float& fireDim = backBatch[bloodBars.fire.bloodId].canvas.dimensions.y;
	if (fireDim > blood.fire) {
		if ((fireDim -= changeRate * unit) < blood.fire) { fireDim = blood.fire; }
	} else if (fireDim < blood.fire) {
		if ((fireDim += changeRate * unit) > blood.fire) { fireDim = blood.fire; }
	}
	backBatch[bloodBars.fire.bloodId].canvas.position.y =
		bloodBars.fire.baseBloodHeight + fireDim / 2;
	// Water.
	float& waterDim = backBatch[bloodBars.water.bloodId].canvas.dimensions.y;
	if (waterDim > blood.water) {
		if ((waterDim -= changeRate * unit) < blood.water) { waterDim = blood.water; }
	} else if (waterDim < blood.water) {
		if ((waterDim += changeRate * unit) > blood.water) { waterDim = blood.water; }
	}
	backBatch[bloodBars.water.bloodId].canvas.position.y =
		bloodBars.water.baseBloodHeight + waterDim / 2;
	// Earth.
	float& earthDim = backBatch[bloodBars.earth.bloodId].canvas.dimensions.y;
	if (earthDim > blood.earth) {
		if ((earthDim -= changeRate * unit) < blood.earth) { earthDim = blood.earth; }
	} else if (earthDim < blood.earth) {
		if ((earthDim += changeRate * unit) > blood.earth) { earthDim = blood.earth; }
	}
	backBatch[bloodBars.earth.bloodId].canvas.position.y =
		bloodBars.earth.baseBloodHeight + earthDim / 2;
	// Air.
	float& airDim = backBatch[bloodBars.air.bloodId].canvas.dimensions.y;
	if (airDim > blood.air) {
		if ((airDim -= changeRate * unit) < blood.air) { airDim = blood.air; }
	} else if (airDim < blood.air) {
		if ((airDim += changeRate * unit) > blood.air) { airDim = blood.air; }
	}
	backBatch[bloodBars.air.bloodId].canvas.position.y =
		bloodBars.air.baseBloodHeight + airDim / 2;
}
