#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/rapidjson.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "CustomJSONData/CustomBeatmapData.h"
#include "NoodleExtensions/NEHooks.h"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::SceneManagement;

// https://github.com/Aeroluna/Chroma/blob/ebc91c9fa672304ffe5cbf64b31293f56e262159/Chroma/ChromaController.cs#L106
void Start(BeatmapObjectSpawnController* beatmapObjectSpawnController) {
    BeatmapObjectManager* beatmapObjectManager = reinterpret_cast<BeatmapObjectManager*>(beatmapObjectSpawnController->beatmapObjectSpawner); 
    BeatmapObjectCallbackController* coreSetup = beatmapObjectSpawnController->beatmapObjectCallbackController;
    IAudioTimeSource* IAudioTimeSource = coreSetup->audioTimeSource;
    IReadonlyBeatmapData* beatmapData = coreSetup->beatmapData;

    CustomJSONData::CustomBeatmapData* customBeatmap = reinterpret_cast<CustomJSONData::CustomBeatmapData*>(beatmapData);

    if (customBeatmap->customData) {
        rapidjson::Value &customData = *customBeatmap->customData->value;
        rapidjson::GenericArray<false, rapidjson::Value> environmentRemoval = customData["_environmentRemoval"].GetArray();
        std::vector<std::string> ObjectsToKill;


        for (int i = 0; i < environmentRemoval.Size(); i++) {
            ObjectsToKill[i] = environmentRemoval[i].GetString();
        }
        
        for (std::string objectName : ObjectsToKill) {
            Il2CppString* il2cppObjectName = il2cpp_utils::createcsstr(objectName);
            GameObject* object = GameObject::Find(il2cppObjectName);
            auto realObjectName = (System::String*) object->get_name();
            auto sceneName = (System::String*) object->get_scene().get_name();

            if (objectName == "BigTrackLaneRing" || objectName == "TrackLaneRing") {
                // The game may find a BigTrackLaneRing even if we're only looking to remove just a TrackLaneRing
                if (objectName == "TrackLaneRing" && realObjectName->Contains(il2cpp_utils::createcsstr("Big"))) {
                    continue;
                }

                object->SetActive(false);
            } else {
                // Make sure we aren't removing the environment in menus
                if (realObjectName->Contains(il2cpp_utils::createcsstr("Environment")) && !sceneName->Contains(il2cpp_utils::createcsstr("Menu"))) {
                    object->SetActive(false);
                }
            }
        }
    }
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnControllerStart, void, BeatmapObjectSpawnController* self) {
    BeatmapObjectSpawnControllerStart(self);
    Start(self);
}

void NoodleExtensions::InstallBeatmapObjectSpawnControllerHooks() {
	INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnControllerStart, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "Start", 0));
}