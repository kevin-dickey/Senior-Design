import React, { useState } from 'react';
import { Routes, Route } from 'react-router-dom';
import FoldersOverview, { Folder } from '../components/pages/FoldersOverview';
import AddNewFolder from '../components/pages/AddNewFolder';
import AddNewFile from '../components/pages/AddNewFile';
import storageManager from "../Managers/ShowStorageManager";
import { Show } from "../components/serialization/Show";
import { GridLayout } from '../components/serialization/Layout';
import {DialogContainer} from "./DialogContainer";

const reduceFiles = (files: string[]): Folder[] => {
    return files.reduce((acc: Folder[], file: string) => {
        const parts = file.split('/');
        const folderName = parts[0];

        // Find the folder in the accumulator
        let folder = acc.find(f => f.name === folderName);
        if (!folder) {
            folder = { name: folderName, files: [] };
            acc.push(folder);
        }

        // Skip folders
        if (file.endsWith('/')) {
            return acc;
        }

        try {
            folder.files.push({
                name: storageManager.loadShow(file).name,
                path: file,
            });
        } catch (e) {
            console.error(`Could not load show: ${file} - ${e}`);
        }

        return acc;
    }, []);
}

const FoldersOverviewContainer: React.FC = () => {
    const [folders, setFolders] = useState<Folder[]>(reduceFiles(storageManager.listShows()));

    const addNewFolder = (folderName: string) => {
        storageManager.createFolder(folderName);
        setFolders([...folders, { name: folderName, files: [] }]);
    };

    const addNewFile = (fileName: string, folderName: string, width: number, height: number) => {
        // TODO: Just navigate to configurator with a new show name?
        //  Only save to disk and ask for a name on first save?
        // FIXME: This is the same translation as in Show.ts:getFileName. Should be in one place.
        //  This wouldn't matter if we just navigated to the configurator with a new show name,
        //  then the save show logic would handle the rest.
        // TODO: Pre-set duration. Allow changing in the UI & First setting it in the
        //  configurator if not already set.
        const show = new Show(fileName, 5000);
        const layout = new GridLayout(width, height);
        show.addLayout(layout);
        storageManager.saveShow(`${folderName}/${fileName.replace(/ /g, '_')}`, show);
        setFolders(reduceFiles(storageManager.listShows()));
    };

    return (
        <Routes>
            <Route
                index
                element={
                <>
                    <FoldersOverview
                        folders={folders}
                        onAddFolder={addNewFolder}
                        onAddFile={addNewFile}
                    />
                    <DialogContainer />
                </>
                }
            />
            <Route path="add-folder" element={<AddNewFolder onAddFolder={addNewFolder} />} />
            <Route path="add-file" element={<AddNewFile folders={folders} onAddFile={addNewFile} />} />
        </Routes>
    );
};

export default FoldersOverviewContainer;
