import {useEffect, useState} from "react";
import {Folder} from "../components/pages/FoldersOverview";
import storageManager from "../Managers/ShowStorageManager";
import Configuration from "../components/pages/Configuration";
import {Show} from "../components/serialization/Show";
import {makeShow} from "../Managers/ConfigurationManager";
import {useLocation} from "react-router-dom";

export const ConfigurationContainer: React.FC = () => {
    const location = useLocation();

    const [loading, setLoading] = useState(true);
    const [folders, setFolders] = useState<Folder[] | null>(null);
    const [show, setShow] = useState<Show | null>(null);
    const [showPath, setShowPath] = useState<string | null>(null);
    const [savingShowAs, setSavingShowAs] = useState(false);

    useEffect(() => {
        const fetchFolders = async () => {
            const folders = await loadFolders();
            setFolders(folders);
            setLoading(false);
        };

        if (savingShowAs) {
            fetchFolders();
        }
    }, [savingShowAs]);

    useEffect(() => {
        if (location.state) {
            if (location.state.show) {
                console.log("Loading show:" + location.state.show);
                setShow(location.state.show);

                if (location.state.path) {
                    setShowPath(location.state.path);
                } else {
                    console.warn('Path not set in location state. Must prompt user for save location.');
                }
            } else if (location.state.path) {
                console.log("Loading show from path: " + location.state.path);
                storageManager.loadShow(location.state.path).then(loadedShow => setShow(loadedShow));
                setShowPath(location.state.path);
            }
        } else {
            console.warn('No location state found. Creating a new show.');
            setShow(makeShow());
        }
        setLoading(false);
    }, [location.state]);

    const loadFolders = async () => {
        let loadedFolders: Folder[] = [];
        loadedFolders = loadedFolders.concat(await storageManager.listShows());
        loadedFolders = loadedFolders.concat(await storageManager.listExampleShows());
        return loadedFolders;
    };

    const saveShow = async (newPath?: string, newShow?: Show) => {
         if (newShow) {
            if (!newPath) {
                console.error('New show provided but no path.');
                return;
            }
        }

        let showToSave: Show = newShow ? newShow : show!;
        let savePath: string = newPath ? newPath: showPath!;

        if (showToSave == null) {
            console.error('Show to save is null. Cannot save.');
            return;
        }

        else if (savePath.startsWith('exampleShows')) {
            console.warn('Show path is in exampleShows. Prompting user for save location.');
            setSavingShowAs(true);
            return;
        }

        console.log('Saving show ' + showToSave.name + ' to path: ' + savePath);
        storageManager.saveShow(savePath, showToSave);
        console.log('Show saved successfully');
    }

    if (loading) {
        return <div>Loading...</div>;
    }

    return (
        <Configuration
            show={show!}
            setShow={setShow}

            savingShowAs={savingShowAs}
            setSavingShowAs={setSavingShowAs}
            folders={folders!}
            onSaveShow={saveShow}
        />
    );
}