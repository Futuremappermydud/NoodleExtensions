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

//https://github.com/Aeroluna/Chroma/blob/ebc91c9fa672304ffe5cbf64b31293f56e262159/Chroma/ChromaController.cs#L106
void Start(BeatmapObjectSpawnController* beatmapObjectSpawnController)
{
    BeatmapObjectManager* beatmapObjectManager = reinterpret_cast<BeatmapObjectManager*>(beatmapObjectSpawnController->beatmapObjectSpawner); 
    BeatmapObjectCallbackController* coreSetup = beatmapObjectSpawnController->beatmapObjectCallbackController;
    IAudioTimeSource* IAudioTimeSource = coreSetup->audioTimeSource;
    IReadonlyBeatmapData* beatmapData = coreSetup->beatmapData;

    //Start the fun :D

    CustomJSONData::CustomBeatmapData* customBeatmap = reinterpret_cast<CustomJSONData::CustomBeatmapData*>(beatmapData);
    if(customBeatmap->customData)
    {
        rapidjson::Value &customData = *customBeatmap->customData->value;
        rapidjson::GenericArray<false, rapidjson::Value> arr = customData["_environmentRemoval"].GetArray();
        std::vector<std::string> ObjectsToKill;
        for (int i = 0; i < arr.Size(); i++)
        {
            ObjectsToKill[i] = arr[i].GetString();
        }
        
        if(ObjectsToKill.size() > 0)
        {
            for (int i = 0; i < ObjectsToKill.size(); i++)
            {
                std::string s = ObjectsToKill[i];
                Il2CppString* css = il2cpp_utils::createcsstr(s);
                GameObject* n = GameObject::Find(css);
                if(s == "BigTrackLaneRing" || s == "TrackLaneRing")
                {
                    //Fuck this piece of absolute dog shit vomit trash
                    if(s == "TrackLaneRing" && ((System::String*)n->get_name())->Contains(il2cpp_utils::createcsstr("Big")))
                    {
                        continue;
                    }
                    n->SetActive(false);
                }
                else
                {
                    //this hurts in so many different ways that i cant explain
                    if(((System::String*)n->get_name())->Contains(il2cpp_utils::createcsstr("Environment")) && (!((System::String*)n->get_scene().get_name())->Contains(il2cpp_utils::createcsstr("Menu"))))
                    {
                        n->SetActive(false);
                    }
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