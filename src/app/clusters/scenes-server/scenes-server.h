/**
 *
 *    Copyright (c) 2023 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,¶
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <app-common/zap-generated/cluster-objects.h>
#include <app/AttributeAccessInterface.h>
#include <app/CommandHandlerInterface.h>
#include <app/ConcreteCommandPath.h>
#include <app/clusters/scenes-server/SceneTableImpl.h>
#include <app/data-model/DecodableList.h>
#include <app/data-model/Nullable.h>
#include <credentials/GroupDataProvider.h>

namespace chip {
namespace app {
namespace Clusters {
namespace Scenes {

class ScenesServer : public CommandHandlerInterface, public AttributeAccessInterface
{
public:
    static constexpr size_t kScenesServerMaxEndpointCount =
        EMBER_AF_SCENES_CLUSTER_SERVER_ENDPOINT_COUNT + CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT;
    static_assert(kScenesServerMaxEndpointCount <= kEmberInvalidEndpointIndex, "Scenes endpoint count error");
    static constexpr uint8_t kScenesServerMaxFabricCount = CHIP_CONFIG_MAX_FABRICS;

    // FabricSceneInfo
    class FabricSceneInfo
    {
    public:
        Span<Structs::SceneInfoStruct::Type> GetFabricSceneInfo(EndpointId endpoint);
        Structs::SceneInfoStruct::Type * GetSceneInfoStruct(EndpointId endpoint, FabricIndex fabric);
        CHIP_ERROR SetSceneInfoStruct(EndpointId endpoint, FabricIndex fabric, Structs::SceneInfoStruct::Type & sceneInfoStruct);
        void ClearSceneInfoStruct(EndpointId endpoint, FabricIndex fabric);

    private:
        CHIP_ERROR FindFabricSceneInfoIndex(EndpointId endpoint, size_t & endpointIndex);
        CHIP_ERROR FindSceneInfoStructIndex(FabricIndex fabric, size_t endpointIndex, uint8_t & index);

        Structs::SceneInfoStruct::Type mSceneInfoStructs[kScenesServerMaxEndpointCount][kScenesServerMaxFabricCount];
        uint8_t mSceneInfoStructsCount[kScenesServerMaxEndpointCount] = { 0 };
    };

    static ScenesServer & Instance();

    CHIP_ERROR Init();
    void Shutdown();

    // CommandHanlerInterface
    void InvokeCommand(HandlerContext & ctx) override;

    // AttributeAccessInterface
    CHIP_ERROR Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder) override;

    // SceneInfoStruct Accessors
    Structs::SceneInfoStruct::Type * GetSceneInfoStruct(EndpointId endpoint, FabricIndex fabric);
    CHIP_ERROR SetSceneInfoStruct(EndpointId endpoint, FabricIndex fabric, Structs::SceneInfoStruct::Type & sceneInfoStruct);

    // Callbacks
    void GroupWillBeRemoved(FabricIndex aFabricIx, EndpointId aEndpointId, GroupId aGroupId);
    void MakeSceneInvalid(EndpointId aEndpointId, FabricIndex aFabricIx);
    void MakeSceneInvalidForAllFabrics(EndpointId aEndpointId);
    void StoreCurrentScene(FabricIndex aFabricIx, EndpointId aEndpointId, GroupId aGroupId, SceneId aSceneId);
    void RecallScene(FabricIndex aFabricIx, EndpointId aEndpointId, GroupId aGroupId, SceneId aSceneId);

    // Handlers for extension field sets
    bool IsHandlerRegistered(EndpointId aEndpointId, scenes::SceneHandler * handler);
    void RegisterSceneHandler(EndpointId aEndpointId, scenes::SceneHandler * handler);
    void UnregisterSceneHandler(EndpointId aEndpointId, scenes::SceneHandler * handler);

    // Fabric
    void RemoveFabric(EndpointId aEndpointId, FabricIndex aFabricIndex);

private:
    ScenesServer() : CommandHandlerInterface(Optional<EndpointId>(), Id), AttributeAccessInterface(Optional<EndpointId>(), Id) {}
    ~ScenesServer() { Shutdown(); }

    bool mIsInitialized = false;

    //  Command handlers
    void HandleAddScene(HandlerContext & ctx, const Commands::AddScene::DecodableType & req);
    void HandleViewScene(HandlerContext & ctx, const Commands::ViewScene::DecodableType & req);
    void HandleRemoveScene(HandlerContext & ctx, const Commands::RemoveScene::DecodableType & req);
    void HandleRemoveAllScenes(HandlerContext & ctx, const Commands::RemoveAllScenes::DecodableType & req);
    void HandleStoreScene(HandlerContext & ctx, const Commands::StoreScene::DecodableType & req);
    void HandleRecallScene(HandlerContext & ctx, const Commands::RecallScene::DecodableType & req);
    void HandleGetSceneMembership(HandlerContext & ctx, const Commands::GetSceneMembership::DecodableType & req);
    void HandleEnhancedAddScene(HandlerContext & ctx, const Commands::EnhancedAddScene::DecodableType & req);
    void HandleEnhancedViewScene(HandlerContext & ctx, const Commands::EnhancedViewScene::DecodableType & req);
    void HandleCopyScene(HandlerContext & ctx, const Commands::CopyScene::DecodableType & req);

    // Group Data Provider
    Credentials::GroupDataProvider * mGroupProvider = nullptr;

    // FabricSceneInfo
    FabricSceneInfo mFabricSceneInfo;

    // Instance
    static ScenesServer mInstance;
};

} // namespace Scenes
} // namespace Clusters
} // namespace app
} // namespace chip
