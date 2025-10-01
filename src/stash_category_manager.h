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

#ifndef __STASH_CATEGORY_MANAGER__
#define __STASH_CATEGORY_MANAGER__

#include "definitions.h"
#include "enums.h"
#include <map>
#include <string>
#include <vector>

class StashCategoryManager
{
public:
    static StashCategoryManager& getInstance();

    // Inicializa��o do sistema
    bool loadFromItemTypes();
    
    // Consultas de categoria
    StashCategory_t getItemCategory(uint16_t itemId) const;
    std::string getCategoryName(StashCategory_t category) const;
    std::vector<uint16_t> getItemsByCategory(StashCategory_t category) const;
    
    // Valida��es
    bool isValidCategory(StashCategory_t category) const;
    bool hasCategory(uint16_t itemId) const;
    
    // Informa��es do sistema
    std::vector<StashCategory_t> getAllCategories() const;
    size_t getCategoryCount() const;
    size_t getItemCount() const;

private:
    StashCategoryManager() = default;
    ~StashCategoryManager() = default;
    
    // N�o permitir c�pia
    StashCategoryManager(const StashCategoryManager&) = delete;
    StashCategoryManager& operator=(const StashCategoryManager&) = delete;
    
    // Dados internos
    std::map<uint16_t, StashCategory_t> itemCategories;
    std::map<StashCategory_t, std::string> categoryNames;
    std::map<StashCategory_t, std::vector<uint16_t>> categorizedItems;
    
    // M�todos auxiliares
    void initializeCategoryNames();
};

#endif // __STASH_CATEGORY_MANAGER__