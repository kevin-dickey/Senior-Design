import React, {useEffect, useState} from 'react';
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

        // Skip folders
        if (file.endsWith('/')) {
            return acc;
        }

        try {
            storageManager.loadShow(file)
                .then(show => {
                    console.log(`Loaded show: ${show.name}`);
                    folder!.files.push({
                        name: show.name,
                        path: file
                    })
                })
        } catch (e) {
            console.error(`Could not load show: ${file} - ${e}`);
        }

        return acc;
    }, []);
}

const FoldersOverviewContainer: React.FC = () => {
    const [loading, setLoading] = useState(true);
    const [folders, setFolders] = useState<Folder[] | null>(null);

    useEffect(() => {
        const fetchFolders = async () => {
            const folders = await loadFolders();
            setFolders(folders);
            setLoading(false);
        };

        fetchFolders();
    }, []);

    const loadFolders = async () => {
        let loadedFolders: Folder[] = [];
        loadedFolders = loadedFolders.concat(reduceFiles(storageManager.listShows()));

        const shows = await storageManager.listExampleShows()
        const exampleFolders = reduceFiles(shows);
        loadedFolders = loadedFolders.concat(exampleFolders);
        return loadedFolders;
    };

    const addNewFolder = (folderName: string) => {
        storageManager.createFolder(folderName);
        setFolders([...folders!, {name: folderName, files: []}]);
    };

    const addNewFile = (fileName: string, folderName: string) => {
        // TODO: Just navigate to configurator with a new show name?
        //  Only save to disk and ask for a name on first save?
        // FIXME: This is the same translation as in Show.ts:getFileName. Should be in one place.
        //  This wouldn't matter if we just navigated to the configurator with a new show name,
        //  then the save show logic would handle the rest.
        // TODO: Pre-set duration. Allow changing in the UI & First setting it in the
        //  configurator if not already set.
        storageManager.saveShow(`${folderName}/${fileName.replace(/ /g, '_')}`, new Show(fileName, 5000));
        setFolders(reduceFiles(storageManager.listShows()));
    };

    const loadShow = (path: string) => {
        return storageManager.loadShow(path);
    }

    return (
        <Routes>
            <Route path="/shows">
                <Route
                    index element={
                    <FoldersOverview
                        folders={folders}
                        loading={loading}
                        onAddFolder={addNewFolder}
                        onAddFile={addNewFile}
                        loadShow={loadShow}
                    />
                }/>
                <Route path="new-folder" element={<AddNewFolder onAddFolder={addNewFolder}/>}/>
                <Route
                    path="new"
                    element={
                        <AddNewFile
                            folders={folders!}
                            onAddFile={addNewFile}
                        />
                    }
                />
            </Route>
        </Routes>
    );
};

export default FoldersOverviewContainer;
