#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

class Component;
class Entity;
class Manager;

using ComponentID = std::size_t;
using Group = std::size_t;

enum class EnableMode : std::uint8_t {
	NONE   = 0,
	UPDATE = 1 << 0,
	DRAW   = 1 << 1,

	BOTH   = UPDATE | DRAW
};

constexpr EnableMode operator|(EnableMode a, EnableMode b) {
	return static_cast<EnableMode>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

constexpr EnableMode operator&(EnableMode a, EnableMode b) {
	return static_cast<EnableMode>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

constexpr EnableMode operator~(EnableMode a) {
	return static_cast<EnableMode>(~static_cast<std::uint8_t>(a));
}

inline ComponentID getNewComponentTypeID() {
	static ComponentID lastID = 0;
	return lastID++;
}

template <typename T> inline ComponentID getComponentTypeID() noexcept {
	static_assert (std::is_base_of<Component, T>::value, "");
	static ComponentID typeID = getNewComponentTypeID();
	return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitSet = std::bitset<maxGroups>;

using ComponentArray = std::array<Component*,maxComponents>;

class Component {
	private:
		ComponentID typeID = 0;
		bool updateEnabled = true;
		bool drawEnabled = true;

		friend class Entity;

	public:
		Entity * entity;
		
		virtual ~Component() {
		
		}
		
		virtual void init() { 
		
		}
		virtual void update() {
		
		}
		virtual void draw() {
		
		}

		ComponentID getTypeID() const {
			return typeID;
		}

		void setEnable(bool enabled, EnableMode mode = EnableMode::BOTH) {
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				updateEnabled = enabled;
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				drawEnabled = enabled;
		}

		bool isUpdateEnabled() const {
			return updateEnabled;
		}

		bool isDrawEnabled() const {
			return drawEnabled;
		}

		bool isEnabled(EnableMode mode = EnableMode::BOTH) const {
			bool result = true;
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				result = result && updateEnabled;
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				result = result && drawEnabled;
			return result;
		}
};

class Entity {	
	private:
		Manager& manager;
		bool active = true;
		bool updateEnabled = true;
		bool drawEnabled = true;
		std::vector<std::unique_ptr<Component>> components;
		
		ComponentArray componentArray{};
		ComponentBitSet componentBitSet;
		GroupBitSet groupBitSet;

		Entity * father = nullptr;
		std::vector<Entity*> sons;
		
		void addSonOnly(Entity * son);
		void removeSonOnly(Entity * son);

	public:
		Entity(Manager& mManager): manager(mManager) {}
		
		void update();
		void draw();
		
		bool isActive() const {
			return active;
		}
		
		void destroy();

		void setEnable(bool enabled, EnableMode mode = EnableMode::BOTH, bool recursive = true);
		bool isUpdateEnabled() const;
		bool isDrawEnabled() const;
		bool isEnabled(EnableMode mode = EnableMode::BOTH) const;
		
		bool hasGroup(Group mGroup) const {
			return groupBitSet[mGroup];
		}
		
		void addGroup(Group mGroup);
		void delGroup(Group mGroup);
		
		template <typename T> bool hasComponent() const {
			return componentBitSet[getComponentTypeID<T>()];
		}
		
		template <typename T, typename... Targs> T& addComponent(Targs&&... mArgs) {
			ComponentID id = getComponentTypeID<T>();
			T * c(new T(std::forward<Targs>(mArgs)...));
			c->entity = this;
			c->typeID = id;
			std::unique_ptr<Component> uPtr {c};
			components.emplace_back(std::move(uPtr));
			
			componentArray[id] = c;
			componentBitSet[id] = true;
			
			c->init();
			return * c;
		}
		
		template <typename T> T& getComponent() const {
			auto ptr(componentArray[getComponentTypeID<T>()]);
			return * static_cast<T*>(ptr);
		}

		template <typename T>
		void setComponentEnable(bool enabled, EnableMode mode = EnableMode::BOTH, bool recursive = true) {
			if (hasComponent<T>()) {
				getComponent<T>().setEnable(enabled, mode);
			}
			if (recursive) {
				for (Entity * son : sons) {
					if (son)
						son->setComponentEnable<T>(enabled, mode, true);
				}
			}
		}

		template <typename T>
		bool isComponentEnabled(EnableMode mode = EnableMode::BOTH) const;

		void setFather(Entity * newFather);
		void addSon(Entity * son);
		const Entity * getFather() const;
		const std::vector<Entity*>& getSons() const;
		void removeSon(Entity * son);
		void removeFromFather();
		bool hasFather(Entity * possibleFather) const;
		void destroySonsRecursive();
};

class Manager {
	private:
		std::vector<std::unique_ptr<Entity>> entities;
		std::array<std::vector<Entity*>,maxGroups> groupedEntities;

		bool needsRefresh = false;
		bool globalUpdateEnabled = true;
		bool globalDrawEnabled = true;
		ComponentBitSet componentUpdateMask;
		ComponentBitSet componentDrawMask;
		GroupBitSet groupUpdateMask;
		GroupBitSet groupDrawMask;
		GroupBitSet groupsWithDisabledComponentUpdate;
		GroupBitSet groupsWithDisabledComponentDraw;
		std::array<ComponentBitSet, maxGroups> groupComponentUpdateMask;
		std::array<ComponentBitSet, maxGroups> groupComponentDrawMask;

	public:
		Manager() {
			resetEnableMasks();
		}

		void requestRefresh() {
			needsRefresh = true;
		}

		void resetEnableMasks() {
			globalUpdateEnabled = true;
			globalDrawEnabled = true;
			componentUpdateMask.set();
			componentDrawMask.set();
			groupUpdateMask.set();
			groupDrawMask.set();
			groupsWithDisabledComponentUpdate.reset();
			groupsWithDisabledComponentDraw.reset();
			for (auto& mask : groupComponentUpdateMask)
				mask.set();
			for (auto& mask : groupComponentDrawMask)
				mask.set();
		}

		void update() {
			if (!globalUpdateEnabled)
				return;
			for (auto& e : entities) {
				e->update();
			}
		}
		
		void draw() {
			if (!globalDrawEnabled)
				return;
			for (auto& e : entities) {
				e->draw();
			}
		}

		void setEnable(bool enabled, EnableMode mode = EnableMode::BOTH) {
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				globalUpdateEnabled = enabled;
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				globalDrawEnabled = enabled;
		}

		bool isEnabled(EnableMode mode = EnableMode::BOTH) const {
			bool result = true;
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				result = result && globalUpdateEnabled;
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				result = result && globalDrawEnabled;
			return result;
		}

		template <typename T>
		void setComponentEnable(bool enabled, EnableMode mode = EnableMode::BOTH) {
			ComponentID id = getComponentTypeID<T>();
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				componentUpdateMask.set(id, enabled);
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				componentDrawMask.set(id, enabled);
		}

		template <typename T>
		bool isComponentEnabled(EnableMode mode = EnableMode::BOTH) const {
			ComponentID id = getComponentTypeID<T>();
			bool result = true;
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				result = result && componentUpdateMask.test(id);
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				result = result && componentDrawMask.test(id);
			return result;
		}

		void setGroupEnable(Group mGroup, bool enabled, EnableMode mode = EnableMode::BOTH) {
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				groupUpdateMask.set(mGroup, enabled);
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				groupDrawMask.set(mGroup, enabled);
		}

		bool isGroupEnabled(Group mGroup, EnableMode mode = EnableMode::BOTH) const {
			bool result = true;
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				result = result && groupUpdateMask.test(mGroup);
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				result = result && groupDrawMask.test(mGroup);
			return result;
		}

		template <typename T>
		void setGroupComponentEnable(Group mGroup, bool enabled, EnableMode mode = EnableMode::BOTH) {
			ComponentID id = getComponentTypeID<T>();
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE) {
				groupComponentUpdateMask[mGroup].set(id, enabled);
				groupsWithDisabledComponentUpdate.set(mGroup, !groupComponentUpdateMask[mGroup].all());
			}
			if ((mode & EnableMode::DRAW) != EnableMode::NONE) {
				groupComponentDrawMask[mGroup].set(id, enabled);
				groupsWithDisabledComponentDraw.set(mGroup, !groupComponentDrawMask[mGroup].all());
			}
		}

		template <typename T>
		bool isGroupComponentEnabled(Group mGroup, EnableMode mode = EnableMode::BOTH) const {
			ComponentID id = getComponentTypeID<T>();
			bool result = true;
			if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
				result = result && groupComponentUpdateMask[mGroup].test(id);
			if ((mode & EnableMode::DRAW) != EnableMode::NONE)
				result = result && groupComponentDrawMask[mGroup].test(id);
			return result;
		}

		bool isGroupUpdateAllowed(const GroupBitSet& entityGroups) const {
			if (!globalUpdateEnabled)
				return false;
			return (entityGroups & ~groupUpdateMask).none();
		}

		bool isGroupDrawAllowed(const GroupBitSet& entityGroups) const {
			if (!globalDrawEnabled)
				return false;
			return (entityGroups & ~groupDrawMask).none();
		}

		ComponentBitSet getEffectiveComponentUpdateMask(const GroupBitSet& entityGroups) const {
			ComponentBitSet mask = componentUpdateMask;
			GroupBitSet relevant = entityGroups & groupsWithDisabledComponentUpdate;
			if (relevant.any()) {
				for (std::size_t i = 0; i < maxGroups; ++i) {
					if (relevant.test(i))
						mask &= groupComponentUpdateMask[i];
				}
			}
			return mask;
		}

		ComponentBitSet getEffectiveComponentDrawMask(const GroupBitSet& entityGroups) const {
			ComponentBitSet mask = componentDrawMask;
			GroupBitSet relevant = entityGroups & groupsWithDisabledComponentDraw;
			if (relevant.any()) {
				for (std::size_t i = 0; i < maxGroups; ++i) {
					if (relevant.test(i))
						mask &= groupComponentDrawMask[i];
				}
			}
			return mask;
		}
		
		void refresh() {
			if (!needsRefresh)
				return;
			needsRefresh = false;

			for (auto i(0u); i < maxGroups; i++) {
				auto& v(groupedEntities[i]);
				v.erase(std::remove_if(std::begin(v),std::end(v),
					[i](Entity * mEntity) {
						return !mEntity->isActive() || !mEntity->hasGroup(i);
					}), std::end(v));
			}
			
			entities.erase(std::remove_if(std::begin(entities),std::end(entities),
				[](const std::unique_ptr<Entity> &mEntity) {
					return !mEntity->isActive();
				}),
				std::end(entities));
		}
		
		void addToGroup(Entity * mEntity, Group mGroup) {
			groupedEntities[mGroup].emplace_back(mEntity);
		}
		
		std::vector<Entity*>& getGroup(Group mGroup) {
			return groupedEntities[mGroup];
		}
		
		Entity& addEntity() {
			Entity * e = new Entity(*this);
			std::unique_ptr<Entity> uPtr{e};
			entities.emplace_back(std::move(uPtr));
			return *e;
		}
		
		void clear() {
		    for (auto& group : groupedEntities) {
		        group.clear();
		    }
		    entities.clear();
		    needsRefresh = false;
		}
};

inline bool Entity::isUpdateEnabled() const {
	return updateEnabled && manager.isGroupUpdateAllowed(groupBitSet);
}

inline bool Entity::isDrawEnabled() const {
	return drawEnabled && manager.isGroupDrawAllowed(groupBitSet);
}

inline bool Entity::isEnabled(EnableMode mode) const {
	bool result = true;
	if ((mode & EnableMode::UPDATE) != EnableMode::NONE)
		result = result && isUpdateEnabled();
	if ((mode & EnableMode::DRAW) != EnableMode::NONE)
		result = result && isDrawEnabled();
	return result;
}

inline void Entity::update() {
	if (!active || !isUpdateEnabled())
		return;
	ComponentBitSet updateMask = manager.getEffectiveComponentUpdateMask(groupBitSet);
	for (auto& c : components) {
		if (c->isUpdateEnabled() && updateMask.test(c->getTypeID())) {
			c->update();
		}
	}
}

inline void Entity::draw() {
	if (!active || !isDrawEnabled())
		return;
	ComponentBitSet drawMask = manager.getEffectiveComponentDrawMask(groupBitSet);
	for (auto& c : components) {
		if (c->isDrawEnabled() && drawMask.test(c->getTypeID())) {
			c->draw();
		}
	}
}

inline void Entity::destroy() {
	active = false;
	manager.requestRefresh();
	destroySonsRecursive();
}

inline void Entity::delGroup(Group mGroup) {
	if (groupBitSet[mGroup]) {
		groupBitSet[mGroup] = false;
		manager.requestRefresh();
	}
}

template <typename T>
bool Entity::isComponentEnabled(EnableMode mode) const {
	if (!hasComponent<T>())
		return false;
	ComponentID id = getComponentTypeID<T>();
	const Component& c = getComponent<T>();
	bool result = true;
	if ((mode & EnableMode::UPDATE) != EnableMode::NONE) {
		ComponentBitSet updateMask = manager.getEffectiveComponentUpdateMask(groupBitSet);
		result = result && isUpdateEnabled() && c.isUpdateEnabled() && updateMask.test(id);
	}
	if ((mode & EnableMode::DRAW) != EnableMode::NONE) {
		ComponentBitSet drawMask = manager.getEffectiveComponentDrawMask(groupBitSet);
		result = result && isDrawEnabled() && c.isDrawEnabled() && drawMask.test(id);
	}
	return result;
}