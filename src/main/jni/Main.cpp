#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>


#include "Includes/Logger.h"
#include "Includes/obfuscate.h"

#include "Includes/Utils.h"
#include "Includes/Strings.h"

#include "KittyMemory/MemoryPatch.h"
#include "Menu.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"




// fancy struct for patches for kittyMemory
struct My_Patches {
    // let's assume we have patches for these functions for whatever game
    // like show in miniMap boolean function
    MemoryPatch FreeCraft, FreeLearn, UnlockCraft,IsKnown,Level,InfItems,
    FreeBuild,CanComplete,Durability,FastLoot,ChestKey,ChestLocked,ShowEvents;
    // etc...
} hexPatches;

bool FreeCraft, InfItems,FreeBuild,InstantRun,Durability,FastLoot,ShowEvents,HungerThirst,DoubleResource,DamageMod,HorseFoodBool,UnlockChestsBool,FreeRun;


monoString *(*String_CreateString)(void *_this, const char *str);
void (*get_StringInstance);

float SpeedValue = 4;
bool (*isPlayer)(void *instance);
float (*old_set_Speed)(void *instance);
float set_Speed(void *instance) {

    if(instance != NULL ) {
        bool isPlayer_ = isPlayer(instance);
        if(isPlayer_) {
            return SpeedValue;
        }

    }
    return old_set_Speed(instance);
}










void (*set_amount_HungerThirst)(void *instance, double amount);
void (*HealPlayer)(void *instance, double amount,int healSource);

int (*old_GetDamage)(void * instance);
int GetDamage(void * instance) {

    if(instance != NULL && DamageMod) {
        return 10000;
    }
    return  old_GetDamage(instance);
}







void (*AddHorseXP)(void * instance,int xp);
bool AddXP = false;
void (*old_Update_Horse)(void *instance);
void Update_Horse(void *instance) {

        if(instance != NULL && AddXP) {
                AddXP = false;
                AddHorseXP(instance,100000);
            }
    old_Update_Horse(instance);

}









int (*old_get_boostEnergyPrice)(void * instance);
int get_boostEnergyPrice(void * instance) {
    if(instance != NULL && FreeRun) {
        return 0;
    }
   return old_get_boostEnergyPrice(instance);
}







void (*old_HorseFood)(void * instance,int foodPrice,bool overComing);
void HorseFood(void * instance,int foodPrice,bool overComing) {

    if(instance != NULL && HorseFoodBool) {


        return;
    }
      old_HorseFood(instance,foodPrice,overComing);
}



void (*Debug_MoveToPointImmediate)(void *instance, monoString * PointID);
bool MoveToCastle = false;
bool MoveToHome = false;
bool MoveToAissa = false;

void(* old_UpdateMap)(void * instance);
void UpdateMap(void*instance) {
    if(instance != NULL ) {
       if(MoveToCastle) {
           MoveToCastle = false;
           Debug_MoveToPointImmediate(instance,String_CreateString(get_StringInstance,"castle"));
       }
        if(MoveToHome) {
            MoveToHome = false;

            Debug_MoveToPointImmediate(instance,String_CreateString(get_StringInstance,"home"));
        }
        if(MoveToAissa) {
            MoveToAissa = false;
            Debug_MoveToPointImmediate(instance,String_CreateString(get_StringInstance,"aissa"));
        }


    }
    old_UpdateMap(instance);
}






void (*Leave)(void *instance);
bool CanLeave = false;



void (*old_Update_Character)(void *instance);
void Update_Character(void *instance) {

    if (instance != NULL && DoubleResource) {

    HealPlayer(instance,200,1);
    }

    if(instance != NULL  && CanLeave) {
        CanLeave = false;
        Leave(instance);
    }



    old_Update_Character(instance);
}





float (*old_MoveToDistance)(void *instance,void * SrcPoint,void * SrcDest);
float MoveToDistance(void *instance,void * SrcPoint,void * SrcDest) {
    if (instance != NULL && InstantRun) {

        old_MoveToDistance(instance,SrcPoint,SrcDest);
        return 0.1f;
    }

   return old_MoveToDistance(instance,SrcPoint,SrcDest);
}




void (*old_Update_HungerThirst)(void *instance);
void Update_HungerThirst(void *instance) {
    if (instance != NULL && HungerThirst) {

        set_amount_HungerThirst(instance,0);
    }

    old_Update_HungerThirst(instance);
}




// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libMyLibName.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__)

    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0x963A94),  (void*)Update_HungerThirst, (void**)&old_Update_HungerThirst);
    set_amount_HungerThirst = (void(*)(void *,double ))getAbsoluteAddress(targetLibName, 0x9638C0);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0xAA7514),  (void*)Update_Character, (void**)&old_Update_Character);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0x9D73EC),  (void*)GetDamage, (void**)&old_GetDamage);
    isPlayer = (bool(*)(void *))getAbsoluteAddress(targetLibName, 0xA9CD24);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0xB17FD4),  (void*)HorseFood, (void**)&old_HorseFood);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0xA9ABA8),  (void*)set_Speed, (void**)&old_set_Speed);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0xB186D8),  (void*)Update_Horse, (void**)&old_Update_Horse);
    AddHorseXP = (void(*)(void *,int ))getAbsoluteAddress(targetLibName, 0xB1808C);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0x19DDBF4),  (void*)MoveToDistance, (void**)&old_MoveToDistance);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0x19DC8D0),  (void*)get_boostEnergyPrice, (void**)&old_get_boostEnergyPrice);
    A64HookFunction((void*)getAbsoluteAddress(targetLibName, 0x19DDDD4),  (void*)UpdateMap, (void**)&old_UpdateMap);
    Debug_MoveToPointImmediate = (void (*)(void *,monoString *))getAbsoluteAddress(targetLibName, 0x19E07A4);
    String_CreateString = (monoString*(*)(void *,const char *))getAbsoluteAddress(targetLibName, 0x1646CA0);
    Leave = (void(*)(void * ))getAbsoluteAddress(targetLibName, 0xAA912C);
    HealPlayer = (void(*)(void *,double ,int))getAbsoluteAddress(targetLibName, 0xA99598);
   get_StringInstance  = (void (*))getAbsoluteAddress(targetLibName,0x1646CA0);


     hexPatches.FreeCraft = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x9FE384")),
                                                    OBFUSCATE("20 00 80 D2 C0 03 5F D6"));
      hexPatches.FreeLearn = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x9FE258")),
                                                    OBFUSCATE("20 00 80 D2 C0 03 5F D6"));

      hexPatches.UnlockCraft = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x9FE1C4")),
                                                    OBFUSCATE("00 00 80 D2 C0 03 5F D6"));
      hexPatches.IsKnown = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x9FE178")),
                                                    OBFUSCATE("20 00 80 D2 C0 03 5F D6"));
      hexPatches.Level = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x9FE0D0")),
                                                    OBFUSCATE("00 00 80 D2 C0 03 5F D6"));
      hexPatches.InfItems = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0xA7F498")),
                                                    OBFUSCATE("80 02 80 D2 C0 03 5F D6"));
       hexPatches.FreeBuild = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0xB423B8")),
                                                    OBFUSCATE("20 00 80 D2 C0 03 5F D6"));

       hexPatches.CanComplete = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x1ADA780")),
                                                    OBFUSCATE("20 00 80 D2 C0 03 5F D6"));
       hexPatches.Durability = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x8AAB04")),
                                                    OBFUSCATE("C0 03 5F D6"));
        hexPatches.FastLoot = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0xAAE62C")),
                                                    OBFUSCATE("00 00 80 D2 C0 03 5F D6"));
         hexPatches.ChestKey = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0xAAE660")),
                                                    OBFUSCATE("00 00 80 D2 C0 03 5F D6"));
         hexPatches.ChestLocked = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0xAAE58C")),
                                                    OBFUSCATE("20 00 80 D2 C0 03 5F D6"));
        hexPatches.ShowEvents = MemoryPatch::createWithHex(targetLibName,
                                                    string2Offset(OBFUSCATE("0x19E818C")),
                                                    OBFUSCATE("20 00 80 D2 C0 03 5F D6"));
#else

#endif

    // Anti Leech
    sleep(20);
    if (!titleValid || !headingValid || !iconValid || !settingsValid) {
        int *p = 0;
        *p = 0;
    }

    return NULL;
}

/// -------------------------------------------------------------------- ///

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

jobjectArray getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    //Toasts added here so it's harder to remove it
    MakeToast(env, context, OBFUSCATE("Modded by Xxbatman9898"), Toast::LENGTH_LONG);

    const char *features[] = {
            OBFUSCATE("Category_ Cheats"), //Not counted
            OBFUSCATE("Toggle_ Free Craft"), // 0
            OBFUSCATE("Toggle_ Infinite Items"), // 1
            OBFUSCATE("Toggle_ Free Build"), // 2
            OBFUSCATE("Toggle_ Instant Run"), // 3
            OBFUSCATE("Toggle_ Durability"), // 4
            OBFUSCATE("Toggle_ Fast Loot"), // 5
            OBFUSCATE("Toggle_ Show Events"), //6
            OBFUSCATE("Toggle_ Infinite Food Water"), //7
            OBFUSCATE("Toggle_ God Mode"), // 8
            OBFUSCATE("Toggle_ Free Run"), // 9
            OBFUSCATE("Toggle_Damage Mod"), // 10
            OBFUSCATE("Toggle_ Free Ride"), // 11
            OBFUSCATE("Toggle_ UnlockChests"), // 12
            OBFUSCATE("SeekBar_Speed_1_10"), // 13
            OBFUSCATE("Button_Leave Location"), // 14
           OBFUSCATE("Button_ Add XP Horse"), //15
            OBFUSCATE("Button_ Set Location Castle"), //16
            OBFUSCATE("Button_ Set Location Home"),//17
            OBFUSCATE("Button_ Set Location Aissa") //18





    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 0:
            FreeCraft = boolean;
            if (FreeCraft) {

                hexPatches.FreeCraft.Modify();
                hexPatches.FreeLearn.Modify();
                hexPatches.UnlockCraft.Modify();
                hexPatches.IsKnown.Modify();
                hexPatches.Level.Modify();

                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.FreeCraft.Restore();
                hexPatches.FreeLearn.Restore();
                hexPatches.UnlockCraft.Restore();
                hexPatches.IsKnown.Restore();
                hexPatches.Level.Restore();

                //LOGI(OBFUSCATE("Off"));
            }
            break;
        case 1:
            InfItems = boolean;
            if (InfItems) {
                hexPatches.InfItems.Modify();


                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.InfItems.Restore();
                //LOGI(OBFUSCATE("Off"));
            }

            break;
        case 2:
            FreeBuild = boolean;
            if (FreeBuild) {
                hexPatches.FreeBuild.Modify();
                hexPatches.CanComplete.Modify();

                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.FreeBuild.Restore();
                hexPatches.CanComplete.Restore();
                //LOGI(OBFUSCATE("Off"));
            }

            break;
        case 3:
            InstantRun = boolean;

            break;
        case 4:
            Durability = boolean;
            if (Durability) {
                hexPatches.Durability.Modify();


                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.Durability.Restore();
                //LOGI(OBFUSCATE("Off"));
            }

            break;
        case 5:
            FastLoot = boolean;
            if (FastLoot) {
                hexPatches.FastLoot.Modify();



                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.FastLoot.Restore();

                //LOGI(OBFUSCATE("Off"));
            }

            break;
        case 6:
            ShowEvents = boolean;
            if (ShowEvents) {
                hexPatches.ShowEvents.Modify();



                //LOGI(OBFUSCATE("On"));
            } else {
                hexPatches.ShowEvents.Restore();

                //LOGI(OBFUSCATE("Off"));
            }

            break;
        case 7:
            HungerThirst = boolean;


            break;
        case 8:
            DoubleResource = boolean;


            break;

        case 9:

            FreeRun = boolean;
            break;

        case 10:

            DamageMod = boolean;

            break;

        case 11:

            HorseFoodBool = boolean;

            break;
        case 12:
                UnlockChestsBool = boolean;
                if(UnlockChestsBool) {
                    hexPatches.ChestLocked.Modify();
                    hexPatches.ChestKey.Modify();

                } else {
                    hexPatches.ChestLocked.Restore();
                    hexPatches.ChestKey.Restore();
                }

            break;
        case 13:

          SpeedValue = value;

            break;
        case 14:

            CanLeave = true;

            break;
        case 15:
            AddXP = true;


            break;
        case 16:
            MoveToCastle = true;


            break;
        case 17:
            MoveToHome = true;


            break;

        case 18:
            MoveToAissa = true;


            break;





    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);

    // Register your class native methods. Build and ecompile the app and see the signature
    // This is to hide function names from disassembler
    // See more: https://developer.android.com/training/articles/perf-jni#native-libraries

    //Your menu class
    jclass c = globalEnv->FindClass("com/android/support/Menu");
    if (c != nullptr){
        static const JNINativeMethod menuMethods[] = {
              {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
              {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
              {OBFUSCATE("isGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
              {OBFUSCATE("setHeadingText"),  OBFUSCATE("(Landroid/widget/TextView;)V"), reinterpret_cast<void *>(setHeadingText)},
              {OBFUSCATE("setTitleText"),  OBFUSCATE("(Landroid/widget/TextView;)V"), reinterpret_cast<void *>(setTitleText)},
              {OBFUSCATE("settingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(settingsList)},
              {OBFUSCATE("getFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(getFeatureList)},
        };

        int mm = globalEnv->RegisterNatives(c, menuMethods, sizeof(menuMethods) / sizeof(JNINativeMethod));
        if (mm != JNI_OK) {
            LOGE(OBFUSCATE("Menu methods error"));
            return mm;
        }
    }
    else{
        LOGE(OBFUSCATE("JNI error"));
        return JNI_ERR;
    }

    jclass p = globalEnv->FindClass( OBFUSCATE("com/android/support/Preferences"));
    if (p != nullptr){
        static const JNINativeMethod prefmethods[] = {
                { OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
        };

        int pm = globalEnv->RegisterNatives(p, prefmethods, sizeof(prefmethods) / sizeof(JNINativeMethod));
        if (pm != JNI_OK){
            LOGE(OBFUSCATE("Preferences methods error"));
            return pm;
        }
    }
    else{
        LOGE(OBFUSCATE("JNI error"));
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

