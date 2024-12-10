import React, {useEffect, useState} from 'react';
import {Routes, Route} from 'react-router-dom';
import FoldersOverview, {Folder} from '../components/pages/FoldersOverview';
import AddNewFolder from '../components/pages/AddNewFolder';
import AddNewFile from '../components/pages/AddNewFile';
import storageManager from "../Managers/ShowStorageManager";
import {Show} from "../components/serialization/Show";
import {GridLayout} from '../components/serialization/Layout';



const FoldersOverviewContainer: React.FC = () => {
    const [loading, setLoading] = useState(true);
    const [folders, setFolders] = useState<Folder[] | null>(null);

    useEffect(() => {
        const fetchFolders = async () => {
            setFolders(await storageManager.listAllShows());
            setLoading(false);
        };

        fetchFolders();
    }, []);


    const addNewFolder = (folderName: string) => {
        storageManager.createFolder(folderName);
        setFolders([...folders!, {name: folderName, files: []}]);
    };

    const addNewFile = (fileName: string, folderName: string, width: number, height: number) => {
        // TODO: Just navigate to configurator with a new show name?
        //  Only save to disk and ask for a name on first save?
        // FIXME: This is the same translation as in Show.ts:getFileName. Should be in one place.
        //  This wouldn't matter if we just navigated to the configurator with a new show name,
        //  then the save show logic would handle the rest.
        // TODO: Pre-set duration. Allow changing in the UI & First setting it in the
        //  configurator if not already set.
        const show = new Show(fileName);
        const layout = new GridLayout(width, height);
        show.addLayout(layout);
        storageManager.saveShow(`${folderName}/${fileName.replace(/ /g, '_')}`, show);
        storageManager.listShows().then(folders => setFolders(folders));
    };

    const loadShow = (path: string) => {
        return storageManager.loadShow(path);
    }

    return (
        <Routes>
            <Route
                index
                element={
                    <FoldersOverview
                        folders={folders}
                        loading={loading}
                        onAddFolder={addNewFolder}
                        onAddFile={addNewFile}
                        loadShow={loadShow}
                    />
                }
            />
            <Route path="add-folder" element={<AddNewFolder onAddFolder={addNewFolder}/>}/>
            <Route path="add-file" element={<AddNewFile folders={folders!} onAddFile={addNewFile}/>}/>
        </Routes>
    );
};

export default FoldersOverviewContainer;
