////////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////

#include "otpch.h"
#include "stash_category_manager.h"
#include "items.h"
#include "item.h"
#include <iostream>

StashCategoryManager& StashCategoryManager::getInstance()
{
    static StashCategoryManager instance;
    return instance;
}

bool StashCategoryManager::loadFromItemTypes()
{
    // Limpar dados existentes
    itemCategories.clear();
    categorizedItems.clear();

    // Inicializar nomes das categorias
    initializeCategoryNames();

    // Iterar por todos os ItemTypes carregados
    for (uint16_t itemId = 1; itemId <= 30000; ++itemId) {
        const ItemType& itemType = Item::items.getItemType(itemId);
        
        // Verificar se o item existe, � stackable e tem uma categoria v�lida
        if (itemType.id != 0 && itemType.stackable && itemType.stashCategory != STASH_CATEGORY_OTHERS) {
            StashCategory_t category = static_cast<StashCategory_t>(itemType.stashCategory);
            
            // Validar se a categoria � v�lida
            if (isValidCategory(category)) {
                itemCategories[itemId] = category;
                categorizedItems[category].push_back(itemId);
            }
        }
    }

    std::cout << "[StashCategoryManager] Loaded " << itemCategories.size() << " items in " 
              << categoryNames.size() << " categories from ItemTypes" << std::endl;
    
    return true;
}

StashCategory_t StashCategoryManager::getItemCategory(uint16_t itemId) const
{
    auto it = itemCategories.find(itemId);
    if (it != itemCategories.end()) {
        return it->second;
    }
    return STASH_CATEGORY_OTHERS; // Categoria padr�o para itens n�o categorizados
}

std::string StashCategoryManager::getCategoryName(StashCategory_t category) const
{
    auto it = categoryNames.find(category);
    if (it != categoryNames.end()) {
        return it->second;
    }
    return "Unknown";
}

std::vector<uint16_t> StashCategoryManager::getItemsByCategory(StashCategory_t category) const
{
    auto it = categorizedItems.find(category);
    if (it != categorizedItems.end()) {
        return it->second;
    }
    return std::vector<uint16_t>();
}

bool StashCategoryManager::isValidCategory(StashCategory_t category) const
{
    return categoryNames.find(category) != categoryNames.end();
}

bool StashCategoryManager::hasCategory(uint16_t itemId) const
{
    return itemCategories.find(itemId) != itemCategories.end();
}

std::vector<StashCategory_t> StashCategoryManager::getAllCategories() const
{
    std::vector<StashCategory_t> categories;
    for (const auto& pair : categoryNames) {
        categories.push_back(pair.first);
    }
    return categories;
}

size_t StashCategoryManager::getCategoryCount() const
{
    return categoryNames.size();
}

size_t StashCategoryManager::getItemCount() const
{
    return itemCategories.size();
}

void StashCategoryManager::initializeCategoryNames()
{
    categoryNames[STASH_CATEGORY_SHOW_ALL] = "Show All";
    categoryNames[STASH_CATEGORY_CREATURE_PRODUCTS] = "Creature Products";
    categoryNames[STASH_CATEGORY_DECORATION] = "Decoration";
    categoryNames[STASH_CATEGORY_FOOD] = "Food";
    categoryNames[STASH_CATEGORY_OTHERS] = "Others";
    categoryNames[STASH_CATEGORY_POTIONS] = "Potions";
    categoryNames[STASH_CATEGORY_RUNES] = "Runes";
    categoryNames[STASH_CATEGORY_TOOLS] = "Tools";
    categoryNames[STASH_CATEGORY_VALUABLES] = "Valuables";
    categoryNames[STASH_CATEGORY_WEAPONS_AMMO] = "Weapons: Ammo";
    categoryNames[STASH_CATEGORY_WEAPONS_DISTANCE] = "Weapons: Distance";
    categoryNames[STASH_CATEGORY_WEAPONS_MELEE] = "Weapons: Melee";
    categoryNames[STASH_CATEGORY_WEAPONS_WANDS] = "Weapons: Wands";
    }