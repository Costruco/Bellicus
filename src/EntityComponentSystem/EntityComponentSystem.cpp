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
void Entity::addGroup(Group mGroup) {
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