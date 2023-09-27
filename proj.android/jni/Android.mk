# Actions to get all files for LOCAL_SRC_FILES
# 1. Open a terminal
# 2. Go to the jni directory <cocos2dx-root>/projects/TomTheGame/proj.android/jni
# 3. Execute the following command: 
# $> ls -R ../../Classes | awk '/:$/&&f{s=$0;f=0}/:$/&&!f{sub(/:$/,"");s=$0;f=1;next}NF&&f{ print s"/"$0" \\" }' | grep '.cpp'
# 4. Copy the resulting output to the variable LOCAL_SRC_FILES, keeping the first two lines 'hellocpp/main.cpp \ ../../Classes/AppDelegate.cpp \ '.

# 5 !!!!ESTO NO LISTA LOS FICHEROS DEL NIVEL DE CLASSES!!!
# 6 find ../../Classes -print |grep '\.cpp$'  <-- faltaria añadir  los separadores finales \



LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

FILE_LIST := $/(LOCAL_PATH)/../../Classes/*.cpp


LOCAL_SRC_FILES := hellocpp/main.cpp \
parse/ParseManagerAndroid.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/Layers/TTLoadingLayer.cpp \
../../Classes/Layers/TTMainMenuLayer.cpp \
../../Classes/Layers/TTMapLayer.cpp \
../../Classes/Layers/TTSplashLayer.cpp \
../../Classes/PugiXML/pugixml.cpp \
../../Classes/thegame.cpp \
../../Classes/TTAnimalCubes.cpp \
../../Classes/TTAudioManager.cpp \
../../Classes/TTBezierRotationTo.cpp \
../../Classes/TTBlockGoal.cpp \
../../Classes/TTBoard.cpp \
../../Classes/TTBoardBoosterComboColor.cpp \
../../Classes/TTBoardBoosterCombos.cpp \
../../Classes/TTBoardBoosterMoreMoves.cpp \
../../Classes/TTBoardBoosterMoreTime.cpp \
../../Classes/TTBoardBoosters.cpp \
../../Classes/TTBoardStep.cpp \
../../Classes/TTBoardStepConvertCellsToCombo.cpp \
../../Classes/TTBoardStepHitCells.cpp \
../../Classes/TTBoardStepKillCells.cpp \
../../Classes/TTBoardStepLaunchKamehamehaComboColor.cpp \
../../Classes/TTBoardStepOVNI.cpp \
../../Classes/TTBoardStepParty.cpp \
../../Classes/TTBoardStepsManager.cpp \
../../Classes/TTBoardStepWait.cpp \
../../Classes/TTBoardStepWaitUpdateCells.cpp \
../../Classes/TTBooster.cpp \
../../Classes/TTBoostersManager.cpp \
../../Classes/TTBreakGoal.cpp \
../../Classes/TTCell.cpp \
../../Classes/TTCellContent.cpp \
../../Classes/TTCellDynamicContent.cpp \
../../Classes/TTCellDynamicContentFigure.cpp \
../../Classes/TTCellDynamicContentFigureComboColor.cpp \
../../Classes/TTCellDynamicContentObjectDown.cpp \
../../Classes/TTCellStaticContent.cpp \
../../Classes/TTCellStaticContentMud.cpp \
../../Classes/TTCellStaticContentNetwork.cpp \
../../Classes/TTCellStaticContentStone.cpp \
../../Classes/TTCellStaticContentWood.cpp \
../../Classes/TTCellStaticContentGlue.cpp \
../../Classes/TTCollectFiguresGoal.cpp \
../../Classes/TTConfigParser.cpp \
../../Classes/TTContentCalculator.cpp \
../../Classes/TTDownObjectGoal.cpp \
../../Classes/TTEventsManager.cpp \
../../Classes/TTFacebook.cpp \
../../Classes/TTGame.cpp \
../../Classes/TTGameBonus.cpp \
../../Classes/TTGameBoosterBoom.cpp \
../../Classes/TTGameBoosterComboColor.cpp \
../../Classes/TTGameBoosterHammer.cpp \
../../Classes/TTGameBoosterPick.cpp \
../../Classes/TTGameBoosterRocket.cpp \
../../Classes/TTGameBoosters.cpp \
../../Classes/TTGameManager.cpp \
../../Classes/TTGameMenus.cpp \
../../Classes/TTGameUIManager.cpp \
../../Classes/TTGoal.cpp \
../../Classes/TTHowToParser.cpp \
../../Classes/TTHowToInGameManager.cpp \
../../Classes/TTHowToInGameParser.cpp \
../../Classes/TTLanguageManager.cpp \
../../Classes/TTLevelParser.cpp \
../../Classes/TTLocalNotification_Android.cpp \
../../Classes/TTMapAnimation.cpp \
../../Classes/TTMapAnimationAnimator.cpp \
../../Classes/TTMapAnimationParticles.cpp \
../../Classes/TTMapAnimationRotation.cpp \
../../Classes/TTMapAnimationJump.cpp \
../../Classes/TTMapAnimationStaticSprite.cpp \
../../Classes/TTMapManager.cpp \
../../Classes/TTMapParser.cpp \
../../Classes/TTMovesRestriction.cpp \
../../Classes/TTNetworkGoal.cpp \
../../Classes/TTNotificationManager.cpp \
../../Classes/TTParseManager.cpp \
../../Classes/TTParticleManager.cpp \
../../Classes/TTRestriction.cpp \
../../Classes/TTScoreGoal.cpp \
../../Classes/TTSendEvent.cpp \
../../Classes/TTStatistics.cpp \
../../Classes/TTStoneGoal.cpp \
../../Classes/TTStoreAssets.cpp \
../../Classes/TTStoreEventHandler.cpp \
../../Classes/TTStoreManager.cpp \
../../Classes/TTTimeRestriction.cpp \
../../Classes/TTUIBonus.cpp \
../../Classes/TTUIFacebook.cpp \
../../Classes/TTUILoading.cpp \
../../Classes/TTUIMap.cpp \
../../Classes/TTUIPopup.cpp \
../../Classes/TTUIPopupSmart.cpp \
../../Classes/TTUISettings.cpp \
../../Classes/TTUIShop.cpp \
../../Classes/TTUIShows.cpp \
../../Classes/TTUIStackManager.cpp \
../../Classes/TTUserInfo.cpp \
../../Classes/TTDailyBonusManager.cpp \
../../Classes/TTMapStopperManager.cpp \
../../Classes/TTUserLives.cpp




LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
$(LOCAL_PATH)/parse \
$(LOCAL_PATH)/../../cocos2d \
$(LOCAL_PATH)/../../Classes/PugiXML \
$(LOCAL_PATH)/../../Classes/Layers \
$(LOCAL_PATH)/../../Classes/Layers \



LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += screw_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_store_static


include $(BUILD_SHARED_LIBRARY)

#http://stackoverflow.com/questions/8549691/how-to-specify-directory-for-ndk-module-path
$(call import-add-path,$(LOCAL_PATH))

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
$(call import-module,screw)
$(call import-module,network)
$(call import-module,extensions)
$(call import-module,extensions/cocos2dx-store)



