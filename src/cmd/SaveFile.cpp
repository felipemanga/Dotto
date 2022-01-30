// Copyright (c) 2021 LibreSprite Authors (cf. AUTHORS.md)
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <cmd/Command.hpp>
#include <common/Writer.hpp>
#include <common/String.hpp>
#include <fs/FileDialog.hpp>
#include <fs/FileSystem.hpp>
#include <gui/Node.hpp>

class SaveFile : public Command {
public:
    Property<String> fileName{this, "filename"};

    void showSaveDialog() {
        auto& writers = Writer::getRegistry();
        Vector<String> filters;
        for (auto& entry : writers) {
            for (auto& flag : entry.second.flags) {
                if (startsWith(flag, "*."))
                    filters.push_back(flag);
            }
        }
        inject<FileDialog> dialog;
        dialog->filterDescription = "All Formats";
        dialog->title = "Save As...";
        dialog->filters = std::move(filters);
        dialog->save();
        if (!dialog->result.empty())
            set("filename", dialog->result[0]);
    }

    void run() override {
        if (fileName->empty())
            showSaveDialog();
        if (!fileName->empty())
            FileSystem::write(fileName, doc());
    }
};

static Command::Shared<SaveFile> savefile{"savefile"};
