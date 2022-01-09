// Copyright (c) 2021 LibreSprite Authors (cf. AUTHORS.md)
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <common/Messages.hpp>
#include <common/PubSub.hpp>
#include <common/Surface.hpp>
#include <fs/FileSystem.hpp>
#include <gui/Controller.hpp>
#include <gui/Events.hpp>
#include <gui/Node.hpp>

class Button : public ui::Controller {
    PubSub<msg::Flush> pub{this};
    bool isHovering = false;

public:
    Property<String> state{this, "state", "enabled", &Button::changeState};
    void changeState() {
        std::shared_ptr<Surface> current;
        Color multiply;
        if (*state == "pressed" || *state == "active") {
            current = pressedSurface;
            multiply = pressedMultiply;
        } else if (*state == "enabled") {
            current = isHovering && *hoverSurface ? hoverSurface : normalSurface;
            multiply = isHovering ? hoverMultiply : normalMultiply;
        } else if (*state == "disabled") {
            current = disabledSurface;
            multiply = disabledMultiply;
        }
        if (multiply.a == 0 && current)
            multiply = 0xFFFFFFFF;
        node()->set("surface", current);
        node()->set("multiply", multiply);
    }

    Property<String> pressed{this, "down-src", "", &Button::reloadPressed};
    Property<std::shared_ptr<Surface>> pressedSurface{this, "down-surface"};
    void reloadPressed() {
        *pressedSurface = FileSystem::parse(*pressed);
    }

    Property<String> normal{this, "up-src", "", &Button::reloadNormal};
    Property<std::shared_ptr<Surface>> normalSurface{this, "up-surface"};
    void reloadNormal() {
        *normalSurface = FileSystem::parse(*normal);
        node()->set("surface", *normalSurface);
    }

    Property<String> hover{this, "hover-src", "", &Button::reloadHover};
    Property<std::shared_ptr<Surface>> hoverSurface{this, "hover-surface"};
    void reloadHover() {
        *hoverSurface = FileSystem::parse(*hover);
    }

    Property<String> disabled{this, "disabled-src", "", &Button::reloadDisabled};
    Property<std::shared_ptr<Surface>> disabledSurface{this, "disabled-surface"};
    void reloadDisabled() {
        *disabledSurface = FileSystem::parse(*disabled);
    }

    Property<Color> pressedMultiply{this, "down-multiply", {0,0,0,0}};
    Property<Color> normalMultiply{this, "up-multiply", {0,0,0,0}, &Button::changeNormalColor};
    void changeNormalColor() {
        node()->set("multiply", *normalMultiply);
    }
    Property<Color> hoverMultiply{this, "hover-multiply", {0,0,0,0}};
    Property<Color> disabledMultiply{this, "disabled-multiply", {0,0,0,0}};

    void on(msg::Flush& flush) {
        flush.hold(*pressedSurface);
        flush.hold(*normalSurface);
        flush.hold(*disabledSurface);
    }

    void attach() override {
        node()->addEventListener<ui::MouseDown, ui::MouseUp, ui::MouseEnter, ui::MouseLeave>(this);
    }

    void eventHandler(const ui::MouseDown&) {
        if (*state == "enabled") {
            node()->set("state", "pressed");
            set("state", "pressed");
        }
    }

    void eventHandler(const ui::MouseUp&) {
        if (*state == "pressed") {
            node()->set("state", "enabled");
            set("state", "enabled");
        }
    }

    void eventHandler(const ui::MouseEnter&) {
        isHovering = true;
        changeState();
    }

    void eventHandler(const ui::MouseLeave&) {
        isHovering = false;
        if (*state == "pressed") {
            node()->set("state", "enabled");
            set("state", "enabled");
        } else changeState();
    }
};

static ui::Controller::Shared<Button> button{"button"};
