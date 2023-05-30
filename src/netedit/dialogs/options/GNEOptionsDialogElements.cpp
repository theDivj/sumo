/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEOptionsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/options/OptionsCont.h>

#include "GNEOptionsDialogElements.h"
#include "GNEOptionsDialog.h"


#define MARGING 4
#define MINNAMEWIDTH 200

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEOptionsDialogElements::InputOption) InputOptionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GNEOptionsDialogElements::InputOption::onCmdSetOption),
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEOptionsDialogElements::InputOption, FXHorizontalFrame,  InputOptionMap, ARRAYNUMBER(InputOptionMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEOptionsDialogElements::InputOption::InputOption(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic, 
        const std::string& name, const std::string& description) :
    FXHorizontalFrame(parent, GUIDesignAuxiliarHorizontalFrame),
    myGUIDialogOptions(GUIDialogOptions),
    myTopic(topic),
    myName(name),
    myDescription(description) {
    // build label with name (default width 150)
    myNameLabel = new MFXLabelTooltip(this, myGUIDialogOptions->myMainWindowParent->getStaticTooltipMenu(), name.c_str(), nullptr, GUIDesignLabelThickedFixed(MINNAMEWIDTH));
    // set description as tooltip
    myNameLabel->setTipText(description.c_str());
    // create content frame
    myContentFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create reset button
    myResetButton = new FXButton(this, (std::string("\t\t") + TL("Reset value")).c_str(), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
}


void
GNEOptionsDialogElements::InputOption::adjustNameSize() {
    const int nameWidth = myNameLabel->getFont()->getTextWidth(myNameLabel->getText().text(), myNameLabel->getText().length() + MARGING);
    if (nameWidth > MINNAMEWIDTH) {
        myNameLabel->setWidth(nameWidth);
    }
}


long
GNEOptionsDialogElements::InputOption::onCmdSetOption(FXObject*, FXSelector, void*) {
    // try to set option and mark as modified if was sucessfully
    if (setOption()) {
        myGUIDialogOptions->myModified = true;
    }
    return 1;
}


const std::string&
GNEOptionsDialogElements::InputOption::getTopic() const {
    return myTopic;
}


const std::string
GNEOptionsDialogElements::InputOption::getNameLower() const {
    std::string lowerName = myName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    return lowerName;
}


const std::string
GNEOptionsDialogElements::InputOption::getDescriptionLower() const {
    std::string lowerDescription = myDescription;
    std::transform(lowerDescription.begin(), lowerDescription.end(), lowerDescription.begin(), ::tolower);
    return lowerDescription;
}


GNEOptionsDialogElements::InputString::InputString(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


bool
GNEOptionsDialogElements::InputString::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputStringVector::InputStringVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getStringVector(name)).c_str());
}


bool
GNEOptionsDialogElements::InputStringVector::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputBool::InputBool(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myCheckButton = new FXCheckButton(myContentFrame, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    if (myGUIDialogOptions->myOptionsContainer->getBool(name)) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


bool
GNEOptionsDialogElements::InputBool::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    if (myCheckButton->getCheck()) {
        myGUIDialogOptions->myOptionsContainer->set(myName, "true");
        myCheckButton->setText(TL("true"));
    } else {
        myGUIDialogOptions->myOptionsContainer->set(myName, "false");
        myCheckButton->setText(TL("false"));
    }
    // special checks for Debug flags
    if ((myName == "gui-testing-debug") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug")) {
        MsgHandler::enableDebugMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug"));
    }
    if ((myName == "gui-testing-debug-gl") && myGUIDialogOptions->myOptionsContainer->isSet("gui-testing-debug-gl")) {
        MsgHandler::enableDebugGLMessages(myGUIDialogOptions->myOptionsContainer->getBool("gui-testing-debug-gl"));
    }
    return true;
}


GNEOptionsDialogElements::InputInt::InputInt(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getInt(name)).c_str());
}


bool
GNEOptionsDialogElements::InputInt::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputIntVector::InputIntVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent,
        const std::string& topic, const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getIntVector(name)).c_str());
}


bool
GNEOptionsDialogElements::InputIntVector::setOption() {
    try {
        // check that int vector can be parsed
        const auto intVector = StringTokenizer(myTextField->getText().text()).getVector();
        for (const auto& intValue : intVector) {
            StringUtils::toInt(intValue);
        }
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
        myTextField->setTextColor(FXRGB(0, 0, 0));
        return true;
    } catch (...) {
        myTextField->setTextColor(FXRGB(255, 0, 0));
    }
    return false;
}


GNEOptionsDialogElements::InputFloat::InputFloat(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myTextField->setText(toString(myGUIDialogOptions->myOptionsContainer->getFloat(name)).c_str());
}


bool
GNEOptionsDialogElements::InputFloat::setOption() {
    myGUIDialogOptions->myOptionsContainer->resetWritable();
    myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
    return true;
}


GNEOptionsDialogElements::InputFilename::InputFilename(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
        const std::string& name, const std::string& description) :
    InputOption(GUIDialogOptions, parent, topic, name, description) {
    myTextField = new FXTextField(myContentFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextField->setText(myGUIDialogOptions->myOptionsContainer->getString(name).c_str());
}


bool
GNEOptionsDialogElements::InputFilename::setOption() {
    if (SUMOXMLDefinitions::isValidFilename(myTextField->getText().text())) {
        myGUIDialogOptions->myOptionsContainer->resetWritable();
        myGUIDialogOptions->myOptionsContainer->set(myName, myTextField->getText().text());
        myTextField->setTextColor(FXRGB(0, 0, 0));
        return true;
    } else {
        myTextField->setTextColor(FXRGB(255, 0, 0));
        return false;
    }
}

/****************************************************************************/