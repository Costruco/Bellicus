#include "EntityComponentSystem.hpp"

//private
void Entity::addSonOnly(Entity * son) {
	if (!son)
		return;
	if (std::find(sons.begin(), sons.end(), son) == sons.end())
		sons.push_back(son);
}

void Entity::removeSonOnly(Entity * son) {
	sons.erase(std::remove(sons.begin(), sons.end(), son), sons.end());
}

//public
void Entity::update() {
	if (!active || !isUpdateEnabled())
		return;
	ComponentBitSet updateMask = manager.getEffectiveComponentUpdateMask(groupBitSet);
	for (auto& c : components) {
		if (c->isUpdateEnabled() && updateMask.test(c->getTypeID())) {
			c->update();
		}
	}
}

void Entity::draw() {
	if (!active || !isDrawEnabled())
		return;
	ComponentBitSet drawMask = manager.getEffectiveComponentDrawMask(groupBitSet);
	for (auto& c : components) {
		if (c->isDrawEnabled() && drawMask.test(c->getTypeID())) {
			c->draw();
		}
	}
}

void Entity::setEnable(bool enabled, EnableMode mode, bool recursive) {
	if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
		updateEnabled = enabled;
	if ((mode & EnableMode::DRAW) != EnableMode::NONE)
		drawEnabled = enabled;

	if (recursive) {
		for (Entity * son : sons) {
			if (son)
				son->setEnable(enabled, mode, true);
		}
	}
}

bool Entity::isUpdateEnabled() const {
	return updateEnabled && manager.isGroupUpdateAllowed(groupBitSet);
}

bool Entity::isDrawEnabled() const {
	return drawEnabled && manager.isGroupDrawAllowed(groupBitSet);
}

bool Entity::isEnabled(EnableMode mode) const {
	bool result = true;
	if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
		result = result && isUpdateEnabled();
	if ((mode & EnableMode::DRAW) != EnableMode::NONE)
		result = result && isDrawEnabled();
	return result;
}

void Entity::addGroup(Group mGroup) {
	if (groupBitSet[mGroup])
		return;
	groupBitSet[mGroup] = true;
	manager.addToGroup(this, mGroup);
}

void Entity::setFather(Entity * newFather) {
	if (newFather == this)
		return;
	if (father == newFather)
		return;
	if (newFather && newFather->hasFather(this))
		return;
	if (father)
		father->removeSonOnly(this);
	father = newFather;
	if (father)
		father->addSonOnly(this);
}

void Entity::addSon(Entity * son) {
	if (!son)
		return;
	son->setFather(this);
}

const Entity * Entity::getFather() const {
	return father;
}

const std::vector<Entity*>& Entity::getSons() const {
	return sons;
}

void Entity::removeSon(Entity * son) {
	if (!son)
		return;
	if (son->father == this)
		son->father = nullptr;
	removeSonOnly(son);
}

void Entity::removeFromFather() {
	if (father)
		father->removeSonOnly(this);
	father = nullptr;
}

bool Entity::hasFather(Entity * possibleFather) const {
	Entity * current = father;

	while (current) {
		if (current == possibleFather)
			return true;
		current = current->father;
	}
	return false;
}
		
void Entity::destroySonsRecursive() {
	std::vector<Entity*> sonsCopy = sons;
	sons.clear();

	for (Entity * son : sonsCopy) {
		if (!son)
			continue;

		son->father = nullptr;
		son->destroySonsRecursive();

		if (son->isActive())
			son->destroy();
	}
}