//
//  HudStage.cpp
//  Vinezors2.0
//
//  Created by Calvin Phung on 4/17/14.
//
//

#include "HudStage.h"

#include "Tunnel.h"
#include "Player.h"

extern Util::ConfigGlobal globals;

HudStage::HudStage(Player* player, Tunnel* tunnel)
: Hud()
{
    link(player, tunnel);
    init();
}

HudStage::~HudStage()
{
    dealloc();
    unlink();
}

void HudStage::init()
{
    alloc();
    setOverlay();
}

void HudStage::adjust()
{
    setOverlay();
}

void HudStage::update(float elapsed)
{
    float timeLeft = fmax(tunnel->getStageTime() - tunnel->getTotalElapsed() - tunnel->getTimePenalty(), 0.0f);

    label1->setCaption(globals.messageBig);
    Ogre::ColourValue fontColor = timeLeft <= 0.0 ? ColourValue(1.0, 0.0, 0.0) : ColourValue(1.0, 1.0, 1.0);
    label2->setColour(fontColor);
    label2->setCaption("Time: " + Util::toStringInt(timeLeft));

    /*
    switch (tunnel->getPhase())
    {
        case 'A':
            label3->setCaption("Color/Sound");
            break;
        case 'B':
            label3->setCaption("Shape/Sound");
            break;
        case 'C':
            label3->setCaption("Sound");
            break;
        case 'D':
            label3->setCaption("Color/Shape/Sound");
            break;
        case 'E':
            label3->setCaption("Navigation");
            break;
        case 'F':
            label3->setCaption("Tutorial");
            break;
        default:
            label3->setCaption("");
            break;
    }
    */
    if (tunnel->getMode() == STAGE_MODE_RECESS)
    {
        float percentComplete = static_cast<float>((tunnel->getSpawnLimit() - tunnel->getSignalsLeft())) / tunnel->getSpawnLimit() * 100;
        percentComplete = Util::clamp(percentComplete, 0.0, 100.0);
        label3->setCaption("Complete: " + Util::toStringInt(percentComplete) + "%");
        
    }
    else
        label3->setCaption("");
    
    label4->setCaption("Speed: " + Util::toStringInt(player->getFinalSpeed()));
    label5->setCaption("Score: " + Util::toStringInt(player->getScore()));
    label6->setCaption(globals.message);
    label7->setCaption("");
    
    // Set Progress Bar indicator position for the appropriate mode
    float barWidth = barHP->getWidth();
    if (tunnel->getMode() == STAGE_MODE_PROFICIENCY)
    {
        float HPRange = globals.HPPositiveLimit - globals.HPNegativeLimit + 1;
        //indicator->setPosition(barHP->getLeft() + barWidth * player->getProgress(), indicator->getTop());
        indicator->setPosition(barHP->getLeft() + barWidth * (player->getHP() - globals.HPNegativeLimit) / HPRange, indicator->getTop());
    }
    
    toggle1Text->setCaption(Util::toStringInt(tunnel->getNBack()));
    toggle2Text->setCaption(Util::toStringInt(tunnel->getNBack() - 1));
    toggle3Text->setCaption(Util::toStringInt(tunnel->getNBack() - 2));
    toggle4Text->setCaption(Util::toStringInt(tunnel->getNBack() - 3));
    
    switch (player->getToggleBack())
    {
        case 0:
            toggleIndicator->setPosition(0.895, 0.45);
            break;
        case 1:
            toggleIndicator->setPosition(0.895, 0.55);
            break;
        case 2:
            toggleIndicator->setPosition(0.895, 0.65);
            break;
        case 3:
            toggleIndicator->setPosition(0.895, 0.75);
            break;
    }
    
    if (player->isPowerUpAvailable("TractorBeam"))
        buttons[BUTTON_POWERUP1].backgroundRef->setMaterialName("General/GUIPowerupButton2");
    else
        buttons[BUTTON_POWERUP1].backgroundRef->setMaterialName("General/GUIPowerupButtonBlank");
    if (player->isPowerUpAvailable("Shields"))
        buttons[BUTTON_POWERUP2].backgroundRef->setMaterialName("General/GUIPowerupButton0");
    else
        buttons[BUTTON_POWERUP2].backgroundRef->setMaterialName("General/GUIPowerupButtonBlank");
    if (player->isPowerUpAvailable("TimeWarp"))
        buttons[BUTTON_POWERUP3].backgroundRef->setMaterialName("General/GUIPowerupButton1");
    else
        buttons[BUTTON_POWERUP3].backgroundRef->setMaterialName("General/GUIPowerupButtonBlank");
    
    std::vector<CollectionCriteria> criterias = tunnel->getCollectionCriteria();
    for (int i = 0; i < collectionBar.size(); ++i)
    {
        if (i < criterias.size())
        {
            std::string scoreName = "General/GUICollection";
            if (criterias[i].nback <= 0)
                scoreName += Util::toStringInt(0);
            else
                scoreName += Util::toStringInt(Util::clamp(3 - (tunnel->getNBack() - criterias[i].nback), 0, 3));
            
            if (criterias[i].collected)
                scoreName += "Filled";
            else
                scoreName += "Blank";
            collectionBar[i]->setMaterialName(scoreName);
        }
        else
        {
            collectionBar[i]->setMaterialName("General/GUICollectionGreyed");
        }
    }
    
    std::string GUIToggleNumber = "General/GUIToggleTextNumber";
    toggle1TextArt->setMaterialName(GUIToggleNumber + Util::toStringInt(tunnel->getNBack() % 10));
    toggle2TextArt->setMaterialName(GUIToggleNumber + Util::toStringInt(Util::clamp((tunnel->getNBack() - 1) % 10, 0, tunnel->getNBack())));
    toggle3TextArt->setMaterialName(GUIToggleNumber + Util::toStringInt(Util::clamp((tunnel->getNBack() - 2) % 10, 0, tunnel->getNBack())));
    // toggle4 is always n-back 0
    
    if (!tunnel->needsCleaning())
        resumeButtonBackground->setMaterialName("General/ResumeButtonRound");
    else
        resumeButtonBackground->setMaterialName("General/ResumeButtonRoundGRAY");
    
    if (player->getLevels()->hasLevel(player->getLevelRequest() + 1))
        nextButtonBackground->setMaterialName("General/NextButtonRound");
    else
        nextButtonBackground->setMaterialName("General/NextButtonRoundGRAY");
}

void HudStage::alloc()
{
    // Allocate Resources
    panelText = static_cast<OverlayContainer*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageTextInterface"));
    panelText->setMetricsMode(GMM_PIXELS);
    panelText->setPosition(10, 10);
    panelText->setDimensions(10, 10);
    
    healthArea = static_cast<BorderPanelOverlayElement*>(
                                                         OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("BorderPanel", "StageHealthAreaBorder"));
    
    barHP = static_cast<PanelOverlayElement*>(
                                              OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageHealthBar"));
    indicator = static_cast<PanelOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageIndicator"));
    pauseBackground = static_cast<PanelOverlayElement*>(
                                                        OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StagePauseBackground"));
    
    collectionBar = std::vector<PanelOverlayElement*>();
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem0")));
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem1")));
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem2")));
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem3")));
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem4")));
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem5")));
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem6")));
    collectionBar.push_back(static_cast<PanelOverlayElement*>(OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageCollectionItem7")));
    
    GUITopPanel = static_cast<PanelOverlayElement*>(
                                                    OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "GUITopPanel"));
    GUILeftPanel = static_cast<PanelOverlayElement*>(
                                                    OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "GUILeftPanel"));
    GUIRightPanel = static_cast<PanelOverlayElement*>(
                                                    OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "GUIRightPanel"));
    
    // Create text area
    label1 = static_cast<TextAreaOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageTextAreaLabel1"));
    label2 = static_cast<TextAreaOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageTextAreaLabel2"));
    label3 = static_cast<TextAreaOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageTextAreaLabel3"));
    label4 = static_cast<TextAreaOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageTextAreaLabel4"));
    label5 = static_cast<TextAreaOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageTextAreaLabel5"));
    label6 = static_cast<TextAreaOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageTextAreaLabel6"));
    label7 = static_cast<TextAreaOverlayElement*>(
                                                  OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageTextAreaLabel7"));
    
    toggleEntireBackground = static_cast<PanelOverlayElement*>(
                                                               OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggleEntireBackground"));
    // Note: These overlay elements are linked to a button, if we end up deleting these and NULLing it, it should be NULLed in the associated button as well
    toggle1Background = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle1Background"));
    toggle2Background = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle2Background"));
    toggle3Background = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle3Background"));
    toggle4Background = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle4Background"));
    toggle1TextArt = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle1TextArt"));
    toggle2TextArt = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle2TextArt"));
    toggle3TextArt = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle3TextArt"));
    toggle4TextArt = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggle4TextArt"));
    toggleIndicator = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageToggleIndicator"));
    toggle1Text = static_cast<TextAreaOverlayElement*>(
                                                       OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageToggle1Text"));
    toggle2Text = static_cast<TextAreaOverlayElement*>(
                                                       OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageToggle2Text"));
    toggle3Text = static_cast<TextAreaOverlayElement*>(
                                                       OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageToggle3Text"));
    toggle4Text = static_cast<TextAreaOverlayElement*>(
                                                       OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("TextArea", "StageToggle4Text"));
    powerup1Background = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "Powerup1Background"));
    powerup2Background = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "Powerup2Background"));
    powerup3Background = static_cast<PanelOverlayElement*>(
                                                          OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "Powerup3Background"));
    
    
    resumeButtonBackground = static_cast<PanelOverlayElement*>(
                                                           OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageResumeButtonBackground"));
    nextButtonBackground = static_cast<PanelOverlayElement*>(
                                                           OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageNextButtonBackground"));
    restartButtonBackground = static_cast<PanelOverlayElement*>(
                                                                OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageRestartButtonBackground"));
    levelSelectButtonBackground = static_cast<PanelOverlayElement*>(
                                                                OgreFramework::getSingletonPtr()->m_pOverlayMgr->createOverlayElement("Panel", "StageLevelSelectButtonBackground"));
    
    buttons = std::vector<HudButton>(12);
    
    // Create an overlay, and add the panel
    Overlay* overlay1 = OgreFramework::getSingletonPtr()->m_pOverlayMgr->create("StageOverlayHUD");
    Overlay* overlay2 = OgreFramework::getSingletonPtr()->m_pOverlayMgr->create("StageOverlayPauseMenu");
    //overlay1->add2D(healthArea);
    //overlay1->add2D(barHP);
    //overlay1->add2D(indicator);
    
    overlay1->add2D(GUITopPanel);
    //overlay1->add2D(GUILeftPanel);
    //overlay1->add2D(GUIRightPanel);
    
    overlay1->add2D(pauseBackground);
    for (int i = 0; i < collectionBar.size(); ++i)
        overlay1->add2D(collectionBar[i]);
    
    //overlay1->add2D(toggleEntireBackground);
    overlay1->add2D(powerup1Background);
    overlay1->add2D(powerup2Background);
    overlay1->add2D(powerup3Background);
    overlay1->add2D(toggle1Background);
    overlay1->add2D(toggle2Background);
    overlay1->add2D(toggle3Background);
    overlay1->add2D(toggle4Background);
    toggle1Background->addChild(toggle1TextArt);
    toggle2Background->addChild(toggle2TextArt);
    toggle3Background->addChild(toggle3TextArt);
    toggle4Background->addChild(toggle4TextArt);
    overlay1->add2D(toggleIndicator);
    overlay1->add2D(panelText);
    panelText->addChild(label1);
    panelText->addChild(label2);
    panelText->addChild(label3);
    panelText->addChild(label4);
    panelText->addChild(label5);
    panelText->addChild(label6);
    panelText->addChild(label7);
    //panelText->addChild(toggle1Text);
    //panelText->addChild(toggle2Text);
    //panelText->addChild(toggle3Text);
    //panelText->addChild(toggle4Text);
    overlay2->add2D(resumeButtonBackground);
    overlay2->add2D(nextButtonBackground);
    overlay2->add2D(restartButtonBackground);
    overlay2->add2D(levelSelectButtonBackground);
    
    overlays.push_back(overlay1);
    overlays.push_back(overlay2);
}

void HudStage::dealloc()
{
    // Delete children first, then parents
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(healthArea);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(barHP);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(indicator);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(pauseBackground);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(GUITopPanel);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(GUILeftPanel);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(GUIRightPanel);
    for (int i = 0; i < collectionBar.size(); ++i)
        OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(collectionBar[i]);
    collectionBar.clear();
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(label1);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(label2);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(label3);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(label4);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(label5);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(label6);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(label7);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggleEntireBackground);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle1TextArt);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle2TextArt);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle3TextArt);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle4TextArt);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle1Background);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle2Background);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle3Background);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle4Background);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggleIndicator);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(powerup1Background);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(powerup2Background);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(powerup3Background);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(resumeButtonBackground);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(nextButtonBackground);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(restartButtonBackground);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(levelSelectButtonBackground);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle1Text);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle2Text);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle3Text);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(toggle4Text);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroyOverlayElement(panelText);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroy(overlays[0]);
    OgreFramework::getSingletonPtr()->m_pOverlayMgr->destroy(overlays[1]);
}

void HudStage::setOverlay()
{
    // Link and Set Resources
    panelText->setMetricsMode(GMM_PIXELS);
    panelText->setPosition(10, 10);
    panelText->setDimensions(10, 10);
    
    healthArea->setMetricsMode(GMM_RELATIVE);
    healthArea->setPosition(globals.HPBarXRef - 0.01, globals.HPBarYRef - 0.01);
    healthArea->setDimensions(globals.HPBarWidth + 0.02, globals.HPBarHeight + 0.02);
    barHP->setMetricsMode(GMM_RELATIVE);
    barHP->setPosition(globals.HPBarXRef, globals.HPBarYRef);
    barHP->setDimensions(0.0, globals.HPBarHeight);
    barHP->setMaterialName("General/BarColors");
    if (globals.HPPositiveLimit >= 8)
    {
        healthArea->setMaterialName("General/BarColors3");
        barHP->setDimensions(healthArea->getWidth() * (14.0 / 14.0), healthArea->getHeight());
    }
    else if (globals.HPPositiveLimit >= 5)
    {
        healthArea->setMaterialName("General/BarColors2");
        barHP->setDimensions(healthArea->getWidth() * (11.0 / 14.0), healthArea->getHeight());
    }
    else //if (globals.HPPositiveLimit >= 3)
    {
        healthArea->setMaterialName("General/BarColors1");
        barHP->setDimensions(healthArea->getWidth() * (9.0 / 14.0), healthArea->getHeight());
    }
    
    indicator->setMetricsMode(GMM_RELATIVE);
    indicator->setPosition(barHP->getLeft(), barHP->getTop() - 0.005);
    indicator->setDimensions(healthArea->getWidth() / 20, globals.HPBarHeight + 0.01);
    indicator->setMaterialName("General/Indicator");
    
    GUITopPanel->setMetricsMode(GMM_RELATIVE);
//    GUITopPanel->setPosition(0.0, 0.0);
//    GUITopPanel->setDimensions(1.0, 0.1);
//    GUITopPanel->setMaterialName("General/GUITopPanel");
    GUITopPanel->setPosition(0.0, 0.0);
    GUITopPanel->setDimensions(1.0, 1.0);
    GUITopPanel->setMaterialName("General/GUIMainHud");
    
    GUILeftPanel->setMetricsMode(GMM_RELATIVE);
    GUILeftPanel->setPosition(0.0, 0.3);
    GUILeftPanel->setDimensions(0.1, 0.5);
    GUILeftPanel->setMaterialName("General/GUILeftPanel");
    
    GUIRightPanel->setMetricsMode(GMM_RELATIVE);
    GUIRightPanel->setPosition(0.9, 0.3);
    GUIRightPanel->setDimensions(0.1, 0.6);
    GUIRightPanel->setMaterialName("General/GUIRightPanel");
    
    float x = 0.285;
    float y = 0.035;
    float wpadding = 0.0050;
    float width = 0.0425;
    float height = 0.075;
    std::vector<CollectionCriteria> criterias = tunnel->getCollectionCriteria();
    for (int i = 0; i < collectionBar.size(); ++i)
    {
        collectionBar[i]->setMetricsMode(GMM_RELATIVE);
        collectionBar[i]->setPosition(x + wpadding, y);
        collectionBar[i]->setDimensions(width, height);
        if (i < criterias.size())
        {
            std::string scoreName = "General/GUICollection" + Util::toStringInt(Util::clamp(3 - (tunnel->getNBack() - criterias[i].nback), 0, 3));
            if (criterias[i].collected)
                scoreName += "Filled";
            else
                scoreName += "Blank";
            collectionBar[i]->setMaterialName(scoreName);
        }
        else
        {
            collectionBar[i]->setMaterialName("General/GUICollectionGreyed");
        }
        x += 2 * wpadding + width;
    }
    
    label1->setMetricsMode(GMM_PIXELS);
    label1->setAlignment(TextAreaOverlayElement::Center);
    label1->setPosition(globals.label1_posX, globals.label1_posY);
    label1->setCharHeight(globals.screenHeight / 40);
    label1->setFontName("Arial");
    label1->setColour(ColourValue::ColourValue(1.0, 1.0, 0.0));
    
    label2->setMetricsMode(GMM_RELATIVE);
    label2->setPosition(0.060, 0.04);
    label2->setCharHeight(0.025);
    label2->setColour(ColourValue::ColourValue(1.0, 1.0, 1.0));
    label2->setFontName("Arial");
    
    label3->setMetricsMode(GMM_RELATIVE);
    label3->setPosition(0.060, 0.08);
    label3->setCharHeight(0.02);
    label3->setColour(ColourValue::ColourValue(1.0, 1.0, 1.0));
    label3->setFontName("Arial");
    
    label4->setMetricsMode(GMM_RELATIVE);
    label4->setPosition(0.775, 0.04);
    label4->setCharHeight(0.02);
    label4->setColour(ColourValue::ColourValue(1.0, 1.0, 1.0));
    label4->setFontName("Arial");
    
    label5->setMetricsMode(GMM_RELATIVE);
    label5->setPosition(0.775, 0.08);
    label5->setCharHeight(0.02);
    label5->setColour(ColourValue::ColourValue(1.0, 1.0, 1.0));
    label5->setFontName("Arial");
    
    label6->setMetricsMode(GMM_PIXELS);
    label6->setAlignment(TextAreaOverlayElement::Center);
    label6->setPosition(globals.label6_posX, globals.label6_posY);
    label6->setCharHeight(globals.screenHeight / 50);
    label6->setColour(ColourValue::ColourValue(1.0, 1.0, 0.0));
    label6->setFontName("Arial");
    
    label7->setMetricsMode(GMM_PIXELS);
    label7->setAlignment(TextAreaOverlayElement::Center);
    label7->setPosition(globals.label7_posX, globals.label7_posY);
    label7->setCharHeight(globals.screenHeight / 50);
    label7->setColour(ColourValue::ColourValue(1.0, 1.0, 0.0));
    label7->setFontName("Arial");
    
    toggle1TextArt->setMetricsMode(GMM_RELATIVE);
    toggle1TextArt->setPosition(0.0250, 0.0025);
    toggle1TextArt->setDimensions(0.03, 0.08);
    
    toggle2TextArt->setMetricsMode(GMM_RELATIVE);
    toggle2TextArt->setPosition(0.0250, 0.0025);
    toggle2TextArt->setDimensions(0.03, 0.08);
    
    toggle3TextArt->setMetricsMode(GMM_RELATIVE);
    toggle3TextArt->setPosition(0.0250, 0.0025);
    toggle3TextArt->setDimensions(0.03, 0.08);
    
    toggle4TextArt->setMetricsMode(GMM_RELATIVE);
    toggle4TextArt->setPosition(0.0250, 0.0025);
    toggle4TextArt->setDimensions(0.03, 0.08);
    toggle4TextArt->setMaterialName("General/GUIToggleTextNumber0");
    
    buttons[BUTTON_PAUSE].setButton("pause", overlays[0], GMM_PIXELS, Vector2(globals.pauseButton_posX, globals.pauseButton_posY), Vector2(globals.pauseButton_width, globals.pauseButton_height), pauseBackground, NULL);
    buttons[BUTTON_TOGGLE1].setButton("toggle1", overlays[0], GMM_RELATIVE, Vector2(0.895, 0.45), Vector2(0.08, 0.08), toggle1Background, toggle1Text);
    buttons[BUTTON_TOGGLE2].setButton("toggle2", overlays[0], GMM_RELATIVE, Vector2(0.895, 0.55), Vector2(0.08, 0.08), toggle2Background, toggle2Text);
    buttons[BUTTON_TOGGLE3].setButton("toggle3", overlays[0], GMM_RELATIVE, Vector2(0.895, 0.65), Vector2(0.08, 0.08), toggle3Background, toggle3Text);
    buttons[BUTTON_TOGGLE4].setButton("toggle4", overlays[0], GMM_RELATIVE, Vector2(0.895, 0.75), Vector2(0.08, 0.08), toggle4Background, toggle4Text);
    float pheight = 0.12;
    float pwidth = pheight * globals.screenHeight / globals.screenWidth;
    buttons[BUTTON_POWERUP1].setButton("powerup1", overlays[0], GMM_RELATIVE, Vector2(0.015, 0.45), Vector2(pwidth, pheight), powerup1Background, NULL);
    buttons[BUTTON_POWERUP2].setButton("powerup2", overlays[0], GMM_RELATIVE, Vector2(0.015, 0.60), Vector2(pwidth, pheight), powerup2Background, NULL);
    buttons[BUTTON_POWERUP3].setButton("powerup3", overlays[0], GMM_RELATIVE, Vector2(0.015, 0.75), Vector2(pwidth, pheight), powerup3Background, NULL);
    
    float qheight = 0.10;
    float qwidth = 0.30;
    buttons[BUTTON_RESUME].setButton("resume", overlays[1], GMM_RELATIVE, Vector2(0.35, 0.30), Vector2(qwidth, qheight), resumeButtonBackground, NULL);
    buttons[BUTTON_NEXT].setButton("next", overlays[1], GMM_RELATIVE, Vector2(0.35, 0.42), Vector2(qwidth, qheight), nextButtonBackground, NULL);
    buttons[BUTTON_RESTART].setButton("restart", overlays[1], GMM_RELATIVE, Vector2(0.35, 0.54), Vector2(qwidth, qheight), restartButtonBackground, NULL);
    buttons[BUTTON_LEVELSELECT].setButton("levelselect", overlays[1], GMM_RELATIVE, Vector2(0.35, 0.66), Vector2(qwidth, qheight), levelSelectButtonBackground, NULL);
    
    toggleIndicator->setMetricsMode(GMM_RELATIVE);
    toggleIndicator->setDimensions(0.08, 0.08);
    toggleIndicator->setMaterialName("General/GUIToggleIndicator");
    switch (player->getToggleBack())
    {
        case 0:
            toggleIndicator->setPosition(0.895, 0.45);
            break;
        case 1:
            toggleIndicator->setPosition(0.895, 0.55);
            break;
        case 2:
            toggleIndicator->setPosition(0.895, 0.65);
            break;
        case 3:
            toggleIndicator->setPosition(0.895, 0.75);
            break;
    }
    
    pauseBackground->setMaterialName("General/PauseButton");
    toggle1Background->setMaterialName("General/GUIToggleButton3");
    toggle2Background->setMaterialName("General/GUIToggleButton2");
    toggle3Background->setMaterialName("General/GUIToggleButton1");
    toggle4Background->setMaterialName("General/GUIToggleButton0");
    powerup1Background->setMaterialName("General/GUIPowerupButtonBlank");
    powerup2Background->setMaterialName("General/GUIPowerupButtonBlank");
    powerup3Background->setMaterialName("General/GUIPowerupButtonBlank");
    
    resumeButtonBackground->setMaterialName("General/ResumeButtonRound");
    nextButtonBackground->setMaterialName("General/NextButtonRound");
    restartButtonBackground->setMaterialName("General/RestartButtonRound");
    levelSelectButtonBackground->setMaterialName("General/LevelSelectButtonRound");
    
    toggleEntireBackground->setMetricsMode(GMM_RELATIVE);
    toggleEntireBackground->setPosition(0.775, 0.25);
    toggleEntireBackground->setDimensions(0.20, 0.50);
    toggleEntireBackground->setMaterialName("General/BaseGray");
    
    // NOTE: should have a button member function called inactivate so it deactivates
    // in the queryGUI function! Hiding currently works since the effect of toggle
    // is negligible in other logic
    if (tunnel->getMode() == STAGE_MODE_RECESS)
    {
        toggle1TextArt->hide();
        toggle2TextArt->hide();
        toggle3TextArt->hide();
        toggle4TextArt->hide();
        toggleIndicator->hide();
    }
    else
    {
        toggle1TextArt->show();
        toggle2TextArt->show();
        toggle3TextArt->show();
        toggle4TextArt->show();
        toggleIndicator->show();
    }
}

void HudStage::link(Player* player, Tunnel* tunnel)
{
    this->player = player;
    this->tunnel = tunnel;
}

void HudStage::unlink()
{
    this->player = NULL;
    this->tunnel = NULL;
}