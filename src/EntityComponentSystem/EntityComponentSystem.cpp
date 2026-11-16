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