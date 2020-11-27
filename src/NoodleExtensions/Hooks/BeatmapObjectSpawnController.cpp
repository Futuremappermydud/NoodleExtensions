#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/BeatmapObjectCallbackController.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/IAudioTimeSource.hpp"
#include "UnityEngine/GameObject.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/rapidjson.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "CustomJSONData/CustomBeatmapData.h"
#include "NoodleExtensions/NEHooks.h"

using namespace GlobalNamespace;
using namespace UnityEngine;

Array<Il2CppString*>* GetCSStrArray(rapidjson::Value val)
{
    Array<Il2CppString*>* csArr;
    rapidjson::GenericArray arr = val.GetArray();
    csArr = Array<Il2CppString*>::NewLength((int)arr.Size());
    for(int i = 0; i < arr.Size(); i++)
    {
        std::string strVal = val.GetString();
        csArr->values[i] = il2cpp_utils::createcsstr(strVal);
    }
    return csArr;
}

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
        
        Array<Il2CppString*>* ObjectsToKill = GetCSStrArray(customData["_environmentRemoval"].GetObject());
        if(ObjectsToKill != nullptr)
        {
            for (size_t i = 0; i < ObjectsToKill->Length(); i++)
            {
                std::string s = to_utf8(csstrtostr(ObjectsToKill->values[i]));
                if(s != "BigTrackLaneRing" || s != "TrackLaneRing")//Lazy Lol will add later
                {
                    GameObject::Find(ObjectsToKill->values[i])->SetActive(false);
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
	//INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnControllerStart, il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController", "Start", 0));
}