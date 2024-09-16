import React, {useState} from 'react';
import {Routes, Route} from 'react-router-dom';
import FoldersOverview, {Folder} from '../components/pages/FoldersOverview';
import AddNewFolder from '../components/pages/AddNewFolder';
import AddNewFile from '../components/pages/AddNewFile';
import storageManager from "../Managers/ShowStorageManager";
import {Show} from "../components/serialization/Show";

const reduceFiles = (files: string[]): Folder[] => {
    return files.reduce((acc: Folder[], file: string) => {
        const parts = file.split('/');
        const folderName = parts[0];

        // Find the folder in the accumulator
        let folder = acc.find(f => f.name === folderName);
        if (!folder) {
            folder = {name: folderName, files: []};
            acc.push(folder);
        }

        folder.files.push({
            name: storageManager.loadShow(file).name,
            path: file,
        });
        return acc;
    }, []);
}

const FoldersOverviewContainer: React.FC = () => {
    const [folders, setFolders] = useState<Folder[]>(reduceFiles(storageManager.listShows()));

    const addNewFolder = (folderName: string) => {
        storageManager.createFolder(folderName);
        setFolders([...folders, {name: folderName, files: []}]);
    };

    const addNewFile = (fileName: string, folderName: string) => {
        // TODO: Just navigate to configurator with a new show name?
        //  Only save to disk and ask for a name on first save?
        storageManager.saveShow(`${folderName}/${fileName}`, new Show(fileName, 0));
        setFolders(reduceFiles(storageManager.listShows()));
    };

    return (
        <Routes>
            <Route path="/shows">
                <Route index element={
                    <FoldersOverview
                        folders={folders}
                        onAddFolder={addNewFolder}
                        onAddFile={addNewFile}
                    />
                }/>
                <Route path="new-folder" element={<AddNewFolder onAddFolder={addNewFolder}/>}/>
                <Route
                    path="new"
                    element={
                        <AddNewFile
                            folders={folders}
                            onAddFile={addNewFile}
                        />
                    }
                />
            </Route>
        </Routes>
    );
};

export default FoldersOverviewContainer;
