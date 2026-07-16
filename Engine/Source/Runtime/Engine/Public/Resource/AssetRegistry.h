#ifndef MANGO_ASSETREGISTRY_H
#define MANGO_ASSETREGISTRY_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "Resource/ResourceFwd.h"
#include "Resource/AssetHash.h"
#include "Utils/Logger.h"

namespace tomato {
    /**
     * @brief Singleton registry that manages assets of type T.
     * Stores and provides access to assets via AssetID.
     *
     * Requirements for T:
     * - static void Cleanup();
     * - static AssetID Create() for primitive asset creation
     */
    template<typename T>
    class AssetRegistry {
        AssetRegistry() = default;

    public:
        ~AssetRegistry()
        {
            Clear();
            T::Cleanup();
        }

        AssetRegistry(const AssetRegistry&) = delete;
        AssetRegistry& operator=(const AssetRegistry&) = delete;

        static AssetRegistry& GetInstance()
        {
            static AssetRegistry instance;
            return instance;
        }

        const char* GetName(AssetID id)
        {
            auto it = idToName_.find(id);
            if (it != idToName_.end())
                return it->second.c_str();

            throw std::runtime_error("AssetID not found");
        }

        std::unordered_map<AssetID, std::string>::const_iterator GetNameMapBegin() const { return idToName_.begin(); }
        std::unordered_map<AssetID, std::string>::const_iterator GetNameMapEnd() const { return idToName_.end(); }

        void Register(std::string name, std::unique_ptr<T>&& asset);


        T* Get(AssetID id);

        void CreatePrimitives()
        {
            T::Create();
        }

        void Clear()
        {
            data_.clear();
            idToIdx_.clear();
        }

    private:
        std::vector<std::unique_ptr<T>> data_;
        std::unordered_map<AssetID, uint32_t> idToIdx_;
        std::unordered_map<AssetID, std::string> idToName_;
    };

    template<typename T>
    void AssetRegistry<T>::Register(std::string name, std::unique_ptr<T>&& asset)
    {

        const auto id = GetAssetID(name.c_str());
        auto it = idToIdx_.find(id);
        if (it == idToIdx_.end())
        {
            idToIdx_[id] = data_.size();
            idToName_[id] = name;
            data_.emplace_back(std::move(asset));
        }
        else
        {
            TMT_WARN << "AssetID " << id << " already exists. Existing asset has been overwritten by " << name;
            data_[it->second] = std::move(asset);
        }
    }

    template<typename T>
    T* AssetRegistry<T>::Get(const AssetID id)
    {
        auto it = idToIdx_.find(id);
        if (it == idToIdx_.end())
            TMT_ERR << "Invalid asset ID: " << id;

        return data_[it->second].get();
    }
}

#endif //MANGO_ASSETREGISTRY_H