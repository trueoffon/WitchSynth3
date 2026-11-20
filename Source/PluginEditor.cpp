#include "PluginEditor.h"

namespace Colors
{
    const juce::Colour bg       (0xff07050c);
    const juce::Colour panel    (0xff120e1e);
    const juce::Colour lcdBg    (0xff06040e);
    const juce::Colour border   (0xff412a6b);
    const juce::Colour purple   (0xff9d4edd);
    const juce::Colour neon     (0xffc77dff);
    const juce::Colour glow     (0xffe0aaff);
    const juce::Colour hotPink  (0xffff4d9d);
    const juce::Colour text     (0xffede6f7);
    const juce::Colour subText  (0xff9e8dbb);
}

static juce::String u8(const char* s) { return juce::String::fromUTF8(s); }

static void disableFocusRecursive(juce::Component& comp)
{
    comp.setWantsKeyboardFocus(false);
    comp.setMouseClickGrabsKeyboardFocus(false);
    for (auto* child : comp.getChildren())
        disableFocusRecursive(*child);
}

WitchLookAndFeel::WitchLookAndFeel()
{
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1a142b));
    setColour(juce::ComboBox::outlineColourId, Colors::border);
    setColour(juce::ComboBox::textColourId, Colors::text);
    setColour(juce::ComboBox::arrowColourId, Colors::neon);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff151024));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::purple);
    setColour(juce::PopupMenu::textColourId, Colors::text);
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff201638));
    setColour(juce::TextButton::buttonOnColourId, Colors::purple);
    setColour(juce::TextButton::textColourOffId, Colors::glow);
    setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    setColour(juce::ToggleButton::tickColourId, Colors::neon);
    setColour(juce::ToggleButton::textColourId, Colors::text);
    setColour(juce::Slider::textBoxTextColourId, Colors::glow);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ListBox::backgroundColourId, Colors::lcdBg);
    setColour(juce::ListBox::outlineColourId, Colors::border);
    setColour(juce::ScrollBar::thumbColourId, Colors::purple);
}

void WitchLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                        float pos, float startAngle, float endAngle, juce::Slider&)
{
    const float radius = static_cast<float>(juce::jmin(w, h)) * 0.42f;
    const float cx = static_cast<float>(x) + static_cast<float>(w) * 0.5f;
    const float cy = static_cast<float>(y) + static_cast<float>(h) * 0.48f;
    const float angle = startAngle + pos * (endAngle - startAngle);

    juce::Path track;
    track.addCentredArc(cx, cy, radius, radius, 0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colour(0xff251b3d));
    g.strokePath(track, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path glowArc;
    glowArc.addCentredArc(cx, cy, radius, radius, 0.0f, startAngle, angle, true);
    g.setColour(Colors::purple.withAlpha(0.35f));
    g.strokePath(glowArc, juce::PathStrokeType(5.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour(Colors::neon);
    g.strokePath(glowArc, juce::PathStrokeType(2.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float innerR = radius * 0.70f;
    g.setColour(juce::Colour(0xff191229));
    g.fillEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);
    g.setColour(Colors::border);
    g.drawEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f, 1.0f);

    juce::Path ptr;
    ptr.addRectangle(-1.2f, -innerR + 2.0f, 2.4f, innerR * 0.52f);
    ptr.applyTransform(juce::AffineTransform::rotation(angle).translated(cx, cy));
    g.setColour(Colors::glow);
    g.fillPath(ptr);
}

void WitchListModel::paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected)
{
    if (selected)
    {
        g.setColour(Colors::purple.withAlpha(0.88f));
        g.fillRoundedRectangle(2.0f, 1.0f, static_cast<float>(w) - 4.0f, static_cast<float>(h) - 2.0f, 3.0f);
    }
    else if (row % 2 == 1)
    {
        g.setColour(juce::Colour(0x10ffffff));
        g.fillRect(0, 0, w, h);
    }

    g.setColour(selected ? juce::Colours::white : Colors::text);
    g.setFont(juce::FontOptions(12.0f, selected ? juce::Font::bold : juce::Font::plain));
    if (row >= 0 && row < static_cast<int>(items.size()))
        g.drawText(items[static_cast<size_t>(row)], 8, 0, w - 12, h, juce::Justification::left, true);
}

WitchXYPad::WitchXYPad(juce::AudioProcessorValueTreeState& state) : apvts(state) {}

void WitchXYPad::paint(juce::Graphics& g)
{
    const auto b = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff0a0713));
    g.fillRoundedRectangle(b, 6.0f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(b, 6.0f, 1.2f);

    g.setColour(juce::Colour(0xff23183b));
    for (int i = 1; i < 4; ++i)
    {
        const float gx = b.getX() + b.getWidth() * (static_cast<float>(i) * 0.25f);
        const float gy = b.getY() + b.getHeight() * (static_cast<float>(i) * 0.25f);
        g.drawVerticalLine(static_cast<int>(gx), b.getY() + 2.0f, b.getBottom() - 2.0f);
        g.drawHorizontalLine(static_cast<int>(gy), b.getX() + 2.0f, b.getRight() - 2.0f);
    }

    const float mx = apvts.getRawParameterValue("macroX")->load();
    const float my = apvts.getRawParameterValue("macroY")->load();
    const float px = b.getX() + 8.0f + mx * (b.getWidth() - 16.0f);
    const float py = b.getBottom() - 8.0f - my * (b.getHeight() - 16.0f);

    g.setColour(Colors::purple.withAlpha(0.35f));
    g.drawHorizontalLine(static_cast<int>(py), b.getX() + 2.0f, b.getRight() - 2.0f);
    g.drawVerticalLine(static_cast<int>(px), b.getY() + 2.0f, b.getBottom() - 2.0f);

    g.setColour(Colors::purple.withAlpha(0.30f));
    g.fillEllipse(px - 18.0f, py - 18.0f, 36.0f, 36.0f);
    g.setColour(Colors::neon);
    g.fillEllipse(px - 6.0f, py - 6.0f, 12.0f, 12.0f);
    g.setColour(Colors::glow);
    g.drawEllipse(px - 8.0f, py - 8.0f, 16.0f, 16.0f, 1.5f);

    g.setColour(Colors::subText);
    g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    g.drawText("DIMENSION", static_cast<int>(b.getX()) + 8, static_cast<int>(b.getY()) + 6, 80, 14, juce::Justification::left);
    g.drawText("CHAOS", static_cast<int>(b.getRight()) - 72, static_cast<int>(b.getBottom()) - 18, 64, 14, juce::Justification::right);
}

void WitchXYPad::mouseDown(const juce::MouseEvent& e) { updateFromMouse(e); }
void WitchXYPad::mouseDrag(const juce::MouseEvent& e) { updateFromMouse(e); }

void WitchXYPad::updateFromMouse(const juce::MouseEvent& e)
{
    const float nx = juce::jlimit(0.0f, 1.0f, (static_cast<float>(e.x) - 8.0f) / juce::jmax(1.0f, static_cast<float>(getWidth() - 16)));
    const float ny = juce::jlimit(0.0f, 1.0f, 1.0f - (static_cast<float>(e.y) - 8.0f) / juce::jmax(1.0f, static_cast<float>(getHeight() - 16)));
    if (auto* px = apvts.getParameter("macroX")) px->setValueNotifyingHost(nx);
    if (auto* py = apvts.getParameter("macroY")) py->setValueNotifyingHost(ny);
    repaint();
}

WitchStepMatrix::WitchStepMatrix(juce::AudioProcessorValueTreeState& state, std::atomic<int>& stepRef)
    : apvts(state), activeStep(stepRef) {}

void WitchStepMatrix::paint(juce::Graphics& g)
{
    const float w = static_cast<float>(getWidth()) / 8.0f;
    const float h = static_cast<float>(getHeight());
    const int arpMode = static_cast<int>(apvts.getRawParameterValue("arpMode")->load());
    const int cur = activeStep.load(std::memory_order_relaxed);
    static constexpr const char* lbls[4] = { "--", "ON", "+12", "2X" };

    for (int i = 0; i < 8; ++i)
    {
        const int val = static_cast<int>(apvts.getRawParameterValue("seq" + juce::String(i))->load());
        const juce::Rectangle<float> r(static_cast<float>(i) * w + 2.0f, 2.0f, w - 4.0f, h - 4.0f);

        juce::Colour c(0xff1b1529);
        if (val == 1) c = Colors::purple;
        else if (val == 2) c = Colors::neon;
        else if (val == 3) c = Colors::hotPink;

        const bool isPlayhead = (arpMode > 0) && (i == cur);
        g.setColour(isPlayhead ? c.brighter(0.35f) : c.withAlpha(val == 0 ? 0.85f : 0.72f));
        g.fillRoundedRectangle(r, 4.0f);
        g.setColour(isPlayhead ? juce::Colours::white : Colors::border);
        g.drawRoundedRectangle(r, 4.0f, isPlayhead ? 1.6f : 1.0f);

        g.setColour(val == 0 ? Colors::subText : juce::Colours::white);
        g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
        g.drawText(lbls[juce::jlimit(0, 3, val)], r, juce::Justification::centred);
    }
}

void WitchStepMatrix::mouseDown(const juce::MouseEvent& e)
{
    const int idx = juce::jlimit(0, 7, static_cast<int>(static_cast<float>(e.x) / (static_cast<float>(getWidth()) / 8.0f)));
    if (auto* p = apvts.getParameter("seq" + juce::String(idx)))
    {
        const int cur = static_cast<int>(apvts.getRawParameterValue("seq" + juce::String(idx))->load());
        const int next = (cur + 1) % 4;
        p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(next)));
        repaint();
    }
}

WitchSynthAudioProcessorEditor::WitchSynthAudioProcessorEditor(WitchSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), xyPad(p.apvts), stepMatrix(p.apvts, p.arpStepDisplay)
{
    setLookAndFeel(&lnf);
    activeHelpText = u8("Наведите курсор на любую ручку, категорию или кнопку — здесь появится описание на русском языке.");

    categoryModel.items.push_back(u8("\u2605 Favorites"));
    const auto& folders = getFactoryFolders();
    for (int i = 0; i < static_cast<int>(folders.size()); ++i)
        categoryModel.items.push_back(juce::String(i + 1).paddedLeft('0', 2) + ". " + folders[static_cast<size_t>(i)]);

    categoryModel.onRowClick = [this](int row)
    {
        if (row >= 0 && row < static_cast<int>(categoryModel.items.size()))
        {
            selectedUiCategory = row;
            rebuildPresetListForCategory(selectedUiCategory);
            if (!visiblePresetIndices.empty())
            {
                presetList.selectRow(0, false, true);
                audioProcessor.loadPreset(visiblePresetIndices[0]);
                lastSeenPreset = audioProcessor.getCurrentPresetIndex();
                updateLikeButtonVisual();
                repaint(332, 12, 496, 382);
            }
        }
        giveAwayKeyboardFocus();
    };

    presetModel.onRowClick = [this](int row)
    {
        if (row >= 0 && row < static_cast<int>(visiblePresetIndices.size()))
        {
            audioProcessor.loadPreset(visiblePresetIndices[static_cast<size_t>(row)]);
            lastSeenPreset = audioProcessor.getCurrentPresetIndex();
            updateLikeButtonVisual();
            repaint(332, 12, 496, 382);
        }
        giveAwayKeyboardFocus();
    };

    categoryList.setRowHeight(23);
    presetList.setRowHeight(22);
    categoryList.setOutlineThickness(1);
    presetList.setOutlineThickness(1);
    addAndMakeVisible(categoryList);
    addAndMakeVisible(presetList);

    prevBtn.onClick = [this]
    {
        const int total = static_cast<int>(getAllPresets().size());
        audioProcessor.loadPreset((audioProcessor.getCurrentPresetIndex() - 1 + total) % total);
        syncPresetUI();
        giveAwayKeyboardFocus();
    };
    nextBtn.onClick = [this]
    {
        const int total = static_cast<int>(getAllPresets().size());
        audioProcessor.loadPreset((audioProcessor.getCurrentPresetIndex() + 1) % total);
        syncPresetUI();
        giveAwayKeyboardFocus();
    };
    likeBtn.onClick = [this]
    {
        audioProcessor.toggleFavorite(audioProcessor.getCurrentPresetIndex());
        updateLikeButtonVisual();
        rebuildPresetListForCategory(selectedUiCategory);
        syncPresetUI();
        giveAwayKeyboardFocus();
    };
    rndBtn.onClick = [this] { randomizeWitchPreset(); giveAwayKeyboardFocus(); };
    initBtn.onClick = [this] { audioProcessor.initPatch(); giveAwayKeyboardFocus(); };
    panicBtn.onClick = [this] { audioProcessor.triggerPanic(); giveAwayKeyboardFocus(); };

    lockFxBtn.setClickingTogglesState(true);
    lockFxBtn.setToggleState(audioProcessor.lockFx.load(), juce::dontSendNotification);
    lockFxBtn.onClick = [this] { audioProcessor.lockFx.store(lockFxBtn.getToggleState()); giveAwayKeyboardFocus(); };

    pageMainBtn.onClick = [this] { setSidePage(SidePage::Main); giveAwayKeyboardFocus(); };
    pageOscBtn.onClick  = [this] { setSidePage(SidePage::Osc); giveAwayKeyboardFocus(); };
    pageArpBtn.onClick  = [this] { setSidePage(SidePage::Arp); giveAwayKeyboardFocus(); };
    pageXyBtn.onClick   = [this] { setSidePage(SidePage::XY); giveAwayKeyboardFocus(); };

    addAndMakeVisible(prevBtn); addAndMakeVisible(nextBtn); addAndMakeVisible(likeBtn);
    addAndMakeVisible(rndBtn);  addAndMakeVisible(initBtn); addAndMakeVisible(lockFxBtn); addAndMakeVisible(panicBtn);
    addAndMakeVisible(pageMainBtn); addAndMakeVisible(pageOscBtn); addAndMakeVisible(pageArpBtn); addAndMakeVisible(pageXyBtn);

    registerHelp(&categoryList, u8("Категории звуков: выберите тип звука (Bass, Lead, Pad, Pluck, Arp, Choir, FX) или Избранное (\u2605 Favorites)."));
    registerHelp(&presetList, u8("Список звуков: кликните по любому пресету в окне, чтобы мгновенно загрузить его."));
    registerHelp(&prevBtn, u8("Предыдущий пресет."));
    registerHelp(&nextBtn, u8("Следующий пресет."));
    registerHelp(&likeBtn, u8("Лайк (\u2665): добавляет или удаляет текущий звук в папку \u2605 Favorites (сохраняется на диск)."));
    registerHelp(&rndBtn, u8("RND: случайная генерация мрачного Witch House звука и ритма секвенсора."));
    registerHelp(&initBtn, u8("INIT: сброс всех ручек в чистый звук для создания патча с нуля."));
    registerHelp(&lockFxBtn, u8("LOCK FX: сохраняет настройки эффектов и XY-пада при переключении звуков."));
    registerHelp(&panicBtn, u8("PANIC: мгновенно глушит зависшие ноты, Freeze и бесконечный дилей."));
    registerHelp(&pageMainBtn, u8("Вкладка MAIN: фильтр, тональность, огибающая громкости и Morph A\u2194B."));
    registerHelp(&pageOscBtn, u8("Вкладка OSC: настройка волн OSC 1 / OSC 2, саб-баса и физики 2040 (Entropy, Gravity, Ghost)."));
    registerHelp(&pageArpBtn, u8("Вкладка ARP: LFO-модуляция, сайдчейн Pump и 8-шаговый секвенсор с дробями (Ratchets)."));
    registerHelp(&pageXyBtn, u8("Вкладка XY: 2D-пад Aether Core для мгновенного морфинга агрессии и пространства."));

    // Side Page 1: MAIN
    initCombo(filterTypeBox, "filterType", { "LowPass", "HighPass", "BandPass", "Formant", "Comb", "DualAcid" },
        u8("Тип фильтра: LP/HP/BP, Formant (гласные), Comb (струны по нотам) и DualAcid 24dB."));
    initKnob(filterCutoff, "filterCutoff", "CUTOFF", u8("CUTOFF: частота среза фильтра (в Comb меняет яркость струны, в Formant — гласную)."));
    initKnob(filterReso, "filterReso", "RESO", u8("RESONANCE: резонансный пик на срезе (в Comb — длительность звучания струны)."));
    initKnob(filterDrive, "filterDrive", "DRIVE", u8("Перегруз на входе фильтра."));
    initKnob(filterEnvAmt, "filterEnvAmt", "ENV AMT", u8("Глубина огибающей фильтра в начале ноты."));
    initKnob(filterDecay, "filterDecay", "F.DECAY", u8("Время закрытия фильтра после нажатия ноты."));
    initKnob(filterKeyTrack, "filterKeyTrack", "K.TRK", u8("KEYTRACK: отслеживание высоты ноты (на высоких нотах фильтр открывается ярче)."));
    initKnob(tiltAmt, "tiltAmt", "TILT", u8("TILT EQ: баланс спектра (влево — темнее/жирнее, вправо — ярче и холоднее)."));
    initKnob(stereoWidth, "stereoWidth", "WIDTH", u8("WIDTH: ширина стереопанорамы (от сухого моно до широкого объёма)."));

    initCombo(polyModeBox, "polyMode", { "Poly", "Mono" }, u8("Режим: Poly (аккорды до 8 нот) или Mono (одноголосный легато-бас/лид)."));
    const auto& allP = getAllPresets();
    for (int i = 0; i < static_cast<int>(allP.size()); ++i)
        morphPresetBox.addItem("B: " + juce::String(allP[static_cast<size_t>(i)].name), i + 1);
    addAndMakeVisible(morphPresetBox);
    comboAtts.push_back(std::make_unique<ComboAtt>(audioProcessor.apvts, "morphPreset", morphPresetBox));
    registerHelp(&morphPresetBox, u8("Цель B для морфинга: выберите звук, в который будет превращаться текущий пресет."));

    initKnob(ampA, "ampAttack", "ATTACK", u8("ATTACK: время нарастания громкости ноты."));
    initKnob(ampD, "ampDecay", "DECAY", u8("DECAY: время спада от пика к уровню Sustain."));
    initKnob(ampS, "ampSustain", "SUSTAIN", u8("SUSTAIN: громкость, пока клавиша зажата."));
    initKnob(ampR, "ampRelease", "RELEASE", u8("RELEASE: длина хвоста после отпускания клавиши."));
    initKnob(glideTime, "glideTime", "GLIDE", u8("GLIDE: плавное скольжение (Portamento) от одной ноты к другой."));
    initKnob(snapAmt, "snapAmt", "SNAP", u8("SNAP: усилитель щелчка атаки (первые 15 мс) для пробивных плаков и басов."));
    initKnob(airAmt, "airAmt", "AIR", u8("CRYSTAL AIR: эксайтер высоких частот (5.5+ kHz) для стеклянного воздуха."));
    initKnob(morphAmt, "morphAmt", "MORPH B", u8("MORPH A\u2194B: плавно превращает текущий звук в пресет B в реальном времени!"));

    // Side Page 2: OSC & 2040
    juce::StringArray waves{ "Saw", "Pulse", "Tri", "NES4", "Sine", "Spectral", "ChaosPWM", "AcidForm" };
    initCombo(osc1WaveBox, "osc1Wave", waves, u8("Волна OSC 1 (Saw, Pulse, Tri, NES4, Sine, Spectral, ChaosPWM, AcidForm)."));
    initCombo(osc2WaveBox, "osc2Wave", waves, u8("Волна OSC 2 для наслоения и FM-модуляции."));
    initKnob(osc1Oct, "osc1Oct", "O1 OCT", u8("Октава 1-го осциллятора (-2..+2)."));
    initKnob(osc1Semi, "osc1Semi", "O1 SEMI", u8("Полутона (Pitch) 1-го осциллятора (-12..+12)."));
    initKnob(pwmWidth, "pwmWidth", "MORPH", u8("MORPH / PWM: меняет ширину импульса и форму волны."));
    initKnob(xModAmt, "xModAmt", "FM/SYNC", u8("FM/SYNC: перекрёстная модуляция OSC 1 от OSC 2."));
    initKnob(osc1Detune, "osc1Detune", "DETUNE", u8("DETUNE: расстройка слоёв унисона (жирный широкий звук)."));
    initKnob(osc1Level, "osc1Level", "O1 LVL", u8("Громкость 1-го осциллятора."));
    initKnob(osc2Oct, "osc2Oct", "O2 OCT", u8("Октава 2-го осциллятора (-2..+2)."));
    initKnob(osc2Semi, "osc2Semi", "O2 SEMI", u8("Полутона (Pitch) 2-го осциллятора (например, +7 квинта)."));
    initKnob(osc2Unison, "osc2Unison", "UNISON", u8("UNISON: количество голосов унисона (1–7)."));
    initKnob(osc2Level, "osc2Level", "O2 LVL", u8("Громкость 2-го осциллятора."));
    initKnob(subLevel, "subLevel", "SUB LVL", u8("SUB: мощный саб-бас ровно на -1 октаву ниже ноты."));
    initKnob(entropyAmt, "entropyAmt", "ENTROPY", u8("ENTROPY: живая микро-расстройка аналоговых компонентов для каждой ноты."));
    initKnob(gravityAmt, "gravityAmt", "GRAVITY", u8("GRAVITY: гравитационное проседание питча и орбит от энергии баса."));
    initKnob(ghostVoxAmt, "ghostVoxAmt", "GHOST", u8("GHOST VOX: гармонические квинтовые и октавные тени в хвосте ноты."));

    // Side Page 3: ARP & LFO
    initCombo(lfoTargetBox, "lfoTarget", { "Pitch", "Cutoff", "Morph" }, u8("Цель LFO: Pitch (вибрато), Cutoff (срез фильтра) или Morph (форма волны)."));
    initCombo(lfoShapeBox, "lfoShape", { "Sine", "Square", "Ramp", "S&H", "BioChaos" }, u8("Форма LFO: плавная, ступеньки S&H-глитча или живой аттрактор Лоренца (BioChaos)."));
    initKnob(lfoRate, "lfoRate", "RATE", u8("RATE: скорость LFO."));
    initKnob(lfoDepth, "lfoDepth", "DEPTH", u8("DEPTH: глубина воздействия LFO на выбранную цель."));
    initKnob(pitchPunch, "pitchPunch", "P.PUNCH", u8("PITCH PUNCH: удар питча в самом начале ноты (для 808 и лазеров)."));
    initKnob(pumpAmt, "pumpAmt", "PUMP", u8("AUTO PUMP: встроенный BPM-сайдчейн под темп FL Studio."));
    initKnob(noiseLevel, "noiseLevel", "NOISE", u8("NOISE: шум кассеты и воздуха (или основа для Comb-струн)."));
    initKnob(vhsDrift, "vhsDrift", "DRIFT", u8("VHS DRIFT: плавание питча старой видеокассеты."));

    initCombo(arpModeBox, "arpMode", { "ArpOff", "Stutter", "ChipArp", "Chaos" }, u8("Режим ритма: Off, Stutter (рубка аккорда), ChipArp (8-бит арп), Chaos."));
    initKnob(arpRate, "arpRate", "ARP SPD", u8("ARP SPD: сетка шагов (1/4, 1/8, триоли 1/8T, 1/16, 1/16T, 1/32)."));
    addAndMakeVisible(stepMatrix);
    registerHelp(&stepMatrix, u8("8-шаговый секвенсор: кликайте по кубикам (-- пауза, ON удар, +12 октава, 2X двойная дробь)."));

    // Side Page 4: XY CORE
    addAndMakeVisible(xyPad);
    registerHelp(&xyPad, u8("AETHER XY: по X (CHAOS) — агрессия и перегруз, по Y (DIMENSION) — космос и шиммер."));

    // Bottom Separated FX Controls (always visible)
    initToggle(distBtn, "distOn", u8("Включает блок перегруза/сатурации."));
    initCombo(distModeBox, "distMode", { "Tube", "Diode", "Rectify", "FoldX", "BitClip" }, u8("Режим перегруза (2x оверсэмплинг): лампа, диод RAT, Rectify, FoldX, BitClip."));
    initKnob(distDrive, "distDrive", "DRIVE", u8("Сила перегруза."));
    initKnob(distMix, "distMix", "D.MIX", u8("Баланс между чистым басом и перегрузом."));
    initKnob(crushAmount, "crushAmount", "CRUSH", u8("Bitcrusher: понижение битности для 8-битного звука."));
    initKnob(lowCutHz, "lowCutHz", "LOW CUT", u8("Срез лишнего низа (20–250 Hz), чтобы не мешать 808-й бочке."));

    initToggle(chorusBtn, "chorusOn", u8("Включает стерео-хорус."));
    initKnob(chorusMix, "chorusMix", "MIX", u8("Громкость стерео-хоруса."));
    initKnob(chorusRate, "chorusRate", "RATE", u8("Скорость модуляции хоруса."));
    initKnob(chorusDepth, "chorusDepth", "DEPTH", u8("Глубина хоруса."));

    initToggle(delayBtn, "delayOn", u8("Включает эхо (Delay)."));
    initCombo(delayModeBox, "delayMode", { "Stereo", "PingPong", "GhostOct" }, u8("Режим эха: Stereo, PingPong (перекрёстное) или GhostOct (улетает на октаву вверх)."));
    initKnob(delayTime, "delayTime", "TIME", u8("Время задержки эха."));
    initKnob(delayFb, "delayFeedback", "F.BACK", u8("Количество повторов эха (Feedback)."));
    initKnob(delayMix, "delayMix", "MIX", u8("Громкость эха."));

    initToggle(reverbBtn, "reverbOn", u8("Включает ревербератор."));
    initToggle(freezeBtn, "reverbFreeze", u8("FREEZE: замораживает хвост реверба в бесконечный дрон."));
    initKnob(reverbSize, "reverbSize", "SIZE", u8("Размер помещения ревербератора."));
    initKnob(reverbMix, "reverbMix", "MIX", u8("Громкость реверберации."));
    initKnob(haloAmt, "haloAmt", "HALO", u8("SHIMMER HALO: добавляет в реверб октавное космическое свечение."));

    initKnob(masterGain, "masterGain", "MASTER", u8("Общая громкость синтезатора с мягким лимитером."));
    initToggle(tapeStopBtn, "tapeStop", u8("TAPE STOP: эффект зажёванной кассеты — роняет питч в ноль (можно автоматизировать)."));

    addMouseListener(this, true);
    setSidePage(SidePage::Main);
    syncPresetUI();
    disableFocusRecursive(*this);
    startTimerHz(30);
    setSize(1160, 720);
}

WitchSynthAudioProcessorEditor::~WitchSynthAudioProcessorEditor()
{
    removeMouseListener(this);
    stopTimer();
    setLookAndFeel(nullptr);
}

void WitchSynthAudioProcessorEditor::registerHelp(juce::Component* c, const juce::String& ruHelp)
{
    if (c != nullptr) helpMap[c] = ruHelp;
}

void WitchSynthAudioProcessorEditor::mouseEnter(const juce::MouseEvent& e)
{
    auto* c = e.eventComponent;
    while (c != nullptr && c != this)
    {
        auto it = helpMap.find(c);
        if (it != helpMap.end())
        {
            if (activeHelpText != it->second)
            {
                activeHelpText = it->second;
                repaint(12, 676, 1136, 36);
            }
            return;
        }
        c = c->getParentComponent();
    }
}

void WitchSynthAudioProcessorEditor::mouseExit(const juce::MouseEvent&) {}

void WitchSynthAudioProcessorEditor::initKnob(juce::Slider& s, const juce::String& id, const juce::String& name, const juce::String& ruHelp, bool attachLbl)
{
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 12);
    s.setWantsKeyboardFocus(false);
    s.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(s);
    sliderAtts.push_back(std::make_unique<SliderAtt>(audioProcessor.apvts, id, s));
    registerHelp(&s, ruHelp);

    if (attachLbl)
    {
        auto lbl = std::make_unique<juce::Label>("", name);
        lbl->setJustificationType(juce::Justification::centred);
        lbl->setColour(juce::Label::textColourId, Colors::subText);
        lbl->setFont(juce::FontOptions(9.5f, juce::Font::bold));
        lbl->attachToComponent(&s, false);
        lbl->setWantsKeyboardFocus(false);
        lbl->setMouseClickGrabsKeyboardFocus(false);
        addAndMakeVisible(*lbl);
        registerHelp(lbl.get(), ruHelp);
        knobLabels[&s] = std::move(lbl);
    }
}

void WitchSynthAudioProcessorEditor::initCombo(juce::ComboBox& c, const juce::String& id, const juce::StringArray& items, const juce::String& ruHelp)
{
    for (int i = 0; i < items.size(); ++i)
        c.addItem(items[i], i + 1);
    c.setWantsKeyboardFocus(false);
    c.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(c);
    comboAtts.push_back(std::make_unique<ComboAtt>(audioProcessor.apvts, id, c));
    registerHelp(&c, ruHelp);
}

void WitchSynthAudioProcessorEditor::initToggle(juce::ToggleButton& b, const juce::String& id, const juce::String& ruHelp)
{
    b.setWantsKeyboardFocus(false);
    b.setMouseClickGrabsKeyboardFocus(false);
    addAndMakeVisible(b);
    btnAtts.push_back(std::make_unique<BtnAtt>(audioProcessor.apvts, id, b));
    registerHelp(&b, ruHelp);
}

void WitchSynthAudioProcessorEditor::showCtrl(juce::Component& c, bool visible)
{
    c.setVisible(visible);
    if (auto it = knobLabels.find(&c); it != knobLabels.end())
        it->second->setVisible(visible);
}

void WitchSynthAudioProcessorEditor::setSidePage(SidePage page)
{
    currentPage = page;
    auto styleTab = [&](juce::TextButton& b, bool active)
    {
        b.setColour(juce::TextButton::buttonColourId, active ? Colors::purple : juce::Colour(0xff201638));
        b.setColour(juce::TextButton::textColourOffId, active ? juce::Colours::white : Colors::glow);
    };
    styleTab(pageMainBtn, currentPage == SidePage::Main);
    styleTab(pageOscBtn,  currentPage == SidePage::Osc);
    styleTab(pageArpBtn,  currentPage == SidePage::Arp);
    styleTab(pageXyBtn,   currentPage == SidePage::XY);

    updateVisibility();
    repaint();
}

void WitchSynthAudioProcessorEditor::updateVisibility()
{
    const bool m = (currentPage == SidePage::Main);
    const bool o = (currentPage == SidePage::Osc);
    const bool a = (currentPage == SidePage::Arp);
    const bool x = (currentPage == SidePage::XY);

    showCtrl(filterTypeBox, m); showCtrl(filterCutoff, m); showCtrl(filterReso, m); showCtrl(filterDrive, m);
    showCtrl(filterEnvAmt, m); showCtrl(filterDecay, m); showCtrl(filterKeyTrack, m); showCtrl(tiltAmt, m); showCtrl(stereoWidth, m);
    showCtrl(polyModeBox, m); showCtrl(morphPresetBox, m); showCtrl(ampA, m); showCtrl(ampD, m); showCtrl(ampS, m);
    showCtrl(ampR, m); showCtrl(glideTime, m); showCtrl(snapAmt, m); showCtrl(airAmt, m); showCtrl(morphAmt, m);

    showCtrl(osc1WaveBox, o); showCtrl(osc2WaveBox, o);
    showCtrl(osc1Oct, o); showCtrl(osc1Semi, o); showCtrl(pwmWidth, o); showCtrl(xModAmt, o); showCtrl(osc1Detune, o); showCtrl(osc1Level, o);
    showCtrl(osc2Oct, o); showCtrl(osc2Semi, o); showCtrl(osc2Unison, o); showCtrl(osc2Level, o);
    showCtrl(subLevel, o); showCtrl(entropyAmt, o); showCtrl(gravityAmt, o); showCtrl(ghostVoxAmt, o);

    showCtrl(lfoTargetBox, a); showCtrl(lfoShapeBox, a); showCtrl(lfoRate, a); showCtrl(lfoDepth, a);
    showCtrl(pitchPunch, a); showCtrl(pumpAmt, a); showCtrl(noiseLevel, a); showCtrl(vhsDrift, a);
    showCtrl(arpModeBox, a); showCtrl(arpRate, a); showCtrl(stepMatrix, a);

    showCtrl(xyPad, x);
}

void WitchSynthAudioProcessorEditor::updateLikeButtonVisual()
{
    const bool fav = audioProcessor.isFavorite(audioProcessor.getCurrentPresetIndex());
    likeBtn.setButtonText(fav ? u8("\u2665") : u8("\u2661"));
    likeBtn.setColour(juce::TextButton::buttonColourId, fav ? Colors::hotPink : juce::Colour(0xff201638));
    likeBtn.setColour(juce::TextButton::textColourOffId, fav ? juce::Colours::white : Colors::glow);
}

void WitchSynthAudioProcessorEditor::rebuildPresetListForCategory(int uiCategoryIdx)
{
    presetModel.items.clear();
    visiblePresetIndices.clear();
    const auto& presets = getAllPresets();

    if (uiCategoryIdx == 0)
    {
        auto favs = audioProcessor.getFavoriteIndices();
        if (favs.empty())
        {
            presetModel.items.push_back(u8("-- Нет лайков (нажми \u2661) --"));
        }
        else
        {
            for (int pIdx : favs)
            {
                const juce::String heart = u8("\u2665 ");
                presetModel.items.push_back(heart + presets[static_cast<size_t>(pIdx)].name);
                visiblePresetIndices.push_back(pIdx);
            }
        }
    }
    else
    {
        const int factoryCat = uiCategoryIdx - 1;
        for (int i = 0; i < static_cast<int>(presets.size()); ++i)
        {
            if (presets[static_cast<size_t>(i)].folderIndex == factoryCat)
            {
                const juce::String prefix = audioProcessor.isFavorite(i) ? u8("\u2665 ") : "";
                presetModel.items.push_back(prefix + presets[static_cast<size_t>(i)].name);
                visiblePresetIndices.push_back(i);
            }
        }
    }

    presetList.updateContent();
    presetList.repaint();
    disableFocusRecursive(categoryList);
    disableFocusRecursive(presetList);
}

void WitchSynthAudioProcessorEditor::syncPresetUI()
{
    const int idx = audioProcessor.getCurrentPresetIndex();
    lastSeenPreset = idx;
    const auto& presets = getAllPresets();
    if (idx >= 0 && idx < static_cast<int>(presets.size()))
    {
        const bool stayInFavs = (selectedUiCategory == 0) && audioProcessor.isFavorite(idx);
        selectedUiCategory = stayInFavs ? 0 : (presets[static_cast<size_t>(idx)].folderIndex + 1);
        categoryList.selectRow(selectedUiCategory, false, true);
        rebuildPresetListForCategory(selectedUiCategory);

        for (size_t row = 0; row < visiblePresetIndices.size(); ++row)
        {
            if (visiblePresetIndices[row] == idx)
            {
                presetList.selectRow(static_cast<int>(row), false, true);
                break;
            }
        }
    }
    updateLikeButtonVisual();
    repaint(332, 12, 496, 382);
}

void WitchSynthAudioProcessorEditor::randomizeWitchPreset()
{
    auto& rng = juce::Random::getSystemRandom();
    auto setP = [this](const char* id, float val)
    {
        if (auto* param = audioProcessor.apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(val));
    };

    setP("osc1Wave", static_cast<float>(rng.nextInt(8)));
    setP("osc1Oct", static_cast<float>(rng.nextInt(3) - 1));
    setP("osc1Detune", 0.15f + rng.nextFloat() * 0.65f);
    setP("pwmWidth", 0.15f + rng.nextFloat() * 0.65f);
    setP("xModAmt", rng.nextFloat() < 0.35f ? rng.nextFloat() * 0.35f : 0.04f);
    setP("osc2Wave", static_cast<float>(rng.nextInt(8)));
    setP("osc2Oct", static_cast<float>(rng.nextInt(3) - 1));
    setP("osc2Unison", static_cast<float>(2 + rng.nextInt(6)));
    setP("subLevel", rng.nextFloat() * 0.75f);
    setP("entropyAmt", 0.15f + rng.nextFloat() * 0.45f);
    setP("gravityAmt", rng.nextFloat() * 0.45f);
    setP("ghostVoxAmt", rng.nextFloat() * 0.45f);
    setP("crushAmount", rng.nextFloat() < 0.35f ? rng.nextFloat() * 0.35f : 0.0f);
    setP("tiltAmt", rng.nextFloat() * 0.5f - 0.15f);
    const int fPick = rng.nextInt(6);
    setP("filterType", static_cast<float>(fPick == 1 ? 5 : fPick));
    setP("filterCutoff", 750.0f + rng.nextFloat() * 6500.0f);
    setP("filterReso", 1.5f + rng.nextFloat() * 5.0f);
    setP("filterKeyTrack", 0.6f + rng.nextFloat() * 0.4f);
    setP("filterEnvAmt", 0.15f + rng.nextFloat() * 0.55f);
    setP("pitchPunch", rng.nextFloat() < 0.4f ? 0.1f + rng.nextFloat() * 0.4f : 0.05f);
    setP("snapAmt", rng.nextFloat() * 0.55f);
    setP("airAmt", 0.35f + rng.nextFloat() * 0.45f);
    setP("lfoRate", 0.8f + rng.nextFloat() * 6.0f);
    setP("lfoDepth", 0.1f + rng.nextFloat() * 0.40f);
    setP("lfoShape", static_cast<float>(rng.nextInt(5)));
    setP("arpMode", rng.nextFloat() < 0.30f ? static_cast<float>(1 + rng.nextInt(3)) : 0.0f);
    setP("arpRate", static_cast<float>(rng.nextInt(6)));
    for (int s = 0; s < 8; ++s)
        setP(("seq" + juce::String(s)).toRawUTF8(), static_cast<float>(rng.nextFloat() < 0.2f ? 0 : (1 + rng.nextInt(3))));
    setP("distOn", 1.0f);
    setP("distMode", static_cast<float>(rng.nextInt(5)));
    setP("distDrive", 0.25f + rng.nextFloat() * 0.50f);
    setP("delayMode", static_cast<float>(rng.nextInt(3)));
    setP("reverbOn", 1.0f);
    setP("haloAmt", rng.nextFloat() * 0.65f);
}

void WitchSynthAudioProcessorEditor::timerCallback()
{
    if (lastSeenPreset != audioProcessor.getCurrentPresetIndex())
        syncPresetUI();

    const float rawPeak = audioProcessor.peakLevel.load(std::memory_order_relaxed);
    smoothedPeak = rawPeak > smoothedPeak ? rawPeak : smoothedPeak * 0.85f;

    repaint(634, 322, 188, 66);
    repaint(1110, 440, 26, 152);
    if (currentPage == SidePage::Arp) stepMatrix.repaint();
    if (currentPage == SidePage::XY)  xyPad.repaint();
}

void WitchSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(Colors::bg);

    auto drawPanel = [&](int x, int y, int w, int h, const juce::String& title)
    {
        g.setColour(Colors::panel);
        g.fillRoundedRectangle(static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h), 6.0f);
        g.setColour(Colors::border);
        g.drawRoundedRectangle(static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h), 6.0f, 1.2f);
        g.setColour(Colors::neon);
        g.setFont(juce::FontOptions(11.5f, juce::Font::bold));
        g.drawText(title, x + 10, y + 6, w - 20, 18, juce::Justification::left);
    };

    juce::String leftTitle = "FILTER & TONE";
    juce::String rightTitle = "AMP, VOICING & MORPH A<->B";
    if (currentPage == SidePage::Osc) { leftTitle = "OSC 1 & OSC 2"; rightTitle = "SUB & 2040 PHYSICS"; }
    else if (currentPage == SidePage::Arp) { leftTitle = "BIO-LFO, PUMP & TAPE"; rightTitle = "ARP & 8-STEP RATCHET"; }
    else if (currentPage == SidePage::XY) { leftTitle = "AETHER CORE (XY)"; rightTitle = "2040 MACRO MATRIX"; }

    drawPanel(12, 12, 310, 382, leftTitle);
    drawPanel(838, 12, 310, 382, rightTitle);

    if (currentPage == SidePage::XY)
    {
        g.setColour(Colors::subText);
        g.setFont(juce::FontOptions(12.5f));
        g.drawFittedText(u8("AETHER CORE (XY) управляет звуком в реальном времени:\n\n"
                            "• Ось X (CHAOS): одновременно повышает агрессию перегруза, "
                            "FM/Sync, срез фильтра и Bitcrusher.\n\n"
                            "• Ось Y (DIMENSION): раскрывает реверберацию, Shimmer Halo, "
                            "эхо, стерео-ширину и Crystal Air.\n\n"
                            "Обе оси поддерживают запись автоматизации в FL Studio!"),
                         854, 56, 278, 280, juce::Justification::topLeft, 12);
    }
    else if (currentPage == SidePage::Arp)
    {
        g.setColour(Colors::subText);
        g.setFont(juce::FontOptions(11.0f));
        g.drawText(u8("Кликай шаги ниже: -- (пауза) | ON | +12 | 2X (дробь)"), 850, 178, 286, 18, juce::Justification::centred);
    }

    g.setColour(Colors::panel);
    g.fillRoundedRectangle(332.0f, 12.0f, 496.0f, 382.0f, 7.0f);
    g.setColour(Colors::purple.withAlpha(0.75f));
    g.drawRoundedRectangle(332.0f, 12.0f, 496.0f, 382.0f, 7.0f, 1.5f);

    g.setColour(Colors::neon);
    juce::Path tri;
    tri.addTriangle(354.0f, 20.0f, 343.0f, 44.0f, 365.0f, 44.0f);
    g.strokePath(tri, juce::PathStrokeType(1.7f));
    g.drawLine(354.0f, 27.0f, 354.0f, 41.0f, 1.7f);
    g.drawLine(348.0f, 33.0f, 360.0f, 33.0f, 1.7f);

    g.setColour(Colors::glow);
    g.setFont(juce::FontOptions(17.5f, juce::Font::bold));
    g.drawText("WITCHSYNTH", 374, 19, 170, 24, juce::Justification::left);
    g.setColour(Colors::subText);
    g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    g.drawText("v9.0 NEXUS EDITION // 100 PRESETS", 548, 23, 270, 18, juce::Justification::right);

    const juce::Rectangle<float> lcdFrame(342.0f, 84.0f, 476.0f, 232.0f);
    g.setColour(Colors::lcdBg);
    g.fillRoundedRectangle(lcdFrame, 5.0f);
    g.setColour(Colors::purple);
    g.drawRoundedRectangle(lcdFrame, 5.0f, 1.5f);

    g.setColour(juce::Colour(0xff1a122e));
    g.fillRect(344, 86, 472, 19);
    g.setColour(Colors::neon);
    g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    g.drawText("CATEGORY", 352, 87, 160, 17, juce::Justification::left);
    g.drawText("PRESET", 534, 87, 260, 17, juce::Justification::left);

    g.setColour(juce::Colour(0xff171029));
    g.fillRect(344, 291, 472, 23);
    const int curIdx = audioProcessor.getCurrentPresetIndex();
    const auto& allP = getAllPresets();
    const juce::String curName = (curIdx >= 0 && curIdx < static_cast<int>(allP.size())) ? allP[static_cast<size_t>(curIdx)].name : "";
    g.setColour(Colors::glow);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.drawText(curName, 352, 292, 340, 21, juce::Justification::left, true);
    g.setColour(Colors::subText);
    g.setFont(juce::FontOptions(10.5f));
    g.drawText(juce::String(curIdx + 1) + " / " + juce::String(allP.size()), 700, 292, 110, 21, juce::Justification::right);

    const juce::Rectangle<float> scopeRect(636.0f, 324.0f, 182.0f, 58.0f);
    g.setColour(Colors::lcdBg);
    g.fillRoundedRectangle(scopeRect, 4.0f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(scopeRect, 4.0f, 1.0f);

    juce::Path wave;
    const int writePos = audioProcessor.scopeWritePos.load(std::memory_order_relaxed);
    for (int i = 0; i < WitchSynthAudioProcessor::scopeSize; ++i)
    {
        const int idx = (writePos + i) % WitchSynthAudioProcessor::scopeSize;
        const float sample = juce::jlimit(-1.0f, 1.0f, audioProcessor.scopeBuffer[static_cast<size_t>(idx)]);
        const float px = scopeRect.getX() + 3.0f + (static_cast<float>(i) / static_cast<float>(WitchSynthAudioProcessor::scopeSize - 1)) * (scopeRect.getWidth() - 6.0f);
        const float py = scopeRect.getCentreY() - sample * (scopeRect.getHeight() * 0.42f);
        if (i == 0) wave.startNewSubPath(px, py);
        else wave.lineTo(px, py);
    }
    const auto scopeCol = smoothedPeak > 0.85f ? Colors::hotPink : Colors::purple;
    g.setColour(scopeCol.withAlpha(0.45f));
    g.strokePath(wave, juce::PathStrokeType(3.6f));
    g.setColour(Colors::glow);
    g.strokePath(wave, juce::PathStrokeType(1.4f));

    drawPanel(12,   404, 248, 266, "PREAMP / LO-FI");
    drawPanel(270,  404, 186, 266, "CHORUS");
    drawPanel(466,  404, 250, 266, "DELAY");
    drawPanel(726,  404, 268, 266, "REVERB & SHIMMER");
    drawPanel(1004, 404, 144, 266, "OUTPUT");

    const juce::Rectangle<float> meterRect(1116.0f, 444.0f, 14.0f, 142.0f);
    g.setColour(Colors::lcdBg);
    g.fillRoundedRectangle(meterRect, 3.0f);
    g.setColour(Colors::border);
    g.drawRoundedRectangle(meterRect, 3.0f, 1.0f);
    const float fillH = juce::jlimit(0.0f, 1.0f, smoothedPeak) * meterRect.getHeight();
    g.setColour(smoothedPeak > 0.92f ? Colors::hotPink : Colors::neon);
    g.fillRoundedRectangle(meterRect.getX() + 1.0f, meterRect.getBottom() - fillH, meterRect.getWidth() - 2.0f, fillH, 2.0f);

    g.setColour(juce::Colour(0xff0d0a17));
    g.fillRoundedRectangle(12.0f, 678.0f, 1136.0f, 32.0f, 5.0f);
    g.setColour(Colors::purple.withAlpha(0.65f));
    g.drawRoundedRectangle(12.0f, 678.0f, 1136.0f, 32.0f, 5.0f, 1.0f);
    g.setColour(Colors::glow);
    g.setFont(juce::FontOptions(12.5f));
    g.drawText(u8("ПОДСКАЗКА: ") + activeHelpText, 22, 680, 1116, 28, juce::Justification::left, true);
}

void WitchSynthAudioProcessorEditor::resized()
{
    pageMainBtn.setBounds(342, 54, 115, 23);
    pageOscBtn.setBounds(462, 54, 115, 23);
    pageArpBtn.setBounds(582, 54, 115, 23);
    pageXyBtn.setBounds(702, 54, 116, 23);

    categoryList.setBounds(344, 106, 180, 184);
    presetList.setBounds(528, 106, 288, 184);

    prevBtn.setBounds(342, 324, 34, 26);
    nextBtn.setBounds(380, 324, 34, 26);
    likeBtn.setBounds(418, 324, 52, 26);
    rndBtn.setBounds(474, 324, 72, 26);
    initBtn.setBounds(550, 324, 76, 26);
    lockFxBtn.setBounds(342, 356, 138, 26);
    panicBtn.setBounds(486, 356, 140, 26);

    filterTypeBox.setBounds(24, 42, 286, 24);
    filterCutoff.setBounds(22, 102, 68, 66);
    filterReso.setBounds(96, 102, 68, 66);
    filterDrive.setBounds(170, 102, 68, 66);
    filterEnvAmt.setBounds(244, 102, 68, 66);
    filterDecay.setBounds(22, 218, 68, 66);
    filterKeyTrack.setBounds(96, 218, 68, 66);
    tiltAmt.setBounds(170, 218, 68, 66);
    stereoWidth.setBounds(244, 218, 68, 66);

    polyModeBox.setBounds(850, 42, 92, 24);
    morphPresetBox.setBounds(948, 42, 188, 24);
    ampA.setBounds(848, 102, 68, 66);
    ampD.setBounds(922, 102, 68, 66);
    ampS.setBounds(996, 102, 68, 66);
    ampR.setBounds(1070, 102, 68, 66);
    glideTime.setBounds(848, 218, 68, 66);
    snapAmt.setBounds(922, 218, 68, 66);
    airAmt.setBounds(996, 218, 68, 66);
    morphAmt.setBounds(1070, 218, 68, 66);

    osc1WaveBox.setBounds(24, 42, 138, 24);
    osc2WaveBox.setBounds(170, 42, 138, 24);
    osc1Oct.setBounds(18, 102, 56, 62);
    osc1Semi.setBounds(78, 102, 56, 62);
    pwmWidth.setBounds(138, 102, 56, 62);
    xModAmt.setBounds(198, 102, 56, 62);
    osc1Detune.setBounds(258, 102, 56, 62);
    osc1Level.setBounds(18, 218, 56, 62);
    osc2Oct.setBounds(78, 218, 56, 62);
    osc2Semi.setBounds(138, 218, 56, 62);
    osc2Unison.setBounds(198, 218, 56, 62);
    osc2Level.setBounds(258, 218, 56, 62);

    subLevel.setBounds(884, 102, 82, 70);
    entropyAmt.setBounds(1018, 102, 82, 70);
    gravityAmt.setBounds(884, 224, 82, 70);
    ghostVoxAmt.setBounds(1018, 224, 82, 70);

    lfoTargetBox.setBounds(24, 42, 138, 24);
    lfoShapeBox.setBounds(170, 42, 138, 24);
    lfoRate.setBounds(30, 102, 74, 66);
    lfoDepth.setBounds(130, 102, 74, 66);
    pitchPunch.setBounds(230, 102, 74, 66);
    pumpAmt.setBounds(30, 218, 74, 66);
    noiseLevel.setBounds(130, 218, 74, 66);
    vhsDrift.setBounds(230, 218, 74, 66);

    arpModeBox.setBounds(850, 42, 286, 24);
    arpRate.setBounds(952, 98, 82, 64);
    stepMatrix.setBounds(852, 206, 282, 66);

    xyPad.setBounds(26, 46, 282, 330);

    distBtn.setBounds(22, 434, 72, 22);
    distModeBox.setBounds(98, 434, 152, 22);
    distDrive.setBounds(44, 486, 66, 60);
    distMix.setBounds(160, 486, 66, 60);
    crushAmount.setBounds(44, 582, 66, 60);
    lowCutHz.setBounds(160, 582, 66, 60);

    chorusBtn.setBounds(280, 434, 100, 22);
    chorusMix.setBounds(288, 486, 66, 60);
    chorusRate.setBounds(372, 486, 66, 60);
    chorusDepth.setBounds(330, 582, 66, 60);

    delayBtn.setBounds(476, 434, 78, 22);
    delayModeBox.setBounds(558, 434, 146, 22);
    delayTime.setBounds(498, 486, 66, 60);
    delayFb.setBounds(614, 486, 66, 60);
    delayMix.setBounds(556, 582, 66, 60);

    reverbBtn.setBounds(736, 434, 90, 22);
    freezeBtn.setBounds(894, 434, 88, 22);
    reverbSize.setBounds(768, 486, 66, 60);
    reverbMix.setBounds(886, 486, 66, 60);
    haloAmt.setBounds(827, 582, 66, 60);

    masterGain.setBounds(1022, 464, 78, 72);
    tapeStopBtn.setBounds(1018, 602, 116, 36);
}
